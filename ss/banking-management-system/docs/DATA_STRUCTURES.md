# Data Structures Reference

All structs use `__attribute__((packed))` for deterministic binary layout.  
Record size is fixed at **512 bytes** (zero-padded on write).

## Sizes Summary

| Struct | Packed Size | Record Size | Padding |
|--------|-------------|-------------|---------|
| Customer | 170 bytes | 512 bytes | 342 bytes |
| Employee | 362 bytes | 512 bytes | 150 bytes |
| Admin | 154 bytes | 512 bytes | 358 bytes |
| Manager | 154 bytes | 512 bytes | 358 bytes |
| Transaction | 52 bytes | 512 bytes | 460 bytes |
| Loan | 20 bytes | 512 bytes | 492 bytes |
| Feedback | 268 bytes | 512 bytes | 244 bytes |
| Session | 24 bytes | 512 bytes | 488 bytes |

---

## Customer (customers.dat)

```c
typedef struct {
    int user_id;                    // 4 bytes  - Primary key (1-based)
    char first_name[MAX_NAME_LEN];  // 50 bytes
    char last_name[MAX_NAME_LEN];   // 50 bytes
    char password[MAX_PASSWORD_LEN];// 50 bytes
    int64_t balance;                // 8 bytes  - Micro-units (1/1,000,000)
    int loan_status;                // 4 bytes  - 0=PENDING, 1=APPROVED, 2=REJECTED
    int64_t loan_amount;            // 8 bytes  - Micro-units
    int account_status;             // 4 bytes  - 0=ACTIVE, 1=INACTIVE
} __attribute__((packed)) Customer; // Total: 170 bytes
```

### Field Details
| Field | Type | Constraints | Notes |
|-------|------|-------------|-------|
| user_id | int | Auto-assigned, 1..N | File record = user_id - 1 |
| first_name | char[50] | Null-terminated | Max 49 visible chars |
| last_name | char[50] | Null-terminated | Max 49 visible chars |
| password | char[50] | Null-terminated | Plaintext (assignment) |
| balance | int64_t | ≥ 0 | Micro-units: 100.00 = 100000000 |
| loan_status | int | 0,1,2 | PENDING/APPROVED/REJECTED |
| loan_amount | int64_t | ≥ 0 | Only valid if PENDING/APPROVED |
| account_status | int | 0,1 | ACTIVE/INACTIVE |

### Record Layout (hex example)
```
Offset 0x00: 01 00 00 00          // user_id = 1
Offset 0x04: 4A 6F 68 6E 00 ...    // first_name = "John\0" + padding
Offset 0x36: 44 6F 65 00 00 ...    // last_name = "Doe\0" + padding
Offset 0x68: 70 61 73 73 31 32 33 00 ...  // password = "pass123\0" + padding
Offset 0x9A: 00 00 00 00 00 00 00 00  // balance = 0 (int64, little-endian)
Offset 0xA2: 00 00 00 00          // loan_status = 0 (REJECTED)
Offset 0xA6: 00 00 00 00 00 00 00 00  // loan_amount = 0
Offset 0xAE: 00 00 00 00          // account_status = 0 (ACTIVE)
Offset 0xB2: 00 ... (padding to 512)
```

---

## Employee (employees.dat)

```c
typedef struct {
    int employee_id;              // 4 bytes  - Primary key (1-based)
    char first_name[MAX_NAME_LEN];// 50 bytes
    char last_name[MAX_NAME_LEN]; // 50 bytes
    char password[MAX_PASSWORD_LEN];// 50 bytes
    int status;                   // 4 bytes  - 0=ACTIVE, 1=INACTIVE
    int loan_count;               // 4 bytes  - Number of assigned loans
    int assigned_loans[50];       // 200 bytes - Loan IDs (max 50)
} __attribute__((packed)) Employee; // Total: 362 bytes
```

### Field Details
| Field | Type | Constraints | Notes |
|-------|------|-------------|-------|
| employee_id | int | Auto-assigned, 1..N | File record = employee_id - 1 |
| first_name | char[50] | Null-terminated | |
| last_name | char[50] | Null-terminated | |
| password | char[50] | Null-terminated | |
| status | int | 0,1 | ACTIVE/INACTIVE |
| loan_count | int | 0..50 | Count of assigned_loans in use |
| assigned_loans | int[50] | Loan IDs | Up to 50 concurrent loans |

