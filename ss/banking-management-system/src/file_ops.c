#include "bank.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <stdint.h>

/* ==================== File Operations ==================== */

int open_data_file(const char *filename, int flags) {
    int fd = open(filename, flags | O_CREAT, 0644);
    if (fd < 0) {
        perror("open_data_file");
    }
    return fd;
}

int lock_record(int fd, int record_num, short lock_type) {
    struct flock fl = {
        .l_type = lock_type,
        .l_whence = SEEK_SET,
        .l_start = record_num * RECORD_SIZE,
        .l_len = RECORD_SIZE,
        .l_pid = getpid()
    };
    int cmd = (lock_type == F_UNLCK) ? F_SETLK : F_SETLKW;
    return fcntl(fd, cmd, &fl);
}

int unlock_record(int fd, int record_num) {
    return lock_record(fd, record_num, F_UNLCK);
}

/* Non-blocking lock attempt for session reaper */
int try_lock_record(int fd, int record_num, short lock_type) {
    struct flock fl = {
        .l_type = lock_type,
        .l_whence = SEEK_SET,
        .l_start = record_num * RECORD_SIZE,
        .l_len = RECORD_SIZE,
        .l_pid = getpid()
    };
    return fcntl(fd, F_SETLK, &fl);  /* Non-blocking */
}

ssize_t read_record(int fd, int record_num, void *buf, size_t size) {
    off_t offset = record_num * RECORD_SIZE;
    if (lseek(fd, offset, SEEK_SET) == -1) return -1;
    return read(fd, buf, size);
}

ssize_t write_record(int fd, int record_num, const void *buf, size_t size) {
    off_t offset = record_num * RECORD_SIZE;
    if (lseek(fd, offset, SEEK_SET) == -1) return -1;
    
    /* Pad to RECORD_SIZE with zeros */
    char padded[RECORD_SIZE] = {0};
    memcpy(padded, buf, size < RECORD_SIZE ? size : RECORD_SIZE);
    return write(fd, padded, RECORD_SIZE);
}

int get_next_id(const char *filename, int record_size) {
    (void)record_size;
    int fd = open_data_file(filename, O_RDONLY);
    if (fd < 0) return 1;
    
    struct stat st;
    if (fstat(fd, &st) < 0) {
        close(fd);
        return 1;
    }
    
    /* File has RECORD_SIZE-byte records */
    int count = st.st_size / RECORD_SIZE;
    close(fd);
    return count + 1;
}

/* Append a record while holding a file-wide allocation lock.
 * The first int of every persisted struct is its ID. */
static int append_record(const char *filename, void *buf, size_t size) {
    int fd = open_data_file(filename, O_RDWR);
    if (fd < 0) return -1;
    
    if (lock_record(fd, 0, BANK_LOCK_WRITE) < 0) {
        close(fd);
        return -1;
    }
    
    struct stat st;
    if (fstat(fd, &st) < 0) {
        unlock_record(fd, 0);
        close(fd);
        return -1;
    }
    
    int record_num = st.st_size / RECORD_SIZE;
    int id = record_num + 1;
    memcpy(buf, &id, sizeof(id));
    
    if (write_record(fd, record_num, buf, size) != RECORD_SIZE) {
        unlock_record(fd, 0);
        close(fd);
        return -1;
    }
    
    unlock_record(fd, 0);
    close(fd);
    return id;
}

/* ==================== Session Management ==================== */

static int open_session_file(void) {
    int fd = open_data_file("data/sessions.dat", O_RDWR);
    return fd;
}

int session_create(int user_id, int role) {
    int fd = open_session_file();
    if (fd < 0) return -1;
    
    if (lock_record(fd, 0, BANK_LOCK_WRITE) < 0) {
        close(fd);
        return -1;
    }
    
    /* Read all sessions */
    Session sessions[MAX_SESSIONS] = {0};
    struct stat st;
    fstat(fd, &st);
    int count = st.st_size / RECORD_SIZE;
    if (count > MAX_SESSIONS) count = MAX_SESSIONS;
    
    for (int i = 0; i < count; i++) {
        if (read_record(fd, i, &sessions[i], sizeof(Session)) != sizeof(Session)) {
            unlock_record(fd, 0);
            close(fd);
            return -1;
        }
    }
    
    /* Reuse an empty/expired slot, or reject an active duplicate */
    int slot = -1;
    time_t now = time(NULL);
    for (int i = 0; i < count; i++) {
        if (sessions[i].active && sessions[i].user_id == user_id && sessions[i].role == role) {
            if (now - sessions[i].last_activity <= SESSION_TIMEOUT) {
                unlock_record(fd, 0);
                close(fd);
                return -1; /* Already logged in */
            }
            slot = i; /* Expired session can be replaced */
            break;
        }
        if (!sessions[i].active && slot < 0) {
            slot = i;
        }
    }
    
    if (slot < 0) {
        if (count >= MAX_SESSIONS) {
            unlock_record(fd, 0);
            close(fd);
            return -1;
        }
        slot = count;
    }
    
    Session new_session = {
        .user_id = user_id,
        .role = role,
        .active = 1,
        .last_activity = now
    };
    
    if (write_record(fd, slot, &new_session, sizeof(Session)) != RECORD_SIZE) {
        unlock_record(fd, 0);
        close(fd);
        return -1;
    }
    
    unlock_record(fd, 0);
    close(fd);
    return 0;
}

