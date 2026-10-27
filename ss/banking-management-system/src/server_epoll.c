#include "bank.h"
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <stdarg.h>

#define session_create mem_session_create
#define session_find mem_session_find
#define session_update_activity mem_session_update_activity
#define session_destroy mem_session_destroy

#define MAX_EVENTS       512
#define MAX_CONNECTIONS  1000000
#define READ_BUFFER_SIZE 524
#define WRITE_BUFFER_SIZE 1540
#define EPOLL_TIMEOUT_MS 1000
#define CONNECTION_TIMEOUT 300

typedef enum {
    CONN_FREE = 0,
    CONN_READING_REQUEST,
    CONN_PROCESSING,
    CONN_WRITING_RESPONSE
} ConnState;

typedef struct {
    int fd;
    char read_buffer[READ_BUFFER_SIZE];
    char write_buffer[WRITE_BUFFER_SIZE];
    size_t read_pos;
    size_t write_pos;
    ConnState state;
    time_t last_activity;
    int session_idx;
} Connection;

static int listen_fd = -1;
static int epoll_fd = -1;
static Connection connections[MAX_CONNECTIONS];
static size_t next_slot = 0;
static Session sessions[MAX_SESSIONS];
static Connection *session_owner[MAX_SESSIONS];

static int session_find(int user_id, int role) {
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (sessions[i].active && sessions[i].user_id == user_id &&
            sessions[i].role == role) {
            return i;
        }
    }
    return -1;
}

static int session_create(int user_id, int role) {
    if (session_find(user_id, role) >= 0) return -1;

    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (!sessions[i].active) {
            sessions[i].user_id = user_id;
            sessions[i].role = role;
            sessions[i].active = 1;
            sessions[i].last_activity = time(NULL);
            session_owner[i] = NULL;
            return i;
        }
    }
    return -1;
}

static void session_update_activity(int idx) {
    if (idx >= 0 && idx < MAX_SESSIONS && sessions[idx].active) {
        sessions[idx].last_activity = time(NULL);
    }
}

static void session_destroy(int idx) {
    if (idx < 0 || idx >= MAX_SESSIONS) return;

    sessions[idx].active = 0;
    sessions[idx].user_id = 0;
    sessions[idx].role = 0;
    sessions[idx].last_activity = 0;

    if (session_owner[idx]) {
        if (session_owner[idx]->session_idx == idx) {
            session_owner[idx]->session_idx = -1;
        }
        session_owner[idx] = NULL;
    }
}

static void cleanup_expired_sessions(void) {
    time_t now = time(NULL);

    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (sessions[i].active &&
            now - sessions[i].last_activity > SESSION_TIMEOUT) {
            session_destroy(i);
        }
    }
}

static int authenticate_user(int role, int user_id, const char *password) {
    if (user_id <= 0 || !password) return 0;

    int fd = -1;
    int ok = 0;

    switch (role) {
        case ROLE_CUSTOMER: {
            Customer customer;
            fd = open_data_file("data/customers.dat", O_RDONLY);
            if (fd >= 0 &&
                read_record(fd, user_id - 1, &customer,
                            sizeof(customer)) == sizeof(customer)) {
                ok = strcmp(customer.password, password) == 0 &&
                     customer.account_status == ACC_STATUS_ACTIVE;
            }
            break;
        }
        case ROLE_EMPLOYEE: {
            Employee employee;
            fd = open_data_file("data/employees.dat", O_RDONLY);
            if (fd >= 0 &&
                read_record(fd, user_id - 1, &employee,
                            sizeof(employee)) == sizeof(employee)) {
                ok = strcmp(employee.password, password) == 0 &&
                     employee.status == 0;
            }
            break;
        }
        case ROLE_MANAGER: {
            Manager manager;
            fd = open_data_file("data/managers.dat", O_RDONLY);
            if (fd >= 0 &&
                read_record(fd, user_id - 1, &manager,
                            sizeof(manager)) == sizeof(manager)) {
                ok = strcmp(manager.password, password) == 0;
            }
            break;
        }
        case ROLE_ADMIN: {
            Admin admin;
            fd = open_data_file("data/admins.dat", O_RDONLY);
            if (fd >= 0 &&
                read_record(fd, user_id - 1, &admin,
                            sizeof(admin)) == sizeof(admin)) {
                ok = strcmp(admin.password, password) == 0;
            }
            break;
        }
        default:
            return 0;
    }

    if (fd >= 0) close(fd);
    return ok;
}

