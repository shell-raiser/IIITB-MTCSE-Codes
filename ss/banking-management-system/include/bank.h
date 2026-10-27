#ifndef BANK_H
#define BANK_H

#include <stdint.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

/* ==================== Constants ==================== */
#define MICRO_UNITS         1000000LL
#define MAX_NAME_LEN        50
#define MAX_PASSWORD_LEN    50
#define MAX_MESSAGE_LEN     256
#define MAX_CLIENTS         200
#define RECORD_SIZE         512
#define PORT                8080

#define ROLE_CUSTOMER       0
#define ROLE_EMPLOYEE       1
#define ROLE_MANAGER        2
#define ROLE_ADMIN          3

#define ACC_STATUS_ACTIVE   0
#define ACC_STATUS_INACTIVE 1

#define LOAN_PENDING        0
#define LOAN_APPROVED       1
#define LOAN_REJECTED       2

#define TXN_DEPOSIT         0
#define TXN_WITHDRAW        1
#define TXN_TRANSFER        2

#define BANK_LOCK_READ      F_RDLCK
#define BANK_LOCK_WRITE     F_WRLCK
#define BANK_LOCK_UNLOCK    F_UNLCK

/* Session tracking */
#define MAX_SESSIONS        200
#define SESSION_TIMEOUT     300  // 5 minutes in seconds

typedef struct {
    int user_id;
    int role;
    int active;
    time_t last_activity;
} Session;

/* ==================== Data Structures ==================== */

typedef struct {
    int user_id;
    char first_name[MAX_NAME_LEN];
    char last_name[MAX_NAME_LEN];
    char password[MAX_PASSWORD_LEN];
    int64_t balance;        // in micro-units (1/1000000)
    int loan_status;
    int64_t loan_amount;    // in micro-units
    int account_status;
} __attribute__((packed)) Customer;

typedef struct {
    int employee_id;
    char first_name[MAX_NAME_LEN];
    char last_name[MAX_NAME_LEN];
    char password[MAX_PASSWORD_LEN];
    int status;
    int loan_count;
    int assigned_loans[50];
} __attribute__((packed)) Employee;

typedef struct {
    int admin_id;
    char first_name[MAX_NAME_LEN];
    char last_name[MAX_NAME_LEN];
    char password[MAX_PASSWORD_LEN];
} __attribute__((packed)) Admin;

typedef struct {
    int manager_id;
    char first_name[MAX_NAME_LEN];
    char last_name[MAX_NAME_LEN];
    char password[MAX_PASSWORD_LEN];
} __attribute__((packed)) Manager;

typedef struct {
    int transaction_id;
    int customer_id;
    int type;
    int64_t amount;        // in micro-units
    int target_customer_id;
    char timestamp[32];
} __attribute__((packed)) Transaction;

typedef struct {
    int loan_id;
    int customer_id;
    int64_t amount;        // in micro-units
    int status;
    int assigned_employee_id;
} __attribute__((packed)) Loan;

typedef struct {
    int feedback_id;
    int customer_id;
    char message[MAX_MESSAGE_LEN];
    int status;
} __attribute__((packed)) Feedback;

/* ==================== Protocol ==================== */

typedef struct {
    int role;
    int user_id;
    int command;
    char payload[512];
} Request;

typedef struct {
    int success;
    char message[512];
    char data[1024];
} Response;

/* Command codes */
#define CMD_LOGIN             1
#define CMD_LOGOUT            2
#define CMD_BALANCE           3
#define CMD_DEPOSIT           4
#define CMD_WITHDRAW          5
#define CMD_TRANSFER          6
#define CMD_CHANGE_PASS       7
#define CMD_LOAN_APPLY        8
#define CMD_TXN_HISTORY       9
#define CMD_FEEDBACK          10

#define CMD_EMP_ADD_CUST      20
#define CMD_EMP_MOD_CUST      21
#define CMD_EMP_DEL_CUST      22
#define CMD_EMP_PROC_LOAN     23
#define CMD_EMP_VIEW_LOANS    24
#define CMD_EMP_PASSBOOK      25
#define CMD_EMP_CHANGE_PASS   26

#define CMD_MGR_TOGGLE_ACC    40
#define CMD_MGR_ASSIGN_LOAN   41
#define CMD_MGR_FEEDBACK      42
#define CMD_MGR_CHANGE_PASS   43
#define CMD_MGR_REVIEW_FEEDBACK 44

#define CMD_ADM_ADD_EMP       60
#define CMD_ADM_MOD_EMP       61
#define CMD_ADM_DEL_EMP       62
#define CMD_ADM_ROLE_MGMT     63
#define CMD_ADM_CHANGE_PASS   64

/* ==================== Function Declarations ==================== */

/* File operations */
int open_data_file(const char *filename, int flags);
int lock_record(int fd, int record_num, short lock_type);
int try_lock_record(int fd, int record_num, short lock_type);
int unlock_record(int fd, int record_num);
ssize_t read_record(int fd, int record_num, void *buf, size_t size);
ssize_t write_record(int fd, int record_num, const void *buf, size_t size);
int get_next_id(const char *filename, int record_size);

/* Session management */
int session_create(int user_id, int role);
int session_destroy(int user_id, int role);
int session_is_active(int user_id, int role);
int session_update_activity(int user_id, int role);
int session_reaper(void);

/* Customer operations */
int customer_login(int user_id, const char *password);
int customer_balance(int user_id, int64_t *balance);
int customer_deposit(int user_id, int64_t amount);
int customer_withdraw(int user_id, int64_t amount);
int customer_transfer(int from_id, int to_id, int64_t amount);
int customer_change_password(int user_id, const char *old_pass, const char *new_pass);
int customer_loan_apply(int user_id, int64_t amount);
int customer_txn_history(int user_id, Transaction *txns, int max_txns);
int customer_feedback(int user_id, const char *message);

/* Employee operations */
int employee_login(int emp_id, const char *password);
int employee_add_customer(const Customer *cust);
int employee_modify_customer(int cust_id, const Customer *cust);
int employee_delete_customer(int cust_id);
int employee_process_loan(int emp_id, int loan_id, int status);
int employee_view_loans(int emp_id, Loan *loans, int max_loans);
int employee_passbook(int cust_id, Transaction *txns, int max_txns);
int employee_change_password(int emp_id, const char *old_pass, const char *new_pass);

/* Manager operations */
int manager_login(int mgr_id, const char *password);
int manager_toggle_account(int cust_id, int status);
int manager_assign_loan(int loan_id, int emp_id);
int manager_view_feedback(Feedback *fbs, int max_fbs);
int manager_review_feedback(int fb_id);
int manager_change_password(int mgr_id, const char *old_pass, const char *new_pass);

/* Admin operations */
int admin_login(int adm_id, const char *password);
int admin_add_employee(const Employee *emp);
int admin_modify_employee(int emp_id, const Employee *emp);
int admin_delete_employee(int emp_id);
int admin_manage_role(int emp_id, int new_role);
int admin_change_password(int adm_id, const char *old_pass, const char *new_pass);

/* Utility */
int command_allowed_for_role(int role, int command);
void log_transaction(int customer_id, int type, int64_t amount, int target_id);
void daemonize_server(void);
void handle_client(int client_fd);
void init_data_files(void);

#endif /* BANK_H */