int session_destroy(int user_id, int role) {
    int fd = open_session_file();
    if (fd < 0) return -1;
    
    if (lock_record(fd, 0, BANK_LOCK_WRITE) < 0) {
        close(fd);
        return -1;
    }
    
    Session sessions[MAX_SESSIONS] = {0};
    struct stat st;
    fstat(fd, &st);
    int count = st.st_size / RECORD_SIZE;
    if (count > MAX_SESSIONS) count = MAX_SESSIONS;
    
    for (int i = 0; i < count; i++) {
        if (read_record(fd, i, &sessions[i], sizeof(Session)) != sizeof(Session)) {
            unlock_record(fd, 0);
            return -1;
        }
    }
    
    for (int i = 0; i < count; i++) {
        if (sessions[i].active && sessions[i].user_id == user_id && sessions[i].role == role) {
            sessions[i].active = 0;
            if (write_record(fd, i, &sessions[i], sizeof(Session)) != RECORD_SIZE) {
                unlock_record(fd, 0);
                return -1;
            }
            break;
        }
    }
    
    unlock_record(fd, 0);
    close(fd);
    return 0;
}

int session_is_active(int user_id, int role) {
    int fd = open_session_file();
    if (fd < 0) return 0;
    
    /* Write lock: an expired session is destroyed during the check. */
    if (lock_record(fd, 0, BANK_LOCK_WRITE) < 0) {
        close(fd);
        return 0;
    }
    
    Session sessions[MAX_SESSIONS] = {0};
    struct stat st;
    fstat(fd, &st);
    int count = st.st_size / RECORD_SIZE;
    if (count > MAX_SESSIONS) count = MAX_SESSIONS;
    
    for (int i = 0; i < count; i++) {
        if (read_record(fd, i, &sessions[i], sizeof(Session)) != sizeof(Session)) {
            unlock_record(fd, 0);
            close(fd);
            return 0;
        }
    }
    
    for (int i = 0; i < count; i++) {
        if (sessions[i].active && sessions[i].user_id == user_id && sessions[i].role == role) {
            /* Check if session has timed out */
            time_t now = time(NULL);
            if (now - sessions[i].last_activity > SESSION_TIMEOUT) {
                /* Session expired - destroy it */
                sessions[i].active = 0;
                write_record(fd, i, &sessions[i], sizeof(Session));
                unlock_record(fd, 0);
                close(fd);
                return 0;
            }
            unlock_record(fd, 0);
            close(fd);
            return 1;
        }
    }
    
    unlock_record(fd, 0);
    close(fd);
    return 0;
}

/* Update session last_activity timestamp */
int session_update_activity(int user_id, int role) {
    int fd = open_session_file();
    if (fd < 0) return -1;
    
    if (lock_record(fd, 0, BANK_LOCK_WRITE) < 0) {
        close(fd);
        return -1;
    }
    
    Session sessions[MAX_SESSIONS] = {0};
    struct stat st;
    fstat(fd, &st);
    int count = st.st_size / RECORD_SIZE;
    if (count > MAX_SESSIONS) count = MAX_SESSIONS;
    
    for (int i = 0; i < count; i++) {
        if (read_record(fd, i, &sessions[i], sizeof(Session)) != sizeof(Session)) {
            unlock_record(fd, 0);
            close(fd);
            return -1;
        }
    }
    
    for (int i = 0; i < count; i++) {
        if (sessions[i].active && sessions[i].user_id == user_id && sessions[i].role == role) {
            sessions[i].last_activity = time(NULL);
            if (write_record(fd, i, &sessions[i], sizeof(Session)) != RECORD_SIZE) {
                unlock_record(fd, 0);
                close(fd);
                return -1;
            }
            break;
        }
    }
    
    unlock_record(fd, 0);
    close(fd);
    return 0;
}

/* Session reaper - expires all sessions older than SESSION_TIMEOUT */
int session_reaper(void) {
    int fd = open_session_file();
    if (fd < 0) return -1;
    
    /* Non-blocking lock - if busy, skip this reaper cycle */
    if (try_lock_record(fd, 0, BANK_LOCK_WRITE) < 0) {
        close(fd);
        return 0;  /* Not an error, just couldn't get lock */
    }
    
    Session sessions[MAX_SESSIONS] = {0};
    struct stat st;
    fstat(fd, &st);
    int count = st.st_size / RECORD_SIZE;
    if (count > MAX_SESSIONS) count = MAX_SESSIONS;
    
    for (int i = 0; i < count; i++) {
        if (read_record(fd, i, &sessions[i], sizeof(Session)) != sizeof(Session)) {
            unlock_record(fd, 0);
            close(fd);
            return -1;
        }
    }
    
    time_t now = time(NULL);
    int expired = 0;
    for (int i = 0; i < count; i++) {
        if (sessions[i].active && (now - sessions[i].last_activity > SESSION_TIMEOUT)) {
            sessions[i].active = 0;
            if (write_record(fd, i, &sessions[i], sizeof(Session)) != RECORD_SIZE) {
                unlock_record(fd, 0);
                close(fd);
                return -1;
            }
            expired++;
        }
    }
    
    unlock_record(fd, 0);
    close(fd);
    return expired;
}

/* ==================== Customer Operations ==================== */

int customer_login(int user_id, const char *password) {
    int fd = open_data_file("data/customers.dat", O_RDWR);
    if (fd < 0) return 0;
    
    Customer cust;
    if (read_record(fd, user_id - 1, &cust, sizeof(Customer)) != sizeof(Customer)) {
        close(fd);
        return 0;
    }
    
    int success = (strcmp(cust.password, password) == 0 && cust.account_status == ACC_STATUS_ACTIVE);
    close(fd);
    
    if (success) {
        if (session_create(user_id, ROLE_CUSTOMER) != 0) {
            return 0; /* Already logged in elsewhere */
        }
    }
    return success;
}