---

## Admin (admins.dat)

```c
typedef struct {
    int admin_id;                 // 4 bytes  - Primary key (1-based)
    char first_name[MAX_NAME_LEN];// 50 bytes
    char last_name[MAX_NAME_LEN]; // 50 bytes
    char password[MAX_PASSWORD_LEN];// 50 bytes
} __attribute__((packed)) Admin; // Total: 154 bytes
```

---

## Manager (managers.dat)

```c
typedef struct {
    int manager_id;               // 4 bytes  - Primary key (1-based)
    char first_name[MAX_NAME_LEN];// 50 bytes
    char last_name[MAX_NAME_LEN]; // 50 bytes
    char password[MAX_PASSWORD_LEN];// 50 bytes
} __attribute__((packed)) Manager; // Total: 154 bytes
```

---

## Transaction (transactions.dat)

```c
typedef struct {
    int transaction_id;           // 4 bytes  - Auto-increment
    int customer_id;              // 4 bytes  - Owner of transaction
    int type;                     // 4 bytes  - 0=DEPOSIT, 1=WITHDRAW, 2=TRANSFER
    int64_t amount;               // 8 bytes  - Micro-units (always positive)
    int target_customer_id;       // 4 bytes  - For transfers: other party
    char timestamp[32];           // 32 bytes - "YYYY-MM-DD HH:MM:SS"
} __attribute__((packed)) Transaction; // Total: 52 bytes
```

### Type Values
| Type | Value | Description |
|------|-------|-------------|
| TXN_DEPOSIT | 0 | Credit to account |
| TXN_WITHDRAW | 1 | Debit from account |
| TXN_TRANSFER | 2 | Transfer (direction by context) |

**Direction Logic:**
- DEPOSIT: `amount` is credit
- WITHDRAW: `amount` is debit
- TRANSFER: 
  - If `customer_id == from`: debit
  - If `customer_id == to`: credit
  - `target_customer_id` identifies counterparty

---

## Loan (loans.dat)

```c
typedef struct {
    int loan_id;                  // 4 bytes  - Auto-increment
    int customer_id;              // 4 bytes  - Applicant
    int64_t amount;               // 8 bytes  - Micro-units
    int status;                   // 4 bytes  - 0=PENDING, 1=APPROVED, 2=REJECTED
    int assigned_employee_id;     // 4 bytes  - Assigned employee (or -1)
} __attribute__((packed)) Loan; // Total: 20 bytes
```

### State Machine
```
PENDING (0)
    │
    ├─ Manager assigns ──▶ PENDING (assigned_employee_id set)
    │                         │
    │                         ├─ Employee approves ──▶ APPROVED (1)
    │                         │                         │
    │                         │                         └─ Credit customer balance
    │                         │                            Reset loan_status/amount
    │                         │
    │                         └─ Employee rejects ──▶ REJECTED (2)
    │                                                    Reset loan_status/amount
    │
    └─ (Re-apply allowed if REJECTED)
```

---

## Feedback (feedback.dat)

```c
typedef struct {
    int feedback_id;              // 4 bytes  - Auto-increment
    int customer_id;              // 4 bytes  - Submitter
    char message[MAX_MESSAGE_LEN];// 256 bytes - Feedback text
    int status;                   // 4 bytes  - 0=PENDING, 1=REVIEWED
} __attribute__((packed)) Feedback; // Total: 268 bytes
```

---

## Session (sessions.dat)

```c
typedef struct {
    int user_id;                  // 4 bytes  - User ID (role-specific)
    int role;                     // 4 bytes  - 0=Customer, 1=Employee, 2=Manager, 3=Admin
    int active;                   // 4 bytes  - 1=valid, 0=logged out/expired
    time_t last_activity;         // 8 bytes  - Unix timestamp (updated per request)
} __attribute__((packed)) Session; // Total: 24 bytes
```

