#include "bank.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <poll.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

static int server_fd = -1;

void cleanup_server(int sig) {
    (void)sig;
    if (server_fd >= 0) close(server_fd);
    exit(0);
}

static char *append_line(char *p, char *end, const char *fmt, ...) {
    if (p >= end) return p;
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(p, (size_t)(end - p), fmt, ap);
    va_end(ap);
    if (n < 0 || p + n >= end) return end - 1;
    return p + n;
}

void handle_client(int client_fd) {
    Request req;
    Response resp;
    int auth_role = -1;
    int auth_user_id = -1;
    
    while (1) {
        ssize_t n = read(client_fd, &req, sizeof(Request));
        if (n <= 0) break;
        
        memset(&resp, 0, sizeof(Response));
        resp.success = 0;
        
        if (req.command != CMD_LOGIN) {
            if (auth_role < 0 || auth_user_id < 0) {
                strcpy(resp.message, "Authentication required");
                write(client_fd, &resp, sizeof(Response));
                continue;
            }
            if (req.role != auth_role || req.user_id != auth_user_id) {
                strcpy(resp.message, "Session mismatch");
                write(client_fd, &resp, sizeof(Response));
                continue;
            }
            if (!session_is_active(auth_user_id, auth_role)) {
                auth_role = -1;
                auth_user_id = -1;
                strcpy(resp.message, "Session invalid or expired");
                write(client_fd, &resp, sizeof(Response));
                continue;
            }
            if (!command_allowed_for_role(auth_role, req.command)) {
                strcpy(resp.message, "Permission denied for this role");
                write(client_fd, &resp, sizeof(Response));
                continue;
            }
        }
        
        switch (req.command) {
            case CMD_LOGIN: {
                if (auth_role >= 0) {
                    strcpy(resp.message, "Already logged in on this connection");
                    break;
                }
                char *role_str = strtok(req.payload, "|");
                char *id_str = strtok(NULL, "|");
                char *pass = strtok(NULL, "|");
                if (!role_str || !id_str || !pass) {
                    strcpy(resp.message, "Invalid login format");
                    break;
                }
                int role = atoi(role_str);
                int id = atoi(id_str);
                int success = 0;
                
                switch (role) {
                    case ROLE_CUSTOMER: success = customer_login(id, pass); break;
                    case ROLE_EMPLOYEE: success = employee_login(id, pass); break;
                    case ROLE_MANAGER: success = manager_login(id, pass); break;
                    case ROLE_ADMIN: success = admin_login(id, pass); break;
                }
                
                if (success) {
                    resp.success = 1;
                    auth_role = role;
                    auth_user_id = id;
                    snprintf(resp.message, sizeof(resp.message), "Login successful");
                    snprintf(resp.data, sizeof(resp.data), "%d|%d", role, id);
                } else {
                    strcpy(resp.message, "Invalid credentials or already logged in");
                }
                break;
            }
            
            case CMD_LOGOUT: {
                if (session_destroy(req.user_id, req.role) == 0) {
                    resp.success = 1;
                    auth_role = -1;
                    auth_user_id = -1;
                    strcpy(resp.message, "Logout successful");
                } else {
                    strcpy(resp.message, "Logout failed");
                }
                break;
            }
            
            case CMD_BALANCE: {
                int64_t bal;
                if (customer_balance(req.user_id, &bal) == 0) {
                    resp.success = 1;
                    session_update_activity(req.user_id, req.role);
                    snprintf(resp.message, sizeof(resp.message), "Balance: %lld", (long long)bal);
                    snprintf(resp.data, sizeof(resp.data), "%lld", (long long)bal);
                } else {
                    strcpy(resp.message, "Failed to get balance");
                }
                break;
            }
            
            case CMD_DEPOSIT: {
                int64_t amt = atoll(req.payload);
                if (customer_deposit(req.user_id, amt) == 0) {
                    resp.success = 1;
                    session_update_activity(req.user_id, req.role);
                    strcpy(resp.message, "Deposit successful");
                } else {
                    strcpy(resp.message, "Deposit failed");
                }
                break;
            }
            
            case CMD_WITHDRAW: {
                int64_t amt = atoll(req.payload);
                if (customer_withdraw(req.user_id, amt) == 0) {
                    resp.success = 1;
                    session_update_activity(req.user_id, req.role);
                    strcpy(resp.message, "Withdrawal successful");
                } else {
                    strcpy(resp.message, "Withdrawal failed (insufficient balance?)");
                }
                break;
            }
            
            case CMD_TRANSFER: {
                char *to_str = strtok(req.payload, "|");
                char *amt_str = strtok(NULL, "|");
                if (!to_str || !amt_str) {
                    strcpy(resp.message, "Invalid transfer format");
                    break;
                }
                int to_id = atoi(to_str);
                int64_t amt = atoll(amt_str);
                if (customer_transfer(req.user_id, to_id, amt) == 0) {
                    resp.success = 1;
                    session_update_activity(req.user_id, req.role);
                    strcpy(resp.message, "Transfer successful");
                } else {
                    strcpy(resp.message, "Transfer failed");
                }
                break;
            }
            
            case CMD_CHANGE_PASS: {
                char *old_p = strtok(req.payload, "|");
                char *new_p = strtok(NULL, "|");
                if (!old_p || !new_p) {
                    strcpy(resp.message, "Invalid format");
                    break;
                }
                int success = 0;
                switch (req.role) {
                    case ROLE_CUSTOMER: success = customer_change_password(req.user_id, old_p, new_p); break;
                    case ROLE_EMPLOYEE: success = employee_change_password(req.user_id, old_p, new_p); break;
                    case ROLE_MANAGER: success = manager_change_password(req.user_id, old_p, new_p); break;
                    case ROLE_ADMIN: success = admin_change_password(req.user_id, old_p, new_p); break;
                }
                if (success == 0) {
                    resp.success = 1;
                    session_update_activity(req.user_id, req.role);
                    strcpy(resp.message, "Password changed");
                } else {
                    strcpy(resp.message, "Password change failed");
                }
                break;
            }
            
            case CMD_LOAN_APPLY: {
                int64_t amt = atoll(req.payload);
                if (customer_loan_apply(req.user_id, amt) == 0) {
                    resp.success = 1;
                    session_update_activity(req.user_id, req.role);
                    strcpy(resp.message, "Loan application submitted");
                } else {
                    strcpy(resp.message, "Loan application failed");
                }
                break;
            }
            
            case CMD_TXN_HISTORY: {
                            Transaction txns[50];
                            int count = customer_txn_history(req.user_id, txns, 50);
                            resp.success = 1;
                            session_update_activity(req.user_id, req.role);
                            char *p = resp.data;
                            char *end = resp.data + sizeof(resp.data);
                            for (int i = 0; i < count; i++) {
                                p = append_line(p, end,
                                    "%d|%d|%lld|%d|%s\n",
                                    txns[i].transaction_id, txns[i].type, (long long)txns[i].amount,
                                    txns[i].target_customer_id, txns[i].timestamp);
                            }
                            break;
                        }
            
            case CMD_FEEDBACK: {
                if (customer_feedback(req.user_id, req.payload) == 0) {
                    resp.success = 1;
                    session_update_activity(req.user_id, req.role);
                    strcpy(resp.message, "Feedback submitted");
                } else {
                    strcpy(resp.message, "Feedback failed");
                }
                break;
            }
            
            /* Employee commands */
            case CMD_EMP_ADD_CUST: {
                Customer cust;
                memset(&cust, 0, sizeof(cust));
                if (sscanf(req.payload, "%49[^|]|%49[^|]|%49[^|]",
                           cust.first_name, cust.last_name, cust.password) != 3) {
                    strcpy(resp.message, "Invalid customer format");
                    break;
                }
                int id = employee_add_customer(&cust);
                if (id > 0) {
                    resp.success = 1;
                    session_update_activity(req.user_id, req.role);
                    snprintf(resp.data, sizeof(resp.data), "%d", id);
                } else {
                    strcpy(resp.message, "Add customer failed");
                }
                break;
            }
            
            case CMD_EMP_MOD_CUST: {
                int cid = 0;
                Customer cust;
                memset(&cust, 0, sizeof(cust));
                if (sscanf(req.payload, "%d|%49[^|]|%49[^|]|%49[^|]",
                           &cid, cust.first_name, cust.last_name, cust.password) != 4) {
                    strcpy(resp.message, "Invalid customer format");
                    break;
                }
                if (employee_modify_customer(cid, &cust) == 0) {
                    resp.success = 1;
                    session_update_activity(req.user_id, req.role);
                    strcpy(resp.message, "Customer modified");
                } else {
                    strcpy(resp.message, "Modify failed");
                }
                break;
            }
            
            case CMD_EMP_DEL_CUST: {
                int cid = atoi(req.payload);
                if (employee_delete_customer(cid) == 0) {
                    resp.success = 1;
                    session_update_activity(req.user_id, req.role);
                    strcpy(resp.message, "Customer deactivated");
                } else {
                    strcpy(resp.message, "Delete failed");
                }
                break;
            }
            
            case CMD_EMP_PROC_LOAN: {
                char *loan_str = strtok(req.payload, "|");
                char *status_str = strtok(NULL, "|");
                if (!loan_str || !status_str) {
                    strcpy(resp.message, "Invalid loan format");
                    break;
                }
                int lid = atoi(loan_str);
                int status = atoi(status_str);
                if (employee_process_loan(req.user_id, lid, status) == 0) {
                    resp.success = 1;
                    session_update_activity(req.user_id, req.role);
                    strcpy(resp.message, "Loan processed");
                } else {
                    strcpy(resp.message, "Process loan failed");
                }
                break;
            }
            
            case CMD_EMP_VIEW_LOANS: {
                Loan loans[50];
                int count = employee_view_loans(req.user_id, loans, 50);
                resp.success = 1;
                session_update_activity(req.user_id, req.role);
                char *p = resp.data;
                char *end = resp.data + sizeof(resp.data);
                for (int i = 0; i < count; i++) {
                    p = append_line(p, end,
                        "%d|%d|%lld|%d|%d\n",
                        loans[i].loan_id, loans[i].customer_id, (long long)loans[i].amount,
                        loans[i].status, loans[i].assigned_employee_id);
                }
                break;
            }
            
            case CMD_EMP_PASSBOOK: {
                int cid = atoi(req.payload);
                Transaction txns[50];
                int count = employee_passbook(cid, txns, 50);
                resp.success = 1;
                session_update_activity(req.user_id, req.role);
                char *p = resp.data;
                char *end = resp.data + sizeof(resp.data);
                for (int i = 0; i < count; i++) {
                    p = append_line(p, end,
                        "%d|%d|%lld|%d|%s\n",
                        txns[i].transaction_id, txns[i].type, (long long)txns[i].amount,
                        txns[i].target_customer_id, txns[i].timestamp);
                }
                break;
            }
            
            case CMD_EMP_CHANGE_PASS: {
                char *old_p = strtok(req.payload, "|");
                char *new_p = strtok(NULL, "|");
                if (!old_p || !new_p) {
                    strcpy(resp.message, "Invalid format");
                    break;
                }
                if (employee_change_password(req.user_id, old_p, new_p) == 0) {
                    resp.success = 1;
                    session_update_activity(req.user_id, req.role);
                    strcpy(resp.message, "Password changed");
                } else {
                    strcpy(resp.message, "Password change failed");
                }
                break;
            }
            
            /* Manager commands */
            case CMD_MGR_TOGGLE_ACC: {
                char *cid_str = strtok(req.payload, "|");
                char *status_str = strtok(NULL, "|");
                if (!cid_str || !status_str) {
                    strcpy(resp.message, "Invalid format");
                    break;
                }
                int cid = atoi(cid_str);
                int status = atoi(status_str);
                if (manager_toggle_account(cid, status) == 0) {
                    resp.success = 1;
                    session_update_activity(req.user_id, req.role);
                    strcpy(resp.message, "Account status updated");
                } else {
                    strcpy(resp.message, "Failed");
                }
                break;
            }
            
            case CMD_MGR_ASSIGN_LOAN: {
                char *lid_str = strtok(req.payload, "|");
                char *eid_str = strtok(NULL, "|");
                if (!lid_str || !eid_str) {
                    strcpy(resp.message, "Invalid format");
                    break;
                }
                int lid = atoi(lid_str);
                int eid = atoi(eid_str);
                if (manager_assign_loan(lid, eid) == 0) {
                    resp.success = 1;
                    session_update_activity(req.user_id, req.role);
                    strcpy(resp.message, "Loan assigned");
                } else {
                    strcpy(resp.message, "Assign failed");
                }
                break;
            }
            
            case CMD_MGR_FEEDBACK: {
                Feedback fbs[50];
                int count = manager_view_feedback(fbs, 50);
                resp.success = 1;
                session_update_activity(req.user_id, req.role);
                char *p = resp.data;
                char *end = resp.data + sizeof(resp.data);
                for (int i = 0; i < count; i++) {
                    p = append_line(p, end,
                        "%d|%d|%s|%d\n",
                        fbs[i].feedback_id, fbs[i].customer_id, fbs[i].message, fbs[i].status);
                }
                break;
            }
            
            case CMD_MGR_REVIEW_FEEDBACK: {
                int fb_id = atoi(req.payload);
                if (manager_review_feedback(fb_id) == 0) {
                    resp.success = 1;
                    session_update_activity(req.user_id, req.role);
                    strcpy(resp.message, "Feedback marked as reviewed");
                } else {
                    strcpy(resp.message, "Review feedback failed");
                }
                break;
            }
            
            case CMD_MGR_CHANGE_PASS: {
                char *old_p = strtok(req.payload, "|");
                char *new_p = strtok(NULL, "|");
                if (!old_p || !new_p) {
                    strcpy(resp.message, "Invalid format");
                    break;
                }
                if (manager_change_password(req.user_id, old_p, new_p) == 0) {
                    resp.success = 1;
                    session_update_activity(req.user_id, req.role);
                    strcpy(resp.message, "Password changed");
                } else {
                    strcpy(resp.message, "Password change failed");
                }
                break;
            }
            
            /* Admin commands */
            case CMD_ADM_ADD_EMP: {
                Employee emp;
                memset(&emp, 0, sizeof(emp));
                if (sscanf(req.payload, "%49[^|]|%49[^|]|%49[^|]",
                           emp.first_name, emp.last_name, emp.password) != 3) {
                    strcpy(resp.message, "Invalid employee format");
                    break;
                }
                int id = admin_add_employee(&emp);
                if (id > 0) {
                    resp.success = 1;
                    session_update_activity(req.user_id, req.role);
                    snprintf(resp.data, sizeof(resp.data), "%d", id);
                } else {
                    strcpy(resp.message, "Add employee failed");
                }
                break;
            }
            
            case CMD_ADM_MOD_EMP: {
                int eid = 0;
                Employee emp;
                memset(&emp, 0, sizeof(emp));
                if (sscanf(req.payload, "%d|%49[^|]|%49[^|]|%49[^|]",
                           &eid, emp.first_name, emp.last_name, emp.password) != 4) {
                    strcpy(resp.message, "Invalid employee format");
                    break;
                }
                if (admin_modify_employee(eid, &emp) == 0) {
                    resp.success = 1;
                    session_update_activity(req.user_id, req.role);
                    strcpy(resp.message, "Employee modified");
                } else {
                    strcpy(resp.message, "Modify failed");
                }
                break;
            }
            
            case CMD_ADM_DEL_EMP: {
                int eid = atoi(req.payload);
                if (admin_delete_employee(eid) == 0) {
                    resp.success = 1;
                    session_update_activity(req.user_id, req.role);
                    strcpy(resp.message, "Employee deactivated");
                } else {
                    strcpy(resp.message, "Delete failed");
                }
                break;
            }
            
            case CMD_ADM_ROLE_MGMT: {
                char *emp_str = strtok(req.payload, "|");
                char *role_str = strtok(NULL, "|");
                if (!emp_str || !role_str) {
                    strcpy(resp.message, "Invalid role format");
                    break;
                }
                int eid = atoi(emp_str);
                int new_role = atoi(role_str);
                int new_id = admin_manage_role(eid, new_role);
                if (new_id > 0) {
                    resp.success = 1;
                    session_update_activity(req.user_id, req.role);
                    snprintf(resp.message, sizeof(resp.message), "Role updated");
                    snprintf(resp.data, sizeof(resp.data), "%d", new_id);
                } else {
                    strcpy(resp.message, "Role update failed");
                }
                break;
            }
            
            case CMD_ADM_CHANGE_PASS: {
                char *old_p = strtok(req.payload, "|");
                char *new_p = strtok(NULL, "|");
                if (!old_p || !new_p) {
                    strcpy(resp.message, "Invalid format");
                    break;
                }
                if (admin_change_password(req.user_id, old_p, new_p) == 0) {
                    resp.success = 1;
                    session_update_activity(req.user_id, req.role);
                    strcpy(resp.message, "Password changed");
                } else {
                    strcpy(resp.message, "Password change failed");
                }
                break;
            }
            
            default:
                strcpy(resp.message, "Unknown command");
                break;
        }
        
        write(client_fd, &resp, sizeof(Response));
    }
    
    close(client_fd);
}

int main(int argc, char *argv[]) {
    int port = (argc > 1) ? atoi(argv[1]) : PORT;
    
    signal(SIGINT, cleanup_server);
    signal(SIGTERM, cleanup_server);
    signal(SIGCHLD, SIG_IGN);
    
    init_data_files();
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return 1;
    }
    
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(port)
    };
    
    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }
    
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("listen");
        return 1;
    }
    
    printf("Bank server listening on port %d\n", port);
    
    int reaper_counter = 0;
    
    while (1) {
        /* Run session reaper periodically (every 100 iterations) to clean up expired sessions */
        if (++reaper_counter >= 100) {
            session_reaper();
            reaper_counter = 0;
        }
        
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }
        
        pid_t pid = fork();
        if (pid == 0) {
            close(server_fd);
            handle_client(client_fd);
            exit(0);
        } else if (pid > 0) {
            close(client_fd);
        } else {
            perror("fork");
            close(client_fd);
        }
    }
    
    return 0;
}