int customer_balance(int user_id, int64_t *balance) {
    int fd = open_data_file("data/customers.dat", O_RDWR);
    if (fd < 0) return -1;
    
    if (lock_record(fd, user_id - 1, BANK_LOCK_READ) < 0) {
        close(fd);
        return -1;
    }
    
    Customer cust;
    if (read_record(fd, user_id - 1, &cust, sizeof(Customer)) != sizeof(Customer)) {
        unlock_record(fd, user_id - 1);
        close(fd);
        return -1;
    }
    
    *balance = cust.balance;
    unlock_record(fd, user_id - 1);
    close(fd);
    return 0;
}

int customer_deposit(int user_id, int64_t amount) {
    if (amount <= 0) return -1;
    
    int fd = open_data_file("data/customers.dat", O_RDWR);
    if (fd < 0) return -1;
    
    if (lock_record(fd, user_id - 1, BANK_LOCK_WRITE) < 0) {
        close(fd);
        return -1;
    }
    
    Customer cust;
    if (read_record(fd, user_id - 1, &cust, sizeof(Customer)) != sizeof(Customer)) {
        unlock_record(fd, user_id - 1);
        close(fd);
        return -1;
    }
    
    if (cust.account_status != ACC_STATUS_ACTIVE) {
        unlock_record(fd, user_id - 1);
        close(fd);
        return -1;
    }
    
    cust.balance += amount;
    if (write_record(fd, user_id - 1, &cust, sizeof(Customer)) != RECORD_SIZE) {
        unlock_record(fd, user_id - 1);
        close(fd);
        return -1;
    }
    
    unlock_record(fd, user_id - 1);
    close(fd);
    
    log_transaction(user_id, TXN_DEPOSIT, amount, 0);
    return 0;
}

int customer_withdraw(int user_id, int64_t amount) {
    if (amount <= 0) return -1;
    
    int fd = open_data_file("data/customers.dat", O_RDWR);
    if (fd < 0) return -1;
    
    if (lock_record(fd, user_id - 1, BANK_LOCK_WRITE) < 0) {
        close(fd);
        return -1;
    }
    
    Customer cust;
    if (read_record(fd, user_id - 1, &cust, sizeof(Customer)) != sizeof(Customer)) {
        unlock_record(fd, user_id - 1);
        close(fd);
        return -1;
    }
    
    if (cust.account_status != ACC_STATUS_ACTIVE) {
        unlock_record(fd, user_id - 1);
        close(fd);
        return -1;
    }
    
    if (cust.balance < amount) {
        unlock_record(fd, user_id - 1);
        close(fd);
        return -1;
    }
    
    cust.balance -= amount;
    if (write_record(fd, user_id - 1, &cust, sizeof(Customer)) != RECORD_SIZE) {
        unlock_record(fd, user_id - 1);
        close(fd);
        return -1;
    }
    
    unlock_record(fd, user_id - 1);
    close(fd);
    
    log_transaction(user_id, TXN_WITHDRAW, amount, 0);
    return 0;
}

int customer_transfer(int from_id, int to_id, int64_t amount) {
    if (amount <= 0 || from_id == to_id) return -1;
    
    int fd = open_data_file("data/customers.dat", O_RDWR);
    if (fd < 0) return -1;
    
    /* Lock both records in consistent order to avoid deadlock */
    int first = (from_id < to_id) ? from_id : to_id;
    int second = (from_id < to_id) ? to_id : from_id;
    
    if (lock_record(fd, first - 1, BANK_LOCK_WRITE) < 0) {
        close(fd);
        return -1;
    }
    if (lock_record(fd, second - 1, BANK_LOCK_WRITE) < 0) {
        unlock_record(fd, first - 1);
        close(fd);
        return -1;
    }
    
    Customer from_cust, to_cust;
    if (read_record(fd, from_id - 1, &from_cust, sizeof(Customer)) != sizeof(Customer) ||
        read_record(fd, to_id - 1, &to_cust, sizeof(Customer)) != sizeof(Customer)) {
        unlock_record(fd, first - 1);
        unlock_record(fd, second - 1);
        close(fd);
        return -1;
    }
    
    if (from_cust.balance < amount || from_cust.account_status != ACC_STATUS_ACTIVE || 
        to_cust.account_status != ACC_STATUS_ACTIVE) {
        unlock_record(fd, first - 1);
        unlock_record(fd, second - 1);
        close(fd);
        return -1;
    }
    
    from_cust.balance -= amount;
    to_cust.balance += amount;
    
    if (write_record(fd, from_id - 1, &from_cust, sizeof(Customer)) != RECORD_SIZE ||
        write_record(fd, to_id - 1, &to_cust, sizeof(Customer)) != RECORD_SIZE) {
        unlock_record(fd, first - 1);
        unlock_record(fd, second - 1);
        close(fd);
        return -1;
    }
    
    unlock_record(fd, first - 1);
    unlock_record(fd, second - 1);
    close(fd);
    
    log_transaction(from_id, TXN_TRANSFER, amount, to_id);
    log_transaction(to_id, TXN_DEPOSIT, amount, from_id);
    return 0;
}

int customer_change_password(int user_id, const char *old_pass, const char *new_pass) {
    int fd = open_data_file("data/customers.dat", O_RDWR);
    if (fd < 0) return -1;
    
    if (lock_record(fd, user_id - 1, BANK_LOCK_WRITE) < 0) {
        close(fd);
        return -1;
    }
    
    Customer cust;
    if (read_record(fd, user_id - 1, &cust, sizeof(Customer)) != sizeof(Customer)) {
        unlock_record(fd, user_id - 1);
        close(fd);
        return -1;
    }
    
    if (cust.account_status != ACC_STATUS_ACTIVE) {
        unlock_record(fd, user_id - 1);
        close(fd);
        return -1;
    }
    
    if (strcmp(cust.password, old_pass) != 0) {
        unlock_record(fd, user_id - 1);
        close(fd);
        return -1;
    }
    
    strncpy(cust.password, new_pass, MAX_PASSWORD_LEN - 1);
    cust.password[MAX_PASSWORD_LEN - 1] = '\0';
    
    if (write_record(fd, user_id - 1, &cust, sizeof(Customer)) != RECORD_SIZE) {
        unlock_record(fd, user_id - 1);
        close(fd);
        return -1;
    }
    
    unlock_record(fd, user_id - 1);
    close(fd);
    return 0;
}