### Session Lifecycle
```
CREATE (login)
    │
    ▼
ACTIVE (active=1, last_activity=now)
    │
    ├─ Request received → last_activity = now
    │
    ├─ Explicit LOGOUT → active = 0
    │
    └─ Timeout (now - last_activity > 300s) → active = 0 (by reaper)
```

---

## Protocol Structs

### Request (524 bytes)
```c
typedef struct {
    int role;       // 4 bytes
    int user_id;    // 4 bytes
    int command;    // 4 bytes
    char payload[512]; // 512 bytes
} Request; // Total: 524 bytes
```

### Response (1540 bytes)
```c
typedef struct {
    int success;         // 4 bytes - 1=OK, 0=Error
    char message[512];   // 512 bytes - Human message
    char data[1024];     // 1024 bytes - Structured data
} Response; // Total: 1540 bytes
```

---

## Constants

```c
// Money
#define MICRO_UNITS       1000000LL    // 1 unit = 1,000,000 micro-units

// Records
#define RECORD_SIZE       512          // Fixed record size (all files)
#define MAX_NAME_LEN      50
#define MAX_PASSWORD_LEN  50
#define MAX_MESSAGE_LEN   256

// Limits
#define MAX_CLIENTS       200
#define MAX_SESSIONS      200
#define SESSION_TIMEOUT   300          // 5 minutes

// Roles
#define ROLE_CUSTOMER     0
#define ROLE_EMPLOYEE     1
#define ROLE_MANAGER      2
#define ROLE_ADMIN        3

// Account Status
#define ACC_STATUS_ACTIVE 0
#define ACC_STATUS_INACTIVE 1

// Loan Status
#define LOAN_PENDING      0
#define LOAN_APPROVED     1
#define LOAN_REJECTED     2

// Transaction Types
#define TXN_DEPOSIT       0
#define TXN_WITHDRAW      1
#define TXN_TRANSFER      2

// Lock Types
#define BANK_LOCK_READ    F_RDLCK
#define BANK_LOCK_WRITE   F_WRLCK
#define BANK_LOCK_UNLOCK  F_UNLCK
```

---

## File Record Mapping

| File | Record Index | Primary Key |
|------|--------------|-------------|
| admins.dat | `admin_id - 1` | admin_id |
| managers.dat | `manager_id - 1` | manager_id |
| employees.dat | `employee_id - 1` | employee_id |
| customers.dat | `user_id - 1` | user_id |
| loans.dat | `loan_id - 1` | loan_id |
| transactions.dat | `transaction_id - 1` | transaction_id |
| feedback.dat | `feedback_id - 1` | feedback_id |
| sessions.dat | 0..MAX_SESSIONS-1 | (user_id, role) composite |

---

## get_next_id() Logic

```c
int get_next_id(const char *filename, int record_size) {
    int fd = open(filename, O_RDONLY | O_CREAT, 0644);
    struct stat st;
    fstat(fd, &st);
    int count = st.st_size / RECORD_SIZE;  // RECORD_SIZE always 512
    close(fd);
    return count + 1;  // Next ID = record count + 1
}
```

- Works because: all files use fixed 512-byte records
- No gaps (deletion = status change, not record removal)
- Thread-safe? No (but fork-per-client = separate address spaces)

---

## Endianness & Portability

- **Little-endian** (x86_64 assumed)
- No network byte order conversion (same architecture client/server)
- For cross-platform: add `htonl`/`ntohl` for int fields, `htonll` for int64_t
- `time_t` is 8 bytes on 64-bit Linux (Unix timestamp)

---

## Verification

Compile-time size check:
```c
_Static_assert(sizeof(Customer) == 170, "Customer size");
_Static_assert(sizeof(Employee) == 362, "Employee size");
_Static_assert(sizeof(Admin) == 154, "Admin size");
_Static_assert(sizeof(Manager) == 154, "Manager size");
_Static_assert(sizeof(Transaction) == 52, "Transaction size");
_Static_assert(sizeof(Loan) == 20, "Loan size");
_Static_assert(sizeof(Feedback) == 268, "Feedback size");
_Static_assert(sizeof(Session) == 24, "Session size");
_Static_assert(sizeof(Request) == 524, "Request size");
_Static_assert(sizeof(Response) == 1540, "Response size");
```