# Testing Documentation

## Test Suite Overview

| Test File | Type | Tests | Purpose |
|-----------|------|-------|---------|
| `tests/test_final.py` | Protocol (Python) | 77 | Primary test suite, direct socket protocol |
| `tests/test_basic.sh` | Shell (CLI) | ~50 | Lecture-mentioned cases |
| `tests/test_comprehensive.sh` | Shell (CLI) | ~40 | Hidden edge cases |
| `tests/test_protocol.py` | Protocol (Python) | 77 | Earlier protocol test version |

**Primary:** `test_final.py` - most reliable, bypasses interactive CLI issues.

## Running Tests

```bash
cd /home/ubuntu/banking-management-system

# Clean build (critical - resets data files)
make clean && make

# Start server (background)
./bin/bank_server 8081 &

# Run primary test suite
python3 tests/test_final.py

# Expected output:
# ================================
# SUMMARY: Passed=77  Failed=0  Total=77
# ================================
```

### Server Lifecycle for Tests

```bash
# Kill any existing server
pkill -9 -f "bank_server"

# Start fresh (after make clean)
./bin/bank_server 8081 > /tmp/server.log 2>&1 &

# Wait for startup
sleep 2

# Run tests
python3 tests/test_final.py

# Cleanup
pkill -9 -f "bank_server"
```

## Test Groups (test_final.py)

### GROUP 1: Authentication & Session Management (9 tests)
| Test | Description |
|------|-------------|
| Admin login | Valid credentials succeed |
| Admin logout | Explicit logout destroys session |
| Wrong password rejected | Invalid password fails |
| Non-existent user rejected | Unknown ID fails |
| Single session enforced | Second login same user fails |
| Employee login | Created employee can login |
| Customer login | Created customer can login |
| Manager login | Default manager can login |
| Invalid role rejected | Role 5 (non-existent) fails |

### GROUP 2: Admin Operations (6 tests)
| Test | Description |
|------|-------------|
| Add employee | Creates new employee, returns ID |
| Duplicate name creates new | Same name creates different ID |
| Modify employee | Updates name/password |
| Admin change password | Valid old→new succeeds |
| Admin change with wrong old fails | Invalid old password rejected |

### GROUP 3: Employee Operations (10 tests)
| Test | Description |
|------|-------------|
| Add customer | Creates new customer, returns ID |
| Duplicate name creates new | Same name creates different ID |
| Passbook | View customer transaction history |
| View pending loans | List unassigned loans |
| Employee modify customer | Update customer details |
| Modify non-existent fails | Invalid customer ID rejected |
| Employee delete customer | Set account_status=INACTIVE |
| Employee change password | Valid old→new succeeds |
| Cannot process unassigned loan | Loan not assigned to employee fails |
| Cannot reprocess loan | Already approved/rejected fails |

### GROUP 4: Customer Core Banking (18 tests)
| Test | Description |
|------|-------------|
| Deposit | Valid amount increases balance |
| Balance check | Returns correct balance |
| Withdraw | Valid amount decreases balance |
| Overdraw rejected | Insufficient balance fails |
| Negative deposit rejected | Negative amount fails |
| Zero deposit rejected | Zero amount fails |
| Negative withdraw rejected | Negative amount fails |
| Zero withdraw rejected | Zero amount fails |
| Transfer to valid account | Transfers to existing customer |
| Transfer to non-existent rejected | Invalid target ID fails |
| Self transfer rejected | Same from/to ID fails |
| Transfer insufficient funds rejected | Over balance fails |
| Negative transfer rejected | Negative amount fails |
| Change password | Valid old→new succeeds |
| Customer change with wrong old fails | Invalid old password fails |
| Transaction history | Returns formatted transactions |

### GROUP 5: Loan Workflow (13 tests) - **Lecture: "Most Complex"**
| Test | Description |
|------|-------------|
| Loan application submitted | Creates PENDING loan |
| Second loan while pending rejected | Cannot apply twice |
| Loan assigned | Manager assigns to active employee |
| Invalid loan ID rejected | Non-existent loan fails |
| Invalid employee ID rejected | Non-existent employee fails |
| Assign to inactive employee fails | Status=INACTIVE rejected |
| Already assigned loan rejected | Double-assign fails |
| Loan approved | Employee approves → balance credited |
| Loan amount credited to balance | Verify balance increased by loan amount |
| New loan after approval | Can re-apply after approval |
| Second assign failed | Cannot reassign approved loan |
| Loan rejected | Employee rejects → no credit |
| View loans | Lists loans with status |

### GROUP 6: Account Deactivation (9 tests)
| Test | Description |
|------|-------------|
| Account deactivated | Manager toggles to INACTIVE |
| Deactivated rejects deposit | Deposit fails on INACTIVE |
| Deactivated rejects withdraw | Withdraw fails on INACTIVE |
| Deactivated rejects transfer out | Transfer fails on INACTIVE |
| Deactivated rejects loan apply | Loan apply fails on INACTIVE |
| Deactivated rejects password change | Change password fails on INACTIVE |
| Account reactivated | Manager toggles back to ACTIVE |
| Reactivated accepts deposit | Deposit works after reactivate |
| Transfer to inactive rejected | Cannot transfer TO inactive account |

### GROUP 7: Feedback System (3 tests)
| Test | Description |
|------|-------------|
| Feedback submitted | Customer submits feedback |
| Feedback visible to manager | Manager sees pending feedback |
| Multiple feedback visible | Multiple customers' feedback visible |