int customer_loan_apply(int user_id, int64_t amount) {
    if (amount <= 0) return -1;
    
    int fd = open_data_file("data/customers.dat", O_RDWR);
    if (fd < 0) return -1;
    
    if (lock_record(fd, user_id - 1, BANK_LOCK_WRITE) < 0) {
        close(fd);
        return -1;
    }
    
    Customer cust;
    if (read_record(fd, user_id - 1, &cust, sizeof(Customer)) != sizeof(Customer)) {
        unlock_record(fd, user_id - 1);
        close(fd);
        return -1;
    }
    
    if (cust.account_status != ACC_STATUS_ACTIVE) {
        unlock_record(fd, user_id - 1);
        close(fd);
        return -1;
    }
    
    if (cust.loan_status == LOAN_PENDING) {
        unlock_record(fd, user_id - 1);
        close(fd);
        return -1;
    }
    
    int old_loan_status = cust.loan_status;
    int64_t old_loan_amount = cust.loan_amount;
    
    cust.loan_status = LOAN_PENDING;
    cust.loan_amount = amount;
    
    if (write_record(fd, user_id - 1, &cust, sizeof(Customer)) != RECORD_SIZE) {
        unlock_record(fd, user_id - 1);
        close(fd);
        return -1;
    }
    
    Loan loan = {
        .customer_id = user_id,
        .amount = amount,
        .status = LOAN_PENDING,
        .assigned_employee_id = -1
    };
    
    if (append_record("data/loans.dat", &loan, sizeof(Loan)) < 0) {
        cust.loan_status = old_loan_status;
        cust.loan_amount = old_loan_amount;
        write_record(fd, user_id - 1, &cust, sizeof(Customer));
        unlock_record(fd, user_id - 1);
        close(fd);
        return -1;
    }
    
    unlock_record(fd, user_id - 1);
    close(fd);
    return 0;
}

int customer_txn_history(int user_id, Transaction *txns, int max_txns) {
    int fd = open_data_file("data/transactions.dat", O_RDONLY);
    if (fd < 0) return 0;
    
    struct stat st;
    fstat(fd, &st);
    int total = st.st_size / RECORD_SIZE;
    int count = 0;
    
    for (int i = total - 1; i >= 0 && count < max_txns; i--) {
        Transaction txn;
        if (read_record(fd, i, &txn, sizeof(Transaction)) == sizeof(Transaction)) {
            if (txn.customer_id == user_id) {
                txns[count++] = txn;
            }
        }
    }
    
    close(fd);
    return count;
}

int customer_feedback(int user_id, const char *message) {
    Feedback fb = {
        .customer_id = user_id,
        .status = 0
    };
    strncpy(fb.message, message, MAX_MESSAGE_LEN - 1);
    fb.message[MAX_MESSAGE_LEN - 1] = '\0';
    
    return (append_record("data/feedback.dat", &fb, sizeof(Feedback)) > 0) ? 0 : -1;
}

/* ==================== Employee Operations ==================== */

int employee_login(int emp_id, const char *password) {
    int fd = open_data_file("data/employees.dat", O_RDWR);
    if (fd < 0) return 0;
    
    Employee emp;
    if (read_record(fd, emp_id - 1, &emp, sizeof(Employee)) != sizeof(Employee)) {
        close(fd);
        return 0;
    }
    
    int success = (strcmp(emp.password, password) == 0 && emp.status == 0);
    close(fd);
    
    if (success) {
        if (session_create(emp_id, ROLE_EMPLOYEE) != 0) {
            return 0; /* Already logged in elsewhere */
        }
    }
    return success;
}

int employee_add_customer(const Customer *cust) {
    Customer new_cust;
    memset(&new_cust, 0, sizeof(new_cust));
    
    memcpy(new_cust.first_name, cust->first_name, MAX_NAME_LEN - 1);
    memcpy(new_cust.last_name, cust->last_name, MAX_NAME_LEN - 1);
    memcpy(new_cust.password, cust->password, MAX_PASSWORD_LEN - 1);
    new_cust.balance = 0;
    new_cust.loan_status = LOAN_REJECTED;
    new_cust.loan_amount = 0;
    new_cust.account_status = ACC_STATUS_ACTIVE;
    
    return append_record("data/customers.dat", &new_cust, sizeof(Customer));
}

int employee_modify_customer(int cust_id, const Customer *cust) {
    int fd = open_data_file("data/customers.dat", O_RDWR);
    if (fd < 0) return -1;
    
    if (lock_record(fd, cust_id - 1, BANK_LOCK_WRITE) < 0) {
        close(fd);
        return -1;
    }
    
    Customer existing;
    if (read_record(fd, cust_id - 1, &existing, sizeof(Customer)) != sizeof(Customer)) {
        unlock_record(fd, cust_id - 1);
        close(fd);
        return -1;
    }
    
    memcpy(existing.first_name, cust->first_name, MAX_NAME_LEN - 1);
    existing.first_name[MAX_NAME_LEN - 1] = '\0';
    memcpy(existing.last_name, cust->last_name, MAX_NAME_LEN - 1);
    existing.last_name[MAX_NAME_LEN - 1] = '\0';
    if (cust->password[0] != '\0') {
        memcpy(existing.password, cust->password, MAX_PASSWORD_LEN - 1);
        existing.password[MAX_PASSWORD_LEN - 1] = '\0';
    }
    
    int result = write_record(fd, cust_id - 1, &existing, sizeof(Customer));
    unlock_record(fd, cust_id - 1);
    close(fd);
    
    return (result == RECORD_SIZE) ? 0 : -1;
}

