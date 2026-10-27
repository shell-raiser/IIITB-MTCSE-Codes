#include "bank.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <termios.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static int sock_fd = -1;
static int current_role = -1;
static int current_user_id = -1;

void get_password(char *buf, size_t len) {
    if (isatty(STDIN_FILENO)) {
        struct termios old, new;
        tcgetattr(STDIN_FILENO, &old);
        new = old;
        new.c_lflag &= ~ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &new);
        fgets(buf, len, stdin);
        tcsetattr(STDIN_FILENO, TCSANOW, &old);
        printf("\n");
    } else {
        fgets(buf, len, stdin);
    }
    buf[strcspn(buf, "\n")] = '\0';
}

static int64_t parse_amount(const char *text) {
    char *end = NULL;
    double value = strtod(text, &end);
    if (end == text || value <= 0) return -1;
    while (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r') end++;
    if (*end != '\0') return -1;
    return (int64_t)(value * (double)MICRO_UNITS + 0.5);
}

int send_request(int cmd, const char *payload) {
    Request req = {
        .role = current_role,
        .user_id = current_user_id,
        .command = cmd
    };
    if (payload) strncpy(req.payload, payload, sizeof(req.payload) - 1);
    
    if (write(sock_fd, &req, sizeof(Request)) != sizeof(Request)) return -1;
    
    Response resp;
    if (read(sock_fd, &resp, sizeof(Response)) != sizeof(Response)) return -1;
    
    if (resp.success) {
        printf("✓ %s\n", resp.message);
        if (resp.data[0]) printf("%s\n", resp.data);
    } else {
        printf("✗ %s\n", resp.message);
    }
    return resp.success ? 0 : -1;
}

void customer_menu() {
    char input[256];
    while (1) {
        printf("\n=== Customer Menu ===\n");
        printf("1. Balance\n2. Deposit\n3. Withdraw\n4. Transfer\n");
        printf("5. Change Password\n6. Apply Loan\n7. Transaction History\n");
        printf("8. Feedback\n9. Logout\nChoice: ");
        if (!fgets(input, sizeof(input), stdin)) break;
        int choice = atoi(input);
        
        switch (choice) {
            case 1: send_request(CMD_BALANCE, NULL); break;
            case 2: {
                printf("Amount: "); if (!fgets(input, sizeof(input), stdin)) break;
                int64_t amount = parse_amount(input);
                if (amount <= 0) { printf("Invalid amount\n"); break; }
                char payload[64];
                snprintf(payload, sizeof(payload), "%lld", (long long)amount);
                send_request(CMD_DEPOSIT, payload); break;
            }
            case 3: {
                printf("Amount: "); if (!fgets(input, sizeof(input), stdin)) break;
                int64_t amount = parse_amount(input);
                if (amount <= 0) { printf("Invalid amount\n"); break; }
                char payload[64];
                snprintf(payload, sizeof(payload), "%lld", (long long)amount);
                send_request(CMD_WITHDRAW, payload); break;
            }
            case 4: {
                char to[64], amt[64], payload[160];
                printf("To Account: "); if (!fgets(to, sizeof(to), stdin)) break;
                to[strcspn(to, "\n")] = '\0';
                printf("Amount: "); if (!fgets(amt, sizeof(amt), stdin)) break;
                int64_t amount = parse_amount(amt);
                if (amount <= 0) { printf("Invalid amount\n"); break; }
                snprintf(payload, sizeof(payload), "%s|%lld", to, (long long)amount);
                send_request(CMD_TRANSFER, payload); break;
            }
            case 5:
                printf("Old password: "); get_password(input, sizeof(input));
                char old[64]; strcpy(old, input);
                printf("New password: "); char newp[64];
                get_password(newp, sizeof(newp));
                char cp_payload[160];
                snprintf(cp_payload, sizeof(cp_payload), "%s|%s", old, newp);
                send_request(CMD_CHANGE_PASS, cp_payload); break;
            case 6: {
                printf("Loan Amount: "); if (!fgets(input, sizeof(input), stdin)) break;
                int64_t amount = parse_amount(input);
                if (amount <= 0) { printf("Invalid amount\n"); break; }
                char payload[64];
                snprintf(payload, sizeof(payload), "%lld", (long long)amount);
                send_request(CMD_LOAN_APPLY, payload); break;
            }
            case 7: send_request(CMD_TXN_HISTORY, NULL); break;
            case 8:
                printf("Feedback: "); if (!fgets(input, sizeof(input), stdin)) break;
                input[strcspn(input, "\n")] = '\0';
                send_request(CMD_FEEDBACK, input); break;
            case 9: send_request(CMD_LOGOUT, NULL); return;
            default: printf("Invalid choice\n");
        }
    }
}

void employee_menu() {
    char input[512];
    while (1) {
        printf("\n=== Employee Menu ===\n");
        printf("1. Add Customer\n2. Modify Customer\n3. Delete Customer\n");
        printf("4. Process Loan\n5. View Assigned Loans\n6. Customer Passbook\n");
        printf("7. Change Password\n8. Logout\nChoice: ");
        if (!fgets(input, sizeof(input), stdin)) break;
        int choice = atoi(input);
        
        switch (choice) {
            case 1:
                printf("First Name: "); if (!fgets(input, sizeof(input), stdin)) break; input[strcspn(input, "\n")] = '\0';
                char fn[50]; strcpy(fn, input);
                printf("Last Name: "); if (!fgets(input, sizeof(input), stdin)) break; input[strcspn(input, "\n")] = '\0';
                char ln[50]; strcpy(ln, input);
                printf("Password: "); char pass[64];
                get_password(pass, sizeof(pass));
                char add_payload[256];
                snprintf(add_payload, sizeof(add_payload), "%s|%s|%s", fn, ln, pass);
                send_request(CMD_EMP_ADD_CUST, add_payload); break;
            case 2:
                printf("Customer ID: "); if (!fgets(input, sizeof(input), stdin)) break; input[strcspn(input, "\n")] = '\0';
                strcat(input, "|");
                printf("First Name: "); if (!fgets(input + strlen(input), sizeof(input) - strlen(input), stdin)) break; input[strcspn(input, "\n")] = '\0';
                strcat(input, "|");
                printf("Last Name: "); if (!fgets(input + strlen(input), sizeof(input) - strlen(input), stdin)) break; input[strcspn(input, "\n")] = '\0';
                strcat(input, "|");
                printf("Password: "); get_password(input + strlen(input), sizeof(input) - strlen(input));
                send_request(CMD_EMP_MOD_CUST, input); break;
            case 3:
                printf("Customer ID: "); if (!fgets(input, sizeof(input), stdin)) break;
                send_request(CMD_EMP_DEL_CUST, input); break;
            case 4:
                printf("Loan ID: "); if (!fgets(input, sizeof(input), stdin)) break; input[strcspn(input, "\n")] = '\0';
                strcat(input, "|");
                printf("Status (1=approve, 2=reject): "); if (!fgets(input + strlen(input), sizeof(input) - strlen(input), stdin)) break;
                send_request(CMD_EMP_PROC_LOAN, input); break;
            case 5: send_request(CMD_EMP_VIEW_LOANS, NULL); break;
            case 6:
                printf("Customer ID: "); if (!fgets(input, sizeof(input), stdin)) break;
                send_request(CMD_EMP_PASSBOOK, input); break;
            case 7:
                printf("Old password: "); get_password(input, sizeof(input));
                char old[64]; strcpy(old, input);
                printf("New password: "); char newp[64];
                get_password(newp, sizeof(newp));
                char cp_payload[160];
                snprintf(cp_payload, sizeof(cp_payload), "%s|%s", old, newp);
                send_request(CMD_CHANGE_PASS, cp_payload); break;
            case 8: send_request(CMD_LOGOUT, NULL); return;
            default: printf("Invalid choice\n");
        }
    }
}

void manager_menu() {
    char input[256];
    while (1) {
        printf("\n=== Manager Menu ===\n");
        printf("1. Activate/Deactivate Account\n2. Assign Loan to Employee\n");
        printf("3. View Feedback\n4. Review Feedback\n5. Change Password\n6. Logout\nChoice: ");
        if (!fgets(input, sizeof(input), stdin)) break;
        int choice = atoi(input);
        
        switch (choice) {
            case 1:
                printf("Customer ID: "); if (!fgets(input, sizeof(input), stdin)) break; input[strcspn(input, "\n")] = '\0';
                strcat(input, "|");
                printf("Status (0=active, 1=inactive): "); if (!fgets(input + strlen(input), sizeof(input) - strlen(input), stdin)) break;
                send_request(CMD_MGR_TOGGLE_ACC, input); break;
            case 2:
                printf("Loan ID: "); if (!fgets(input, sizeof(input), stdin)) break; input[strcspn(input, "\n")] = '\0';
                strcat(input, "|");
                printf("Employee ID: "); if (!fgets(input + strlen(input), sizeof(input) - strlen(input), stdin)) break;
                send_request(CMD_MGR_ASSIGN_LOAN, input); break;
            case 3: send_request(CMD_MGR_FEEDBACK, NULL); break;
            case 4:
                printf("Feedback ID: "); if (!fgets(input, sizeof(input), stdin)) break;
                send_request(CMD_MGR_REVIEW_FEEDBACK, input); break;
            case 5:
                printf("Old password: "); get_password(input, sizeof(input));
                char old[64]; strcpy(old, input);
                printf("New password: "); char newp[64];
                get_password(newp, sizeof(newp));
                char cp_payload[160];
                snprintf(cp_payload, sizeof(cp_payload), "%s|%s", old, newp);
                send_request(CMD_CHANGE_PASS, cp_payload); break;
            case 6: send_request(CMD_LOGOUT, NULL); return;
            default: printf("Invalid choice\n");
        }
    }
}

void admin_menu() {
    char input[512];
    while (1) {
        printf("\n=== Admin Menu ===\n");
        printf("1. Add Employee\n2. Modify Employee\n3. Delete Employee\n");
        printf("4. Role Management\n5. Change Password\n6. Logout\nChoice: ");
        if (!fgets(input, sizeof(input), stdin)) break;
        int choice = atoi(input);
        
        switch (choice) {
            case 1:
                printf("First Name: "); fgets(input, sizeof(input), stdin); input[strcspn(input, "\n")] = '\0';
                char fn[50]; strcpy(fn, input);
                printf("Last Name: "); fgets(input, sizeof(input), stdin); input[strcspn(input, "\n")] = '\0';
                char ln[50]; strcpy(ln, input);
                printf("Password: "); char pass[64];
                get_password(pass, sizeof(pass));
                char add_payload[256];
                snprintf(add_payload, sizeof(add_payload), "%s|%s|%s", fn, ln, pass);
                send_request(CMD_ADM_ADD_EMP, add_payload); break;
            case 2:
                printf("Employee ID: "); fgets(input, sizeof(input), stdin); input[strcspn(input, "\n")] = '\0';
                strcat(input, "|");
                printf("First Name: "); fgets(input + strlen(input), sizeof(input) - strlen(input), stdin);
                input[strcspn(input, "\n")] = '\0';
                strcat(input, "|");
                printf("Last Name: "); fgets(input + strlen(input), sizeof(input) - strlen(input), stdin);
                input[strcspn(input, "\n")] = '\0';
                strcat(input, "|");
                printf("Password: "); get_password(input + strlen(input), sizeof(input) - strlen(input));
                send_request(CMD_ADM_MOD_EMP, input); break;
            case 3:
                printf("Employee ID: "); fgets(input, sizeof(input), stdin);
                send_request(CMD_ADM_DEL_EMP, input); break;
            case 4:
                printf("Employee ID: "); if (!fgets(input, sizeof(input), stdin)) break;
                input[strcspn(input, "\n")] = '\0';
                strncat(input, "|", sizeof(input) - strlen(input) - 1);
                printf("New Role (1=Employee, 2=Manager, 3=Admin): ");
                if (!fgets(input + strlen(input), sizeof(input) - strlen(input), stdin)) break;
                send_request(CMD_ADM_ROLE_MGMT, input); break;
            case 5:
                printf("Old password: "); get_password(input, sizeof(input));
                char old[64]; strcpy(old, input);
                printf("New password: "); char newp[64];
                get_password(newp, sizeof(newp));
                char cp_payload[160];
                snprintf(cp_payload, sizeof(cp_payload), "%s|%s", old, newp);
                send_request(CMD_CHANGE_PASS, cp_payload); break;
            case 6: send_request(CMD_LOGOUT, NULL); return;
            default: printf("Invalid choice\n");
        }
    }
}

int main(int argc, char *argv[]) {
    const char *host = (argc > 1) ? argv[1] : "127.0.0.1";
    int port = (argc > 2) ? atoi(argv[2]) : PORT;
    
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) { perror("socket"); return 1; }
    
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr.s_addr = inet_addr(host)
    };
    
    if (connect(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect"); return 1;
    }
    
    printf("Connected to bank server at %s:%d\n", host, port);
    
    while (1) {
        printf("\n=== Login ===\n");
        printf("Role (0=Customer, 1=Employee, 2=Manager, 3=Admin): ");
        char input[64];
        if (!fgets(input, sizeof(input), stdin)) break;
        int role = atoi(input);
        
        printf("User ID: "); if (!fgets(input, sizeof(input), stdin)) break;
        int id = atoi(input);
        
        printf("Password: "); get_password(input, sizeof(input));
        
        char payload[128];
        snprintf(payload, sizeof(payload), "%d|%d|%s", role, id, input);
        
        if (send_request(CMD_LOGIN, payload) == 0) {
            current_role = role;
            current_user_id = id;
            
            switch (role) {
                case ROLE_CUSTOMER: customer_menu(); break;
                case ROLE_EMPLOYEE: employee_menu(); break;
                case ROLE_MANAGER: manager_menu(); break;
                case ROLE_ADMIN: admin_menu(); break;
            }
            
            current_role = -1;
            current_user_id = -1;
        }
    }
    
    close(sock_fd);
    return 0;
}