static int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

static void close_connection(Connection *conn) {
    if (conn->fd < 0) return;

    if (conn->session_idx >= 0) {
        session_destroy(conn->session_idx);
    }

    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, conn->fd, NULL);
    close(conn->fd);

    conn->fd = -1;
    conn->read_pos = 0;
    conn->write_pos = 0;
    conn->state = CONN_FREE;
    conn->last_activity = 0;
    conn->session_idx = -1;
}

static void cleanup_server(int sig) {
    (void)sig;

    if (listen_fd >= 0) close(listen_fd);
    if (epoll_fd >= 0) close(epoll_fd);

    for (size_t i = 0; i < MAX_CONNECTIONS; i++) {
        if (connections[i].fd >= 0) close(connections[i].fd);
    }

    exit(0);
}

static int update_connection_events(Connection *conn, uint32_t events) {
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = events;
    ev.data.ptr = conn;
    return epoll_ctl(epoll_fd, EPOLL_CTL_MOD, conn->fd, &ev);
}

static void build_error_response(Response *resp, const char *message) {
    resp->success = 0;
    snprintf(resp->message, sizeof(resp->message), "%s", message);
    resp->data[0] = '\0';
}

static void build_success_response(Response *resp, const char *message,
                                   const char *data) {
    resp->success = 1;
    snprintf(resp->message, sizeof(resp->message), "%s", message);
    if (data) {
        snprintf(resp->data, sizeof(resp->data), "%s", data);
    } else {
        resp->data[0] = '\0';
    }
}

static void append_response_data(Response *resp, const char *format, ...) {
    size_t used = strlen(resp->data);
    if (used >= sizeof(resp->data) - 1) return;

    va_list args;
    va_start(args, format);
    vsnprintf(resp->data + used, sizeof(resp->data) - used, format, args);
    va_end(args);
}

static int validate_session(Connection *conn, int user_id, int role) {
    if (user_id <= 0 || role < 0 || role > ROLE_ADMIN) return -1;

    int idx = session_find(user_id, role);
    if (idx < 0 || idx != conn->session_idx) return -1;

    if (time(NULL) - sessions[idx].last_activity > SESSION_TIMEOUT) {
        session_destroy(idx);
        return -1;
    }
    return idx;
}

static void finish_response(Connection *conn, const Response *resp) {
    memcpy(conn->write_buffer, resp, sizeof(*resp));
    conn->write_pos = 0;
    conn->state = CONN_WRITING_RESPONSE;
}