int employee_delete_customer(int cust_id) {
    int fd = open_data_file("data/customers.dat", O_RDWR);
    if (fd < 0) return -1;
    
    if (lock_record(fd, cust_id - 1, BANK_LOCK_WRITE) < 0) {
        close(fd);
        return -1;
    }
    
    Customer cust;
    if (read_record(fd, cust_id - 1, &cust, sizeof(Customer)) != sizeof(Customer)) {
        unlock_record(fd, cust_id - 1);
        close(fd);
        return -1;
    }
    
    cust.account_status = ACC_STATUS_INACTIVE;
    int result = write_record(fd, cust_id - 1, &cust, sizeof(Customer));
    unlock_record(fd, cust_id - 1);
    close(fd);
    
    return (result == RECORD_SIZE) ? 0 : -1;
}

int employee_process_loan(int emp_id, int loan_id, int status) {
    if (status != LOAN_APPROVED && status != LOAN_REJECTED) return -1;
    
    int fd = open_data_file("data/loans.dat", O_RDWR);
    if (fd < 0) return -1;
    
    if (lock_record(fd, loan_id - 1, BANK_LOCK_WRITE) < 0) {
        close(fd);
        return -1;
    }
    
    Loan loan;
    if (read_record(fd, loan_id - 1, &loan, sizeof(Loan)) != sizeof(Loan)) {
        unlock_record(fd, loan_id - 1);
        close(fd);
        return -1;
    }
    
    if (loan.status != LOAN_PENDING || loan.assigned_employee_id != emp_id) {
        unlock_record(fd, loan_id - 1);
        close(fd);
        return -1;
    }
    
    loan.status = status;
    int result = write_record(fd, loan_id - 1, &loan, sizeof(Loan));
    unlock_record(fd, loan_id - 1);
    close(fd);
    
    if (result != RECORD_SIZE) return -1;
    
    int cust_fd = open_data_file("data/customers.dat", O_RDWR);
    int customer_updated = 0;
    if (cust_fd >= 0) {
        if (lock_record(cust_fd, loan.customer_id - 1, BANK_LOCK_WRITE) == 0) {
            Customer cust;
            if (read_record(cust_fd, loan.customer_id - 1, &cust, sizeof(Customer)) == sizeof(Customer)) {
                if (status == LOAN_APPROVED) {
                    cust.balance += loan.amount;
                }
                cust.loan_status = LOAN_REJECTED;
                cust.loan_amount = 0;
                if (write_record(cust_fd, loan.customer_id - 1, &cust, sizeof(Customer)) == RECORD_SIZE) {
                    customer_updated = 1;
                }
            }
            unlock_record(cust_fd, loan.customer_id - 1);
        }
        close(cust_fd);
    }
    
    if (!customer_updated) {
        int undo_fd = open_data_file("data/loans.dat", O_RDWR);
        if (undo_fd >= 0) {
            if (lock_record(undo_fd, loan_id - 1, BANK_LOCK_WRITE) == 0) {
                Loan current;
                if (read_record(undo_fd, loan_id - 1, &current, sizeof(Loan)) == sizeof(Loan)) {
                    current.status = LOAN_PENDING;
                    write_record(undo_fd, loan_id - 1, &current, sizeof(Loan));
                }
                unlock_record(undo_fd, loan_id - 1);
            }
            close(undo_fd);
        }
        return -1;
    }
    
    return 0;
}

int employee_view_loans(int emp_id, Loan *loans, int max_loans) {
    int fd = open_data_file("data/loans.dat", O_RDONLY);
    if (fd < 0) return 0;
    
    struct stat st;
    fstat(fd, &st);
    int total = st.st_size / RECORD_SIZE;
    int count = 0;
    
    for (int i = 0; i < total && count < max_loans; i++) {
        Loan loan;
        if (read_record(fd, i, &loan, sizeof(Loan)) == sizeof(Loan)) {
            if (loan.assigned_employee_id == emp_id) {
                loans[count++] = loan;
            }
        }
    }
    
    close(fd);
    return count;
}

int employee_passbook(int cust_id, Transaction *txns, int max_txns) {
    return customer_txn_history(cust_id, txns, max_txns);
}

int employee_change_password(int emp_id, const char *old_pass, const char *new_pass) {
    int fd = open_data_file("data/employees.dat", O_RDWR);
    if (fd < 0) return -1;
    
    if (lock_record(fd, emp_id - 1, BANK_LOCK_WRITE) < 0) {
        close(fd);
        return -1;
    }
    
    Employee emp;
    if (read_record(fd, emp_id - 1, &emp, sizeof(Employee)) != sizeof(Employee)) {
        unlock_record(fd, emp_id - 1);
        close(fd);
        return -1;
    }
    
    if (strcmp(emp.password, old_pass) != 0) {
        unlock_record(fd, emp_id - 1);
        close(fd);
        return -1;
    }
    
    strncpy(emp.password, new_pass, MAX_PASSWORD_LEN - 1);
    emp.password[MAX_PASSWORD_LEN - 1] = '\0';
    
    int result = write_record(fd, emp_id - 1, &emp, sizeof(Employee));
    unlock_record(fd, emp_id - 1);
    close(fd);
    
    return (result == RECORD_SIZE) ? 0 : -1;
}