### GROUP 8: Edge Cases (7 tests)
| Test | Description |
|------|-------------|
| Large amount deposit | Near INT64_MAX micro-units |
| Large transfer | Near INT64_MAX (should fail gracefully) |
| Zero loan amount rejected | Zero amount rejected |
| Negative loan amount rejected | Negative amount rejected |
| Max password length | 50 chars accepted |
| Special chars in password | `p@ss!w0rd#` accepted |
| Max name length | 50 chars accepted |

### GROUP 9: ACID & Concurrency (3 tests)
| Test | Description |
|------|-------------|
| Money conserved after 20 bidirectional transfers | 10× A→B(100) + 10× B→A(100) = conserved |
| 50 rapid sequential deposits | Final balance = initial + 50 |
| Transfer chain A→B→C→A | Circular transfer conserves money |

### GROUP 10: Concurrency & Stability (2 tests)
| Test | Description |
|------|-------------|
| Stress | Multiple concurrent admin logins |
| Concurrent | 10 parallel admin logins |

## Test Data Setup

Tests create their own data via protocol:
```python
# In test_final.py run_tests():
c.login(ROLE_ADMIN, 1, "admin123")
c.adm_add_employee("Test", "Employee", "emp123")
c.logout()

c.login(ROLE_EMPLOYEE, 1, "emp123")
c.emp_add_customer("Test", "Customer", "cust123")
c.logout()
```

**Why `make clean`?** Ensures IDs are deterministic (employee=1, customer=1).

## Shell Test Scripts

### test_basic.sh
Lecture-mentioned cases, ~500 lines. Uses piped CLI input.

```bash
# Run (requires server running)
./tests/test_basic.sh
```

**Known issue:** Interactive CLI with piped stdin has timing issues. Python tests preferred.

### test_comprehensive.sh
Hidden edge cases beyond documentation, 499 lines.

```bash
./tests/test_comprehensive.sh
```

## Debugging Test Failures

### Common Failure Patterns

| Symptom | Cause | Fix |
|---------|-------|-----|
| "Invalid credentials" on first login | Stale data files | `make clean && make` |
| Employee/Customer login fails | Wrong ID assumption | Check test expects ID=1 |
| "Transfer failed" on valid transfer | Insufficient balance | Check test setup deposited enough |
| Loan assign fails | Employee not active or loan not pending | Verify test sequence |
| Session tests fail | Server not restarted | `pkill -9 -f bank_server` + restart |

### Debug Mode
```python
# In test_final.py, add to BankClient.send_request():
print(f"SEND: role={self.role}, uid={self.user_id}, cmd={cmd}, payload={payload[:50]}")
print(f"RECV: success={success}, msg={msg}, data={data[:50]}")
```

### Server Log
```bash
./bin/bank_server 8081 2>&1 | tee server.log
# Watch for: "Bank server listening on port 8081"
# Each fork prints this (expected)
```

## Continuous Integration

```yaml
# .github/workflows/test.yml (example)
name: Tests
on: [push, pull_request]
jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - name: Build
        run: make clean && make
      - name: Start server
        run: |
          ./bin/bank_server 8081 &
          sleep 2
      - name: Run tests
        run: python3 tests/test_final.py
      - name: Cleanup
        run: pkill -9 -f bank_server
```

## Performance Benchmarks

```python
# Add to test_final.py for manual benchmarking
import time

def benchmark_transfers(num=1000):
    start = time.time()
    for i in range(num):
        c.transfer(2, 1000000)
        c.transfer(1, 1000000)
    elapsed = time.time() - start
    print(f"{num*2} transfers in {elapsed:.2f}s = {num*2/elapsed:.0f} TPS")

# Typical result: ~500-1000 TPS on localhost (fork overhead dominates)
```

## Test Coverage Matrix

| Feature | test_final.py | test_basic.sh | test_comprehensive.sh |
|---------|---------------|---------------|----------------------|
| Auth/Login | ✅ | ✅ | ✅ |
| Session mgmt | ✅ | ✅ | ✅ |
| Customer ops | ✅ | ✅ | ✅ |
| Employee ops | ✅ | ✅ | ✅ |
| Manager ops | ✅ | ✅ | ✅ |
| Admin ops | ✅ | ✅ | ✅ |
| Loan workflow | ✅ | ✅ | ✅ |
| Deactivation | ✅ | ✅ | ✅ |
| Feedback | ✅ | ✅ | ✅ |
| Edge cases | ✅ | ❌ | ✅ |
| ACID/concurrency | ✅ | ❌ | ❌ |
| Large amounts | ✅ | ❌ | ✅ |
| Special chars | ✅ | ❌ | ✅ |

## Adding New Tests

```python
# In test_final.py run_tests():
def ok(msg): ...
def xk(msg): ...
def assert_resp(success, msg, data, pattern, name): ...
def assert_fail(success, msg, data, name): ...

# Add test:
c.login(ROLE_CUSTOMER, 1, "cust123")
success, msg, data = c.deposit(999999999999)
assert_resp(success, msg, data, "Deposit successful", "Huge deposit")
```

## CI Checklist

- [ ] `make clean && make` passes
- [ ] Server starts without errors
- [ ] `python3 tests/test_final.py` → 77/77 pass
- [ ] No memory leaks (run with `valgrind` if needed)
- [ ] Server shuts down cleanly on SIGINT