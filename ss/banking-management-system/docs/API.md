# API Specification

## Protocol Overview

**Transport:** TCP/IP  
**Port:** 8081 (configurable)  
**Encoding:** Binary structs (network byte order = host, same machine)  
**Concurrency:** One connection per client, fork-per-client server

## Request Format

```c
typedef struct {
    int role;       // 0=Customer, 1=Employee, 2=Manager, 3=Admin
    int user_id;    // Role-specific ID (1-based)
    int command;    // Command code (see below)
    char payload[512]; // Command-specific data
} Request;  // sizeof = 524 bytes
```

## Response Format

```c
typedef struct {
    int success;        // 1 = success, 0 = failure
    char message[512];  // Human-readable message
    char data[1024];    // Structured response data
} Response;  // sizeof = 1540 bytes
```

## Command Codes

### Authentication (All Roles)
| Code | Name | Value | Payload | Response Data |
|------|------|-------|---------|---------------|
| 1 | CMD_LOGIN | `role\|user_id\|password` | `"3|1|admin123"` | `"role\|user_id"` (e.g., `"3|1"`) |
| 2 | CMD_LOGOUT | (empty) | `""` | (empty) |

### Customer Commands (0)
| Code | Name | Value | Payload | Response Data |
|------|------|-------|---------|---------------|
| 3 | CMD_BALANCE | (empty) | `""` | Balance in micro-units (e.g., `"500000000"`) |
| 4 | CMD_DEPOSIT | Amount (micro-units) | `"100000000"` | (empty) |
| 5 | CMD_WITHDRAW | Amount (micro-units) | `"50000000"` | (empty) |
| 6 | CMD_TRANSFER | `to_id\|amount` | `"4|200000000"` | (empty) |
| 7 | CMD_CHANGE_PASS | `old\|new` | `"oldpass\|newpass"` | (empty) |
| 8 | CMD_LOAN_APPLY | Amount (micro-units) | `"5000000000"` | (empty) |
| 9 | CMD_TXN_HISTORY | (empty) | `""` | Lines: `txn_id\|type\|amount\|target_id\|timestamp` |
| 10 | CMD_FEEDBACK | Message text | `"Great service!"` | (empty) |

### Employee Commands (1)
| Code | Name | Value | Payload | Response Data |
|------|------|-------|---------|---------------|
| 20 | CMD_EMP_ADD_CUST | `fn\|ln\|password` | `"John\|Doe\|pass123"` | New customer ID (e.g., `"5"`) |
| 21 | CMD_EMP_MOD_CUST | `cid\|fn\|ln\|password` | `"5\|Jane\|Smith\|newpass"` | (empty) |
| 22 | CMD_EMP_DEL_CUST | Customer ID | `"5"` | (empty) |
| 23 | CMD_EMP_PROC_LOAN | `loan_id\|status` | `"1\|1"` (1=approve, 2=reject) | (empty) |
| 24 | CMD_EMP_VIEW_LOANS | (empty) | `""` | Lines: `loan_id\|cust_id\|amount\|status\|emp_id` |
| 25 | CMD_EMP_PASSBOOK | Customer ID | `"5"` | Lines: `txn_id\|type\|amount\|target_id\|timestamp` |
| 26 | CMD_EMP_CHANGE_PASS | `old\|new` | `"oldpass\|newpass"` | (empty) |

### Manager Commands (5)
| Code | Name | Value | Payload | Response Data |
|------|------|-------|---------|---------------|
| 40 | CMD_MGR_TOGGLE_ACC | `cust_id\|status` | `"5\|1"` (0=active, 1=inactive) | (empty) |
| 41 | CMD_MGR_ASSIGN_LOAN | `loan_id\|emp_id` | `"1\|2"` | (empty) |
| 42 | CMD_MGR_FEEDBACK | (empty) | `""` | Lines: `fb_id\|cust_id\|message\|status` |
| 43 | CMD_MGR_CHANGE_PASS | `old\|new` | `"oldpass\|newpass"` | (empty) |
| 44 | CMD_MGR_REVIEW_FEEDBACK | Feedback ID | `"2"` | (empty) |

### Admin Commands (5)
| Code | Name | Value | Payload | Response Data |
|------|------|-------|---------|---------------|
| 60 | CMD_ADM_ADD_EMP | `fn\|ln\|password` | `"John\|Doe\|emp123"` | New employee ID (e.g., `"3"`) |
| 61 | CMD_ADM_MOD_EMP | `eid\|fn\|ln\|password` | `"3\|Jane\|Smith\|newpass"` | (empty) |
| 62 | CMD_ADM_DEL_EMP | Employee ID | `"3"` | (empty) |
| 63 | CMD_ADM_ROLE_MGMT | `emp_id\|new_role` | `"3\|2"` (2=manager, 3=admin) | New ID in target role table |
| 64 | CMD_ADM_CHANGE_PASS | `old\|new` | `"oldpass\|newpass"` | (empty) |

## Transaction Types (in TXN_HISTORY)