/* ==================== Manager Operations ==================== */

int manager_login(int mgr_id, const char *password) {
    int fd = open_data_file("data/managers.dat", O_RDWR);
    if (fd < 0) return 0;
    
    Manager mgr;
    if (read_record(fd, mgr_id - 1, &mgr, sizeof(Manager)) != sizeof(Manager)) {
        close(fd);
        return 0;
    }
    
    int success = (strcmp(mgr.password, password) == 0);
    close(fd);
    
    if (success) {
        if (session_create(mgr_id, ROLE_MANAGER) != 0) {
            return 0; /* Already logged in elsewhere */
        }
    }
    return success;
}

int manager_toggle_account(int cust_id, int status) {
    int fd = open_data_file("data/customers.dat", O_RDWR);
    if (fd < 0) return -1;
    
    if (lock_record(fd, cust_id - 1, BANK_LOCK_WRITE) < 0) {
        close(fd);
        return -1;
    }
    
    Customer cust;
    if (read_record(fd, cust_id - 1, &cust, sizeof(Customer)) != sizeof(Customer)) {
        unlock_record(fd, cust_id - 1);
        close(fd);
        return -1;
    }
    
    cust.account_status = status;
    int result = write_record(fd, cust_id - 1, &cust, sizeof(Customer));
    unlock_record(fd, cust_id - 1);
    close(fd);
    
    return (result == RECORD_SIZE) ? 0 : -1;
}

int manager_assign_loan(int loan_id, int emp_id) {
    /* Validate loan_id */
    int fd = open_data_file("data/loans.dat", O_RDWR);
    if (fd < 0) return -1;
    
    struct stat st;
    fstat(fd, &st);
    int loan_count = st.st_size / RECORD_SIZE;
    if (loan_id <= 0 || loan_id > loan_count) {
        close(fd);
        return -1; /* Invalid loan ID */
    }
    
    if (lock_record(fd, loan_id - 1, BANK_LOCK_WRITE) < 0) {
        close(fd);
        return -1;
    }
    
    Loan loan;
    if (read_record(fd, loan_id - 1, &loan, sizeof(Loan)) != sizeof(Loan)) {
        unlock_record(fd, loan_id - 1);
        close(fd);
        return -1;
    }
    
    if (loan.status != LOAN_PENDING || loan.assigned_employee_id > 0) {
        unlock_record(fd, loan_id - 1);
        close(fd);
        return -1; /* Loan not pending or already assigned */
    }
    
    /* Validate emp_id */
    int emp_fd = open_data_file("data/employees.dat", O_RDWR);
    if (emp_fd < 0) {
        unlock_record(fd, loan_id - 1);
        close(fd);
        return -1;
    }
    
    fstat(emp_fd, &st);
    int emp_count = st.st_size / RECORD_SIZE;
    if (emp_id <= 0 || emp_id > emp_count) {
        unlock_record(fd, loan_id - 1);
        close(fd);
        close(emp_fd);
        return -1; /* Invalid employee ID */
    }
    
    if (lock_record(emp_fd, emp_id - 1, BANK_LOCK_WRITE) < 0) {
        unlock_record(fd, loan_id - 1);
        close(fd);
        close(emp_fd);
        return -1;
    }
    
    Employee emp;
    if (read_record(emp_fd, emp_id - 1, &emp, sizeof(Employee)) != sizeof(Employee)) {
        unlock_record(fd, loan_id - 1);
        unlock_record(emp_fd, emp_id - 1);
        close(fd);
        close(emp_fd);
        return -1;
    }
    
    if (emp.status != 0) {
        unlock_record(fd, loan_id - 1);
        unlock_record(emp_fd, emp_id - 1);
        close(fd);
        close(emp_fd);
        return -1; /* Employee not active */
    }
    
    /* Assign loan */
    loan.assigned_employee_id = emp_id;
    int result = write_record(fd, loan_id - 1, &loan, sizeof(Loan));
    unlock_record(fd, loan_id - 1);
    close(fd);
    
    /* Update employee loan count */
    if (emp.loan_count < 50) {
        emp.assigned_loans[emp.loan_count++] = loan_id;
        write_record(emp_fd, emp_id - 1, &emp, sizeof(Employee));
    }
    unlock_record(emp_fd, emp_id - 1);
    close(emp_fd);
    
    return (result == RECORD_SIZE) ? 0 : -1;
}

int manager_view_feedback(Feedback *fbs, int max_fbs) {
    int fd = open_data_file("data/feedback.dat", O_RDONLY);
    if (fd < 0) return 0;
    
    struct stat st;
    fstat(fd, &st);
    int total = st.st_size / RECORD_SIZE;
    int count = 0;
    
    for (int i = 0; i < total && count < max_fbs; i++) {
        Feedback fb;
        if (read_record(fd, i, &fb, sizeof(Feedback)) == sizeof(Feedback)) {
            fbs[count++] = fb;
        }
    }
    
    close(fd);
    return count;
}

int manager_review_feedback(int fb_id) {
    int fd = open_data_file("data/feedback.dat", O_RDWR);
    if (fd < 0) return -1;
    
    if (lock_record(fd, fb_id - 1, BANK_LOCK_WRITE) < 0) {
        close(fd);
        return -1;
    }
    
    Feedback fb;
    if (read_record(fd, fb_id - 1, &fb, sizeof(Feedback)) != sizeof(Feedback)) {
        unlock_record(fd, fb_id - 1);
        close(fd);
        return -1;
    }
    
    fb.status = 1;
    int result = write_record(fd, fb_id - 1, &fb, sizeof(Feedback));
    unlock_record(fd, fb_id - 1);
    close(fd);
    
    return (result == RECORD_SIZE) ? 0 : -1;
}