static void dispatch_request(Connection *conn, Request *req) {
    Response resp;
    memset(&resp, 0, sizeof(resp));

    if (req->command != CMD_LOGIN) {
        if (validate_session(conn, req->user_id, req->role) < 0) {
            build_error_response(&resp, "Session invalid or expired");
            finish_response(conn, &resp);
            return;
        }
        if (!command_allowed_for_role(req->role, req->command)) {
            build_error_response(&resp, "Permission denied for this role");
            finish_response(conn, &resp);
            return;
        }
    }

    switch (req->command) {
        case CMD_LOGIN: {
            char *role_str = strtok(req->payload, "|");
            char *id_str = strtok(NULL, "|");
            char *pass = strtok(NULL, "|");

            if (!role_str || !id_str || !pass) {
                build_error_response(&resp, "Invalid login format");
                break;
            }

            int role = atoi(role_str);
            int id = atoi(id_str);

            if (!authenticate_user(role, id, pass)) {
                build_error_response(&resp, "Invalid credentials");
                break;
            }

            int idx = session_find(id, role);
            if (idx >= 0 && idx != conn->session_idx) {
                build_error_response(&resp, "Already logged in elsewhere");
                break;
            }

            if (idx < 0) {
                if (conn->session_idx >= 0) {
                    session_destroy(conn->session_idx);
                }
                idx = session_create(id, role);
                if (idx < 0) {
                    build_error_response(&resp, "Session limit reached");
                    break;
                }
                conn->session_idx = idx;
                session_owner[idx] = conn;
            } else {
                session_update_activity(idx);
            }

            req->role = role;
            req->user_id = id;
            build_success_response(&resp, "Login successful", NULL);
            snprintf(resp.data, sizeof(resp.data), "%d|%d", role, id);
            break;
        }

        case CMD_LOGOUT: {
            if (conn->session_idx < 0) {
                build_error_response(&resp, "Logout failed");
            } else {
                session_destroy(conn->session_idx);
                build_success_response(&resp, "Logout successful", NULL);
            }
            break;
        }

        case CMD_BALANCE: {
            int64_t balance = 0;
            if (customer_balance(req->user_id, &balance) == 0) {
                build_success_response(&resp, "Balance retrieved", NULL);
                snprintf(resp.data, sizeof(resp.data), "%lld",
                         (long long)balance);
            } else {
                build_error_response(&resp, "Failed to get balance");
            }
            break;
        }

        case CMD_DEPOSIT: {
            int64_t amount = atoll(req->payload);
            if (amount <= 0) {
                build_error_response(&resp, "Invalid amount");
            } else if (customer_deposit(req->user_id, amount) == 0) {
                build_success_response(&resp, "Deposit successful", NULL);
            } else {
                build_error_response(&resp, "Deposit failed");
            }
            break;
        }

        case CMD_WITHDRAW: {
            int64_t amount = atoll(req->payload);
            if (amount <= 0) {
                build_error_response(&resp, "Invalid amount");
            } else if (customer_withdraw(req->user_id, amount) == 0) {
                build_success_response(&resp, "Withdrawal successful", NULL);
            } else {
                build_error_response(&resp, "Withdrawal failed");
            }
            break;
        }

        case CMD_TRANSFER: {
            char *to_str = strtok(req->payload, "|");
            char *amount_str = strtok(NULL, "|");

            if (!to_str || !amount_str) {
                build_error_response(&resp, "Invalid transfer format");
                break;
            }

            int to_id = atoi(to_str);
            int64_t amount = atoll(amount_str);

            if (amount <= 0 || to_id == req->user_id) {
                build_error_response(&resp, "Invalid transfer parameters");
            } else if (customer_transfer(req->user_id, to_id, amount) == 0) {
                build_success_response(&resp, "Transfer successful", NULL);
            } else {
                build_error_response(&resp, "Transfer failed");
            }
            break;
        }

        case CMD_CHANGE_PASS: {
            char *old_pass = strtok(req->payload, "|");
            char *new_pass = strtok(NULL, "|");

            if (!old_pass || !new_pass) {
                build_error_response(&resp, "Invalid password format");
                break;
            }

            int result = -1;
            switch (req->role) {
                case ROLE_CUSTOMER:
                    result = customer_change_password(req->user_id,
                                                      old_pass, new_pass);
                    break;
                case ROLE_EMPLOYEE:
                    result = employee_change_password(req->user_id,
                                                      old_pass, new_pass);
                    break;
                case ROLE_MANAGER:
                    result = manager_change_password(req->user_id,
                                                     old_pass, new_pass);
                    break;
                case ROLE_ADMIN:
                    result = admin_change_password(req->user_id,
                                                   old_pass, new_pass);
                    break;
            }

            if (result == 0) {
                build_success_response(&resp, "Password changed", NULL);
            } else {
                build_error_response(&resp, "Password change failed");
            }
            break;
        }

        case CMD_LOAN_APPLY: {
            int64_t amount = atoll(req->payload);
            if (amount <= 0) {
                build_error_response(&resp, "Invalid loan amount");
            } else if (customer_loan_apply(req->user_id, amount) == 0) {
                build_success_response(&resp, "Loan application submitted",
                                       NULL);
            } else {
                build_error_response(&resp, "Loan application failed");
            }
            break;
        }

        case CMD_TXN_HISTORY: {
            Transaction txns[50];
            int count = customer_txn_history(req->user_id, txns, 50);

            build_success_response(&resp, "Transaction history", NULL);
            for (int i = 0; i < count; i++) {
                append_response_data(&resp, "%d|%d|%lld|%d|%s\n",
                                     txns[i].transaction_id, txns[i].type,
                                     (long long)txns[i].amount,
                                     txns[i].target_customer_id,
                                     txns[i].timestamp);
            }
            break;
        }

        case CMD_FEEDBACK: {
            if (customer_feedback(req->user_id, req->payload) == 0) {
                build_success_response(&resp, "Feedback submitted", NULL);
            } else {
                build_error_response(&resp, "Feedback failed");
            }
            break;
        }

        case CMD_EMP_ADD_CUST: {
            Customer cust;
            memset(&cust, 0, sizeof(cust));

            int fields = sscanf(req->payload, "%49[^|]|%49[^|]|%49[^|]",
                                cust.first_name, cust.last_name,
                                cust.password);
            if (fields != 3) {
                build_error_response(&resp, "Invalid customer format");
                break;
            }

            int id = employee_add_customer(&cust);
            if (id > 0) {
                build_success_response(&resp, "Customer added", NULL);
                snprintf(resp.data, sizeof(resp.data), "%d", id);
            } else {
                build_error_response(&resp, "Add customer failed");
            }
            break;
        }

        case CMD_EMP_MOD_CUST: {
            int cust_id = 0;
            Customer cust;
            memset(&cust, 0, sizeof(cust));

            int fields = sscanf(req->payload, "%d|%49[^|]|%49[^|]|%49[^|]",
                                &cust_id, cust.first_name, cust.last_name,
                                cust.password);
            if (fields != 4) {
                build_error_response(&resp, "Invalid customer format");
            } else if (employee_modify_customer(cust_id, &cust) == 0) {
                build_success_response(&resp, "Customer modified", NULL);
            } else {
                build_error_response(&resp, "Modify customer failed");
            }
            break;
        }

        case CMD_EMP_DEL_CUST: {
            int cust_id = atoi(req->payload);
            if (employee_delete_customer(cust_id) == 0) {
                build_success_response(&resp, "Customer deactivated", NULL);
            } else {
                build_error_response(&resp, "Delete customer failed");
            }
            break;
        }

        case CMD_EMP_PROC_LOAN: {
            char *loan_str = strtok(req->payload, "|");
            char *status_str = strtok(NULL, "|");

            if (!loan_str || !status_str) {
                build_error_response(&resp, "Invalid loan format");
                break;
            }

            int loan_id = atoi(loan_str);
            int status = atoi(status_str);

            if (employee_process_loan(req->user_id, loan_id, status) == 0) {
                build_success_response(&resp, "Loan processed", NULL);
            } else {
                build_error_response(&resp, "Process loan failed");
            }
            break;
        }

        case CMD_EMP_VIEW_LOANS: {
            Loan loans[50];
            int count = employee_view_loans(req->user_id, loans, 50);

            build_success_response(&resp, "Loans retrieved", NULL);
            for (int i = 0; i < count; i++) {
                append_response_data(&resp, "%d|%d|%lld|%d|%d\n",
                                     loans[i].loan_id, loans[i].customer_id,
                                     (long long)loans[i].amount,
                                     loans[i].status,
                                     loans[i].assigned_employee_id);
            }
            break;
        }

        case CMD_EMP_PASSBOOK: {
            int cust_id = atoi(req->payload);
            Transaction txns[50];
            int count = employee_passbook(cust_id, txns, 50);

            build_success_response(&resp, "Passbook retrieved", NULL);
            for (int i = 0; i < count; i++) {
                append_response_data(&resp, "%d|%d|%lld|%d|%s\n",
                                     txns[i].transaction_id, txns[i].type,
                                     (long long)txns[i].amount,
                                     txns[i].target_customer_id,
                                     txns[i].timestamp);
            }
            break;
        }

        case CMD_EMP_CHANGE_PASS: {
            char *old_pass = strtok(req->payload, "|");
            char *new_pass = strtok(NULL, "|");

            if (!old_pass || !new_pass) {
                build_error_response(&resp, "Invalid password format");
            } else if (employee_change_password(req->user_id, old_pass,
                                                new_pass) == 0) {
                build_success_response(&resp, "Password changed", NULL);
            } else {
                build_error_response(&resp, "Password change failed");
            }
            break;
        }

        case CMD_MGR_TOGGLE_ACC: {
            char *cust_str = strtok(req->payload, "|");
            char *status_str = strtok(NULL, "|");

            if (!cust_str || !status_str) {
                build_error_response(&resp, "Invalid account format");
                break;
            }

            int cust_id = atoi(cust_str);
            int status = atoi(status_str);

            if (manager_toggle_account(cust_id, status) == 0) {
                build_success_response(&resp, "Account status updated", NULL);
            } else {
                build_error_response(&resp, "Account status update failed");
            }
            break;
        }

        case CMD_MGR_ASSIGN_LOAN: {
            char *loan_str = strtok(req->payload, "|");
            char *emp_str = strtok(NULL, "|");

            if (!loan_str || !emp_str) {
                build_error_response(&resp, "Invalid loan assignment format");
                break;
            }

            int loan_id = atoi(loan_str);
            int emp_id = atoi(emp_str);

            if (manager_assign_loan(loan_id, emp_id) == 0) {
                build_success_response(&resp, "Loan assigned", NULL);
            } else {
                build_error_response(&resp, "Loan assignment failed");
            }
            break;
        }

        case CMD_MGR_FEEDBACK: {
            Feedback feedback[50];
            int count = manager_view_feedback(feedback, 50);

            build_success_response(&resp, "Feedback retrieved", NULL);
            for (int i = 0; i < count; i++) {
                append_response_data(&resp, "%d|%d|%s|%d\n",
                                     feedback[i].feedback_id,
                                     feedback[i].customer_id,
                                     feedback[i].message,
                                     feedback[i].status);
            }
            break;
        }

        case CMD_MGR_REVIEW_FEEDBACK: {
            int fb_id = atoi(req->payload);
            if (manager_review_feedback(fb_id) == 0) {
                build_success_response(&resp, "Feedback marked as reviewed",
                                       NULL);
            } else {
                build_error_response(&resp, "Review feedback failed");
            }
            break;
        }

        case CMD_MGR_CHANGE_PASS: {
            char *old_pass = strtok(req->payload, "|");
            char *new_pass = strtok(NULL, "|");

            if (!old_pass || !new_pass) {
                build_error_response(&resp, "Invalid password format");
            } else if (manager_change_password(req->user_id, old_pass,
                                               new_pass) == 0) {
                build_success_response(&resp, "Password changed", NULL);
            } else {
                build_error_response(&resp, "Password change failed");
            }
            break;
        }

        case CMD_ADM_ADD_EMP: {
            Employee emp;
            memset(&emp, 0, sizeof(emp));

            int fields = sscanf(req->payload, "%49[^|]|%49[^|]|%49[^|]",
                                emp.first_name, emp.last_name, emp.password);
            if (fields != 3) {
                build_error_response(&resp, "Invalid employee format");
                break;
            }

            int id = admin_add_employee(&emp);
            if (id > 0) {
                build_success_response(&resp, "Employee added", NULL);
                snprintf(resp.data, sizeof(resp.data), "%d", id);
            } else {
                build_error_response(&resp, "Add employee failed");
            }
            break;
        }

        case CMD_ADM_MOD_EMP: {
            int emp_id = 0;
            Employee emp;
            memset(&emp, 0, sizeof(emp));

            int fields = sscanf(req->payload, "%d|%49[^|]|%49[^|]|%49[^|]",
                                &emp_id, emp.first_name, emp.last_name,
                                emp.password);
            if (fields != 4) {
                build_error_response(&resp, "Invalid employee format");
            } else if (admin_modify_employee(emp_id, &emp) == 0) {
                build_success_response(&resp, "Employee modified", NULL);
            } else {
                build_error_response(&resp, "Modify employee failed");
            }
            break;
        }

        case CMD_ADM_DEL_EMP: {
            int emp_id = atoi(req->payload);
            if (admin_delete_employee(emp_id) == 0) {
                build_success_response(&resp, "Employee deactivated", NULL);
            } else {
                build_error_response(&resp, "Delete employee failed");
            }
            break;
        }

        case CMD_ADM_ROLE_MGMT: {
            char *emp_str = strtok(req->payload, "|");
            char *role_str = strtok(NULL, "|");

            if (!emp_str || !role_str) {
                build_error_response(&resp, "Invalid role format");
                break;
            }

            int emp_id = atoi(emp_str);
            int new_role = atoi(role_str);

            int new_id = admin_manage_role(emp_id, new_role);
            if (new_id > 0) {
                build_success_response(&resp, "Role updated", NULL);
                snprintf(resp.data, sizeof(resp.data), "%d", new_id);
            } else {
                build_error_response(&resp, "Role update failed");
            }
            break;
        }

        case CMD_ADM_CHANGE_PASS: {
            char *old_pass = strtok(req->payload, "|");
            char *new_pass = strtok(NULL, "|");

            if (!old_pass || !new_pass) {
                build_error_response(&resp, "Invalid password format");
            } else if (admin_change_password(req->user_id, old_pass,
                                             new_pass) == 0) {
                build_success_response(&resp, "Password changed", NULL);
            } else {
                build_error_response(&resp, "Password change failed");
            }
            break;
        }

        default:
            build_error_response(&resp, "Unknown command");
            break;
    }

    if (resp.success && req->command != CMD_LOGOUT) {
        session_update_activity(conn->session_idx);
    }

    finish_response(conn, &resp);
}

