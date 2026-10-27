# Locking & Concurrency

## Overview

The system uses **POSIX `fcntl` advisory record locks** on fixed 512-byte records to achieve:
- **Isolation** between concurrent operations
- **ACID** compliance for transfers
- **Deadlock prevention** via global lock ordering
- **Joint account safety** (multiple users on same record)

## Lock Types

```c
#define BANK_LOCK_READ  F_RDLCK   // Shared lock (multiple readers)
#define BANK_LOCK_WRITE F_WRLCK   // Exclusive lock (single writer)
#define BANK_LOCK_UNLOCK F_UNLCK   // Release lock
```

## Lock Functions

### Blocking Lock (Operations)
```c
int lock_record(int fd, int record_num, short lock_type) {
    struct flock fl = {
        .l_type   = lock_type,
        .l_whence = SEEK_SET,
        .l_start  = record_num * RECORD_SIZE,
        .l_len    = RECORD_SIZE,
        .l_pid    = getpid()
    };
    return fcntl(fd, F_SETLKW, &fl);  // Blocks until granted
}
```

### Non-Blocking Lock (Reaper)
```c
int try_lock_record(int fd, int record_num, short lock_type) {
    struct flock fl = { ...same... };
    return fcntl(fd, F_SETLK, &fl);   // Returns -1 if busy (EAGAIN/EACCES)
}
```

### Unlock
```c
int unlock_record(int fd, int record_num) {
    return lock_record(fd, record_num, F_UNLCK);
}
```

## Record-Level Granularity

Each data file uses **fixed 512-byte records**. Lock scope = one record.

```
File: customers.dat
┌─────────────────────────────────────────────────────────────┐
│ Record 0 (bytes 0-511)   │ Customer ID 1                    │
│ Record 1 (bytes 512-1023) │ Customer ID 2                   │
│ Record 2 (bytes 1024-1535)│ Customer ID 3                   │
│ ...                          ...                              │
└─────────────────────────────────────────────────────────────┘
Lock on Record 1 → bytes 512-1023 (Customer ID 2)
```

**Why whole record?** Simplicity - struct fits in 512 bytes, no partial overlap concerns.

## Locking Patterns by Operation

| Operation | Records Locked | Lock Type | Order |
|-----------|----------------|-----------|-------|
| Balance/History | 1 (self) | READ | - |
| Deposit/Withdraw/Password | 1 (self) | WRITE | - |
| Transfer | 2 (from, to) | WRITE | **min ID first** |
| Loan Apply | 1 (customer) + 1 (loan) | WRITE | Customer first |
| Loan Approve | 1 (loan) + 1 (customer) + 1 (employee) | WRITE | Loan → Customer → Employee |
| Loan Assign | 1 (loan) + 1 (employee) | WRITE | Loan → Employee |

## Deadlock Prevention: Global Lock Ordering

### The Problem
```
Transfer 1→2:         Transfer 2→1:
  lock(1)                lock(2)
  lock(2)  ← BLOCKED     lock(1)  ← BLOCKED
DEADLOCK!
```

### The Solution: Always Lock Lower ID First
```c
int customer_transfer(int from_id, int to_id, int64_t amount) {
    int first = (from_id < to_id) ? from_id : to_id;
    int second = (from_id < to_id) ? to_id : from_id;
    
    lock_record(fd, first - 1, BANK_LOCK_WRITE);
    lock_record(fd, second - 1, BANK_LOCK_WRITE);
    // ... atomic transfer ...
    unlock_record(fd, second - 1);
    unlock_record(fd, first - 1);
}
```

**Proof:** All threads acquire locks in monotonically increasing order → no circular wait → no deadlock.

## Concurrency Scenarios

### Scenario 1: Concurrent Deposits to Same Account
```
Client A: deposit(100)          Client B: deposit(200)
    │                                │
    ├─ lock(WRITE, cust_1)          ├─ lock(WRITE, cust_1) ← BLOCKS
    ├─ read(bal=500)                │
    ├─ bal=600                      │
    ├─ write(600)                   │
    ├─ unlock(cust_1) ──────────────▶│ lock granted
    │                                ├─ read(bal=600)
    │                                ├─ bal=800
    │                                ├─ write(800)
    │                                ├─ unlock(cust_1)
    ▼                                ▼
Result: 800 (correct)
```

### Scenario 2: Concurrent Transfers (A→B and C→D)
```
Transfer 1→4:    lock(1) → lock(4) → transfer → unlock(4) → unlock(1)
Transfer 2→3:    lock(2) → lock(3) → transfer → unlock(3) → unlock(2)
```
No conflict (disjoint record sets) → **full parallelism**

### Scenario 3: Concurrent Transfers (A→B and B→A)
```
Transfer 1→2:    lock(1) → lock(2) → transfer
Transfer 2→1:    lock(1) ← BLOCKS (1 already locked)
```
**Ordered locking prevents deadlock** - second transfer waits for first to complete.