int manager_change_password(int mgr_id, const char *old_pass, const char *new_pass) {
    int fd = open_data_file("data/managers.dat", O_RDWR);
    if (fd < 0) return -1;
    
    if (lock_record(fd, mgr_id - 1, BANK_LOCK_WRITE) < 0) {
        close(fd);
        return -1;
    }
    
    Manager mgr;
    if (read_record(fd, mgr_id - 1, &mgr, sizeof(Manager)) != sizeof(Manager)) {
        unlock_record(fd, mgr_id - 1);
        close(fd);
        return -1;
    }
    
    if (strcmp(mgr.password, old_pass) != 0) {
        unlock_record(fd, mgr_id - 1);
        close(fd);
        return -1;
    }
    
    strncpy(mgr.password, new_pass, MAX_PASSWORD_LEN - 1);
    mgr.password[MAX_PASSWORD_LEN - 1] = '\0';
    
    int result = write_record(fd, mgr_id - 1, &mgr, sizeof(Manager));
    unlock_record(fd, mgr_id - 1);
    close(fd);
    
    return (result == RECORD_SIZE) ? 0 : -1;
}

/* ==================== Admin Operations ==================== */

int admin_login(int adm_id, const char *password) {
    int fd = open_data_file("data/admins.dat", O_RDWR);
    if (fd < 0) return 0;
    
    Admin adm;
    if (read_record(fd, adm_id - 1, &adm, sizeof(Admin)) != sizeof(Admin)) {
        close(fd);
        return 0;
    }
    
    int success = (strcmp(adm.password, password) == 0);
    close(fd);
    
    if (success) {
        if (session_create(adm_id, ROLE_ADMIN) != 0) {
            return 0; /* Already logged in elsewhere */
        }
    }
    return success;
}

int admin_add_employee(const Employee *emp) {
    Employee new_emp;
    memset(&new_emp, 0, sizeof(new_emp));
    
    memcpy(new_emp.first_name, emp->first_name, MAX_NAME_LEN - 1);
    memcpy(new_emp.last_name, emp->last_name, MAX_NAME_LEN - 1);
    memcpy(new_emp.password, emp->password, MAX_PASSWORD_LEN - 1);
    new_emp.status = 0;
    new_emp.loan_count = 0;
    
    return append_record("data/employees.dat", &new_emp, sizeof(Employee));
}

int admin_modify_employee(int emp_id, const Employee *emp) {
    int fd = open_data_file("data/employees.dat", O_RDWR);
    if (fd < 0) return -1;
    
    if (lock_record(fd, emp_id - 1, BANK_LOCK_WRITE) < 0) {
        close(fd);
        return -1;
    }
    
    Employee existing;
    if (read_record(fd, emp_id - 1, &existing, sizeof(Employee)) != sizeof(Employee)) {
        unlock_record(fd, emp_id - 1);
        close(fd);
        return -1;
    }
    
    memcpy(existing.first_name, emp->first_name, MAX_NAME_LEN - 1);
    existing.first_name[MAX_NAME_LEN - 1] = '\0';
    memcpy(existing.last_name, emp->last_name, MAX_NAME_LEN - 1);
    existing.last_name[MAX_NAME_LEN - 1] = '\0';
    if (emp->password[0] != '\0') {
        memcpy(existing.password, emp->password, MAX_PASSWORD_LEN - 1);
        existing.password[MAX_PASSWORD_LEN - 1] = '\0';
    }
    existing.status = 0;
    
    int result = write_record(fd, emp_id - 1, &existing, sizeof(Employee));
    unlock_record(fd, emp_id - 1);
    close(fd);
    
    return (result == RECORD_SIZE) ? 0 : -1;
}

int admin_delete_employee(int emp_id) {
    int fd = open_data_file("data/employees.dat", O_RDWR);
    if (fd < 0) return -1;
    
    if (lock_record(fd, emp_id - 1, BANK_LOCK_WRITE) < 0) {
        close(fd);
        return -1;
    }
    
    Employee emp;
    if (read_record(fd, emp_id - 1, &emp, sizeof(Employee)) != sizeof(Employee)) {
        unlock_record(fd, emp_id - 1);
        close(fd);
        return -1;
    }
    
    emp.status = 1;
    int result = write_record(fd, emp_id - 1, &emp, sizeof(Employee));
    unlock_record(fd, emp_id - 1);
    close(fd);
    
    return (result == RECORD_SIZE) ? 0 : -1;
}