static void process_request(Connection *conn) {
    Request req;
    memcpy(&req, conn->read_buffer, sizeof(req));
    req.payload[sizeof(req.payload) - 1] = '\0';
    dispatch_request(conn, &req);
}

static void close_idle_connections(void) {
    time_t now = time(NULL);

    for (size_t i = 0; i < MAX_CONNECTIONS; i++) {
        Connection *conn = &connections[i];
        if (conn->fd < 0) continue;
        if (now - conn->last_activity > CONNECTION_TIMEOUT) {
            close_connection(conn);
        }
    }
}

static void accept_connections(void) {
    for (;;) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int client_fd = accept4(listen_fd, (struct sockaddr *)&client_addr,
                                &client_len, SOCK_NONBLOCK | SOCK_CLOEXEC);
        if (client_fd < 0) {
            if (errno == EINTR) continue;
            if (errno == EAGAIN || errno == EWOULDBLOCK) return;
            perror("accept4");
            return;
        }

        Connection *conn = NULL;
        for (size_t i = 0; i < MAX_CONNECTIONS; i++) {
            size_t slot = (next_slot + i) % MAX_CONNECTIONS;
            if (connections[slot].fd < 0) {
                conn = &connections[slot];
                next_slot = (slot + 1) % MAX_CONNECTIONS;
                break;
            }
        }

        if (!conn) {
            close(client_fd);
            continue;
        }

        conn->fd = client_fd;
        conn->read_pos = 0;
        conn->write_pos = 0;
        conn->state = CONN_READING_REQUEST;
        conn->last_activity = time(NULL);
        conn->session_idx = -1;

        struct epoll_event ev;
        memset(&ev, 0, sizeof(ev));
        ev.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
        ev.data.ptr = conn;

        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) < 0) {
            perror("epoll_ctl add");
            close_connection(conn);
        }
    }
}