### Scenario 4: Joint Account (Same Customer ID)
```
Customer 1 (joint owner) and Customer 2 both operate on Account 5
```
- Both operations lock Record 4 (customer_id - 1 = 4)
- `fcntl` serializes them → **correct isolation**

## Loan Workflow Locking

### Apply (Customer)
```
lock(WRITE, customer)
  read customer
  validate (no pending loan)
  update customer.loan_status = PENDING
  write customer
unlock(customer)

lock(WRITE, loan)  // new loan record
  write loan (PENDING, unassigned)
unlock(loan)
```

### Assign (Manager)
```
lock(WRITE, loan)
  read loan
  validate (PENDING, unassigned)
  
  lock(WRITE, employee)
    read employee
    validate (ACTIVE)
    loan.assigned_employee_id = emp_id
    write loan
    employee.assigned_loans[loan_count++] = loan_id
    write employee
  unlock(employee)
unlock(loan)
```
**Order: Loan → Employee** (consistent with approve)

### Approve (Employee)
```
lock(WRITE, loan)
  read loan (validate PENDING, assigned to me)
  
  lock(WRITE, customer)
    read customer
    customer.balance += loan.amount
    customer.loan_status = APPROVED
    customer.loan_amount = 0
    write customer
    log_transaction(customer, DEPOSIT, loan.amount)
  unlock(customer)
  
  loan.status = APPROVED
  write loan
unlock(loan)
```
**Order: Loan → Customer** (consistent with assign)

### Reject (Employee)
```
lock(WRITE, loan)
  read loan (validate PENDING, assigned to me)
  
  lock(WRITE, customer)
    read customer
    customer.loan_status = REJECTED
    customer.loan_amount = 0
    write customer
  unlock(customer)
  
  loan.status = REJECTED
  write loan
unlock(loan)
```

## Lock Contention Analysis

| Scenario | Contention | Mitigation |
|----------|------------|------------|
| Hot account (many deposits) | High | Short critical section (read-modify-write) |
| Transfer storm | Medium | Ordered locking prevents deadlock |
| Loan approve + assign same loan | Low | Sequential by design |
| Reaper vs operations | Very low | Reaper uses non-blocking lock |

## Correctness Guarantees

| Property | Mechanism |
|----------|-----------|
| **Atomicity** | Multi-record ops under combined locks |
| **Consistency** | Validation before write, constraints enforced |
| **Isolation** | `fcntl` locks prevent dirty reads/writes |
| **Durability** | `write()` to disk (fsync not called - assignment scope) |
| **No Deadlock** | Global lock ordering by record number |
| **No Starvation** | `F_SETLKW` queues waiters FIFO (kernel) |

## Limitations & Assumptions

| Limitation | Impact |
|------------|--------|
| Advisory locks only | Requires all processes to cooperate (true here) |
| No `fsync()` | Power loss may lose last write |
| Single lock file for sessions | Contention at very high scale |
| No lock timeout | `F_SETLKW` blocks indefinitely |
| Fork inherits locks | Child gets parent's locks (not an issue - child creates own) |

## Testing Lock Correctness

```bash
# Money conservation test (20 bidirectional transfers)
# In test_final.py GROUP 9:
#   Creates 2 accounts with 10000 each
#   Runs 20 threads: 10× A→B(100), 10× B→A(100)
#   Verifies: A.balance + B.balance == 20000 (always)

# Concurrent deposit test (50 rapid sequential)
#   Single account, 50× deposit(1)
#   Final balance = initial + 50

# Transfer chain test
#   A→B→C→A with 100 each
#   Verifies all balances correct, money conserved
```

## Debugging Lock Issues

```c
// Add to lock_record for debugging
printf("[PID %d] lock %s record %d\n", getpid(), 
       lock_type == F_RDLCK ? "READ" : "WRITE", record_num);

// Check for stuck processes
lsof -p <pid>  # Shows open files
cat /proc/locks  # Shows all file locks system-wide
```

## Alternative: Lock-Free (Not Used)

| Approach | Why Not Used |
|----------|--------------|
| Optimistic locking (version numbers) | Requires CAS, complex in C |
| Lock striping | Overkill for 200 records |
| pthread mutexes | Requires threads, not fork |
| Database | Prohibited (flat files only) |

## Future: Scaling Locks

For 1M+ connections (bonus):
```
┌─────────────────────────────────────────────────────────────┐
│  Sharded Lock Manager                                        │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐       │
│  │ Shard 0  │ │ Shard 1  │ │ Shard 2  │ │ Shard 3  │       │
│  │ Records  │ │ Records  │ │ Records  │ │ Records  │       │
│  │ 0-49     │ │ 50-99    │ │ 100-149  │ │ 150-199  │       │
│  │ Mutex    │ │ Mutex    │ │ Mutex    │ │ Mutex    │       │
│  └──────────┘ └──────────┘ └──────────┘ └──────────┘       │
└─────────────────────────────────────────────────────────────┘
```
- Hash record_num → shard → per-shard mutex
- Reduces contention by factor of N_shards
- Compatible with epoll/io_uring single-threaded model