int admin_manage_role(int emp_id, int new_role) {
    if (emp_id <= 0) return -1;
    if (new_role != ROLE_EMPLOYEE && new_role != ROLE_MANAGER &&
        new_role != ROLE_ADMIN) {
        return -1;
    }
    
    int fd = open_data_file("data/employees.dat", O_RDWR);
    if (fd < 0) return -1;
    
    if (lock_record(fd, emp_id - 1, BANK_LOCK_WRITE) < 0) {
        close(fd);
        return -1;
    }
    
    Employee emp;
    if (read_record(fd, emp_id - 1, &emp, sizeof(Employee)) != sizeof(Employee)) {
        unlock_record(fd, emp_id - 1);
        close(fd);
        return -1;
    }
    
    if (emp.status != 0) {
        unlock_record(fd, emp_id - 1);
        close(fd);
        return -1;
    }
    
    if (new_role == ROLE_EMPLOYEE) {
        unlock_record(fd, emp_id - 1);
        close(fd);
        return emp_id;
    }
    
    if (emp.loan_count > 0) {
        unlock_record(fd, emp_id - 1);
        close(fd);
        return -1;
    }
    
    int old_status = emp.status;
    emp.status = 1;
    if (write_record(fd, emp_id - 1, &emp, sizeof(Employee)) != RECORD_SIZE) {
        unlock_record(fd, emp_id - 1);
        close(fd);
        return -1;
    }
    
    int new_id = -1;
    if (new_role == ROLE_MANAGER) {
        Manager mgr;
        memset(&mgr, 0, sizeof(mgr));
        memcpy(mgr.first_name, emp.first_name, MAX_NAME_LEN - 1);
        memcpy(mgr.last_name, emp.last_name, MAX_NAME_LEN - 1);
        memcpy(mgr.password, emp.password, MAX_PASSWORD_LEN - 1);
        new_id = append_record("data/managers.dat", &mgr, sizeof(Manager));
    } else {
        Admin adm;
        memset(&adm, 0, sizeof(adm));
        memcpy(adm.first_name, emp.first_name, MAX_NAME_LEN - 1);
        memcpy(adm.last_name, emp.last_name, MAX_NAME_LEN - 1);
        memcpy(adm.password, emp.password, MAX_PASSWORD_LEN - 1);
        new_id = append_record("data/admins.dat", &adm, sizeof(Admin));
    }
    
    if (new_id < 0) {
        emp.status = old_status;
        write_record(fd, emp_id - 1, &emp, sizeof(Employee));
    }
    
    unlock_record(fd, emp_id - 1);
    close(fd);
    return new_id;
}

int admin_change_password(int adm_id, const char *old_pass, const char *new_pass) {
    int fd = open_data_file("data/admins.dat", O_RDWR);
    if (fd < 0) return -1;
    
    if (lock_record(fd, adm_id - 1, BANK_LOCK_WRITE) < 0) {
        close(fd);
        return -1;
    }
    
    Admin adm;
    if (read_record(fd, adm_id - 1, &adm, sizeof(Admin)) != sizeof(Admin)) {
        unlock_record(fd, adm_id - 1);
        close(fd);
        return -1;
    }
    
    if (strcmp(adm.password, old_pass) != 0) {
        unlock_record(fd, adm_id - 1);
        close(fd);
        return -1;
    }
    
    strncpy(adm.password, new_pass, MAX_PASSWORD_LEN - 1);
    adm.password[MAX_PASSWORD_LEN - 1] = '\0';
    
    int result = write_record(fd, adm_id - 1, &adm, sizeof(Admin));
    unlock_record(fd, adm_id - 1);
    close(fd);
    
    return (result == RECORD_SIZE) ? 0 : -1;
}

/* ==================== Utility ==================== */

int command_allowed_for_role(int role, int command) {
    if (command == CMD_LOGOUT || command == CMD_CHANGE_PASS) return 1;
    
    switch (role) {
        case ROLE_CUSTOMER:
            switch (command) {
                case CMD_BALANCE:
                case CMD_DEPOSIT:
                case CMD_WITHDRAW:
                case CMD_TRANSFER:
                case CMD_LOAN_APPLY:
                case CMD_TXN_HISTORY:
                case CMD_FEEDBACK:
                    return 1;
                default:
                    return 0;
            }
        case ROLE_EMPLOYEE:
            switch (command) {
                case CMD_EMP_ADD_CUST:
                case CMD_EMP_MOD_CUST:
                case CMD_EMP_DEL_CUST:
                case CMD_EMP_PROC_LOAN:
                case CMD_EMP_VIEW_LOANS:
                case CMD_EMP_PASSBOOK:
                case CMD_EMP_CHANGE_PASS:
                    return 1;
                default:
                    return 0;
            }
        case ROLE_MANAGER:
            switch (command) {
                case CMD_MGR_TOGGLE_ACC:
                case CMD_MGR_ASSIGN_LOAN:
                case CMD_MGR_FEEDBACK:
                case CMD_MGR_REVIEW_FEEDBACK:
                case CMD_MGR_CHANGE_PASS:
                    return 1;
                default:
                    return 0;
            }
        case ROLE_ADMIN:
            switch (command) {
                case CMD_ADM_ADD_EMP:
                case CMD_ADM_MOD_EMP:
                case CMD_ADM_DEL_EMP:
                case CMD_ADM_ROLE_MGMT:
                case CMD_ADM_CHANGE_PASS:
                    return 1;
                default:
                    return 0;
            }
        default:
            return 0;
    }
}

void log_transaction(int customer_id, int type, int64_t amount, int target_id) {
    Transaction txn = {
        .customer_id = customer_id,
        .type = type,
        .amount = amount,
        .target_customer_id = target_id
    };
    
    time_t now = time(NULL);
    strftime(txn.timestamp, sizeof(txn.timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
    
    append_record("data/transactions.dat", &txn, sizeof(Transaction));
}

void init_data_files(void) {
    mkdir("data", 0755);
    
    /* Create default admin if not exists */
    int fd = open_data_file("data/admins.dat", O_RDWR);
    struct stat st;
    if (fstat(fd, &st) == 0 && st.st_size == 0) {
        Admin adm = {.admin_id = 1, .first_name = "Admin", .last_name = "User", .password = "admin123"};
        write_record(fd, 0, &adm, sizeof(Admin));
    }
    close(fd);
    
    /* Create default manager if not exists */
    fd = open_data_file("data/managers.dat", O_RDWR);
    if (fstat(fd, &st) == 0 && st.st_size == 0) {
        Manager mgr = {.manager_id = 1, .first_name = "Manager", .last_name = "User", .password = "mgr123"};
        write_record(fd, 0, &mgr, sizeof(Manager));
    }
    close(fd);
    
    /* Clear sessions file on startup */
    fd = open_data_file("data/sessions.dat", O_RDWR | O_TRUNC);
    close(fd);
}