static int handle_client_read(Connection *conn) {
    while (conn->read_pos < sizeof(conn->read_buffer)) {
        ssize_t n = read(conn->fd,
                         conn->read_buffer + conn->read_pos,
                         sizeof(conn->read_buffer) - conn->read_pos);
        if (n > 0) {
            conn->read_pos += (size_t)n;
            conn->last_activity = time(NULL);

            if (conn->read_pos == sizeof(conn->read_buffer)) {
                conn->state = CONN_PROCESSING;
                return 0;
            }
            continue;
        }

        if (n == 0) return -1;
        if (errno == EINTR) continue;
        if (errno == EAGAIN || errno == EWOULDBLOCK) return 0;
        return -1;
    }

    return 0;
}

static int handle_client_write(Connection *conn) {
    while (conn->write_pos < sizeof(conn->write_buffer)) {
        ssize_t n = write(conn->fd,
                          conn->write_buffer + conn->write_pos,
                          sizeof(conn->write_buffer) - conn->write_pos);
        if (n > 0) {
            conn->write_pos += (size_t)n;
            conn->last_activity = time(NULL);
            continue;
        }

        if (n < 0 && errno == EINTR) continue;
        if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) return 0;
        return -1;
    }

    conn->read_pos = 0;
    conn->write_pos = 0;
    conn->state = CONN_READING_REQUEST;
    return update_connection_events(conn, EPOLLIN | EPOLLET | EPOLLRDHUP);
}