| Type | Code | Description |
|------|------|-------------|
| TXN_DEPOSIT | 0 | Money deposited |
| TXN_WITHDRAW | 1 | Money withdrawn |
| TXN_TRANSFER | 2 | Transfer to/from another account |

**Direction:** Positive amount = credit, negative = debit (determined by context)

## Loan Statuses

| Status | Code | Meaning |
|--------|------|---------|
| LOAN_PENDING | 0 | Applied, awaiting assignment |
| LOAN_APPROVED | 1 | Approved, amount credited |
| LOAN_REJECTED | 2 | Rejected |

## Account Statuses

| Status | Code | Meaning |
|--------|------|---------|
| ACC_STATUS_ACTIVE | 0 | Normal operations allowed |
| ACC_STATUS_INACTIVE | 1 | All operations rejected |

## Error Responses

All failures return `success=0` with descriptive message:

| Scenario | Message |
|----------|---------|
| Invalid credentials | `"Invalid credentials"` |
| Insufficient balance | `"Withdrawal failed (insufficient balance?)"` |
| Account inactive | `"Deposit failed"` / `"Withdrawal failed"` |
| Self transfer | `"Transfer failed"` |
| Non-existent target | `"Transfer failed"` |
| Loan already pending | `"Loan application failed"` |
| Invalid loan ID | `"Assign failed"` |
| Invalid employee ID | `"Assign failed"` |
| Employee not active | `"Assign failed"` |
| Loan not pending | `"Process loan failed"` / `"Assign failed"` |
| Already assigned | `"Assign failed"` |
| Session conflict | `"Invalid credentials"` (login) / `"Already logged in elsewhere"` |
| Session expired | Login succeeds (old session destroyed) |

## Example Exchange

### Login (Admin)
```
Client → Server: Request{role=3, user_id=1, command=1, payload="3|1|admin123"}
Server → Client: Response{success=1, message="Login successful", data="3|1"}
```

### Deposit (Customer)
```
Client → Server: Request{role=0, user_id=1, command=4, payload="100000000"}
Server → Client: Response{success=1, message="Deposit successful", data=""}
```

### Transfer (Customer)
```
Client → Server: Request{role=0, user_id=1, command=6, payload="4|50000000"}
Server → Client: Response{success=1, message="Transfer successful", data=""}
```

### Loan Workflow
```
1. Customer applies:
   C→S: CMD_LOAN_APPLY, "5000000000"
   S→C: "Loan application submitted"

2. Manager assigns:
   C→S: CMD_MGR_ASSIGN_LOAN, "1|2"
   S→C: "Loan assigned"

3. Employee approves:
   C→S: CMD_EMP_PROC_LOAN, "1|1"
   S→C: "Loan processed"
   (Customer balance automatically credited)

4. Customer checks balance:
   C→S: CMD_BALANCE
   S→C: Balance includes loan amount
```

## Python Client Example

```python
import socket
import struct

REQUEST_FMT = 'iii512s'
RESPONSE_FMT = 'i512s1024s'

class BankClient:
    def __init__(self, host='127.0.0.1', port=8081):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect((host, port))
        self.role = None
        self.user_id = None
    
    def send(self, cmd, payload=''):
        req = struct.pack(REQUEST_FMT, self.role, self.user_id, cmd, payload.encode()[:512])
        self.sock.sendall(req)
        resp = self.sock.recv(struct.calcsize(RESPONSE_FMT))
        success, msg, data = struct.unpack(RESPONSE_FMT, resp)
        return success == 1, msg.decode().rstrip('\x00'), data.decode().rstrip('\x00')
    
    def login(self, role, user_id, password):
        self.role = role
        self.user_id = user_id
        return self.send(1, f'{role}|{user_id}|{password}')
    
    def balance(self):
        return self.send(3)
    
    def deposit(self, amount):
        return self.send(4, str(amount))
    
    def transfer(self, to_id, amount):
        return self.send(6, f'{to_id}|{amount}')

# Usage
c = BankClient()
c.login(3, 1, 'admin123')  # Admin
c.login(0, 1, 'cust123')   # Customer
c.deposit(100000000)        # Deposit 100.00
c.transfer(2, 50000000)     # Transfer 50.00 to customer 2
```

## Wire Shark / Debugging

Raw bytes on wire (Request = 524 bytes, Response = 1540 bytes):

```
Request (hex):
03 00 00 00  01 00 00 00  04 00 00 00  31 30 30 30 30 30 30 30 30 00 00 ...
│            │            │            └─ command=4 (DEPOSIT)
│            │            └─ user_id=1
│            └─ role=3 (Admin)
└─ payload="100000000" (100.00 in micro-units)

Response (hex):
01 00 00 00  44 65 70 6f 73 69 74 20 73 75 63 63 65 73 73 66 75 6c 00 ...
│            └─ message="Deposit successful"
└─ success=1
```

## Version Compatibility

- Protocol v1 (current): Fixed struct sizes, no version field
- Future: Add `version` field to Request/Response for backward compatibility
- Breaking changes require new command codes (not reusing)