static int setup_listen_socket(int port) {
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket");
        return -1;
    }

    int opt = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons((uint16_t)port);

    if (bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return -1;
    }

    if (listen(listen_fd, SOMAXCONN) < 0) {
        perror("listen");
        return -1;
    }

    if (set_nonblocking(listen_fd) < 0) {
        perror("set_nonblocking");
        return -1;
    }

    return 0;
}

int main(int argc, char *argv[]) {
    int port = (argc > 1) ? atoi(argv[1]) : PORT;

    signal(SIGINT, cleanup_server);
    signal(SIGTERM, cleanup_server);
    signal(SIGPIPE, SIG_IGN);

    for (size_t i = 0; i < MAX_CONNECTIONS; i++) {
        connections[i].fd = -1;
        connections[i].state = CONN_FREE;
    }

    init_data_files();

    if (setup_listen_socket(port) < 0) {
        cleanup_server(0);
    }

    epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    if (epoll_fd < 0) {
        perror("epoll_create1");
        cleanup_server(0);
    }

    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN;
    ev.data.ptr = NULL;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev) < 0) {
        perror("epoll_ctl listen");
        cleanup_server(0);
    }

    printf("Bank server (epoll) listening on port %d\n", port);

    struct epoll_event events[MAX_EVENTS];
    time_t last_timeout_check = time(NULL);
    unsigned long event_count = 0;

    for (;;) {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, EPOLL_TIMEOUT_MS);
        if (nfds < 0) {
            if (errno == EINTR) continue;
            perror("epoll_wait");
            break;
        }

        time_t now = time(NULL);
        if (now != last_timeout_check) {
            close_idle_connections();
            last_timeout_check = now;
        }

        for (int i = 0; i < nfds; i++) {
            if (++event_count >= 1000) {
                cleanup_expired_sessions();
                event_count = 0;
            }

            if (events[i].data.ptr == NULL) {
                accept_connections();
                continue;
            }

            Connection *conn = (Connection *)events[i].data.ptr;
            if (conn->fd < 0) continue;

            uint32_t mask = events[i].events;

            if (mask & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
                close_connection(conn);
                continue;
            }

            if ((mask & EPOLLIN) && conn->state == CONN_READING_REQUEST) {
                if (handle_client_read(conn) < 0) {
                    close_connection(conn);
                    continue;
                }
            }

            if (conn->state == CONN_PROCESSING) {
                process_request(conn);
                if (update_connection_events(conn,
                        EPOLLOUT | EPOLLET | EPOLLRDHUP) < 0) {
                    close_connection(conn);
                    continue;
                }
            }

            if ((mask & EPOLLOUT) && conn->state == CONN_WRITING_RESPONSE) {
                if (handle_client_write(conn) < 0) {
                    close_connection(conn);
                    continue;
                }
            }
        }
    }

    cleanup_server(0);
    return 0;
}
