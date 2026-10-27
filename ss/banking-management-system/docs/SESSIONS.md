# Session Management

## Overview

The Banking Management System implements **single-session-per-user** with **timeout-based invalidation** to handle terminal closes, crashes, and network partitions.

## Design Goals

| Requirement | Solution |
|-------------|----------|
| Single session per user | Enforced at login via session file scan |
| Terminal close handling | Timeout reaper (5 min default) |
| Crash recovery | Session persists to disk, expires on timeout |
| No shared memory | All session state in `data/sessions.dat` |
| Fork-per-client compatible | File-based, no in-memory state |

## Session Record

```c
typedef struct {
    int user_id;              // 4 bytes
    int role;                 // 4 bytes (0=Customer, 1=Employee, 2=Manager, 3=Admin)
    int active;               // 4 bytes (1=valid, 0=expired/logged out)
    time_t last_activity;     // 8 bytes (Unix timestamp)
} __attribute__((packed)) Session;  // 24 bytes → padded to 512
```

## Session File

**File:** `data/sessions.dat`  
**Capacity:** 200 sessions (MAX_SESSIONS)  
**Record Size:** 512 bytes (fixed)  
**Index:** Linear scan (no hash - small N)

## API

### session_create(user_id, role)
```c
int session_create(int user_id, int role);
```
**Called by:** All `*_login()` functions on success  
**Behavior:**
1. Lock entire sessions file (`BANK_LOCK_WRITE` on record 0)
2. Read all existing sessions
3. Check for existing active session for same (user_id, role) → fail if found
4. Find empty slot (active=0) or append
5. Write new session: `{user_id, role, active=1, last_activity=time(NULL)}`
6. Unlock, return 0 on success, -1 on failure

### session_destroy(user_id, role)
```c
int session_destroy(int user_id, int role);
```
**Called by:** `CMD_LOGOUT`, reaper on timeout  
**Behavior:**
1. Lock sessions file
2. Find matching active session
3. Set `active = 0`
4. Write back, unlock, return 0 on success

### session_is_active(user_id, role)
```c
int session_is_active(int user_id, int role);
```
**Called by:** Every command dispatch (before processing)  
**Behavior:**
1. Lock sessions file (`BANK_LOCK_READ`)
2. Find matching session
3. **Timeout check:** `if (now - last_activity > SESSION_TIMEOUT) { active=0; write; return 0; }`
4. Return 1 if active and not expired, else 0

### session_update_activity(user_id, role)
```c
int session_update_activity(int user_id, int role);
```
**Called by:** Server on every successful command  
**Behavior:**
1. Lock sessions file (`BANK_LOCK_WRITE`)
2. Find matching active session
3. Update `last_activity = time(NULL)`
5. Write back, unlock, return 0

### session_reaper()
```c
int session_reaper(void);
```
**Called by:** Parent process every 100 accept() iterations  
**Behavior:**
1. **Non-blocking lock** (`try_lock_record` with `F_SETLK`)
2. If lock fails → another process has it → return 0 (skip cycle)
3. Scan all sessions
4. For each active session: `if (now - last_activity > SESSION_TIMEOUT) active = 0`
5. Write modified sessions, unlock, return count expired

## Flow Diagrams

### Login Flow
```
Client sends LOGIN
    │
    ▼
*_login() verifies credentials
    │
    ├─ Fail → return 0
    │
    └─ Success → session_create(user_id, role)
                    │
                    ├─ Already active? → return 0 (single session enforced)
                    │
                    └─ Create session → return 1
```

### Command Processing Flow
```
Server receives Request
    │
    ▼
session_is_active(req.user_id, req.role)
    │
    ├─ Returns 0 → "Session expired" / "Not logged in"
    │
    └─ Returns 1
         │
         ▼
    Process command (switch on req.command)
         │
         ├─ Success → session_update_activity(req.user_id, req.role)
         │
         └─ Failure → no activity update
```

### Terminal Close / Crash Handling
```
User closes terminal / process crashes
    │
    ▼
Kernel sends EOF on socket
    │
    ▼
Child's read() returns 0
    │
    ▼
handle_client() loop breaks → child exits
    │
    ▼
Session remains in sessions.dat with last_activity timestamp
    │
    ▼
[Time passes...]
    │
    ├─ Next command from same user → session_is_active() detects timeout → destroys
    │
    └─ Parent's session_reaper() runs → destroys expired sessions
         │
         ▼
    User can login again (new session created)
```

## Locking Strategy

### Session File Locking
```c
// Single lock for entire sessions file (record 0)
lock_record(fd, 0, BANK_LOCK_WRITE);   // Blocking for create/destroy/update
lock_record(fd, 0, BANK_LOCK_READ);    // Blocking for is_active
try_lock_record(fd, 0, BANK_LOCK_WRITE); // Non-blocking for reaper
```

### Why Record 0?
- Simple: one lock for entire session table
- Low contention: sessions file small (200 × 512 = 100KB)
- Reaper uses non-blocking to avoid blocking children

### Lock Contention Scenario
```
Timeline:
─────────────────────────────────────────────────────────────▶
Child A: lock(WRITE) ────── create ───── unlock
                          │
Child B: lock(READ) ─────── wait ─────── is_active ───── unlock
                          │
Reaper: try_lock(WRITE) ── fail ─────── skip (returns 0)
                          │
Child A: lock(WRITE) ────── update ───── unlock
                          │
Reaper: try_lock(WRITE) ── success ──── scan/expire ──── unlock
```

## Timeout Configuration

```c
#define SESSION_TIMEOUT 300  // 5 minutes (in seconds)
```

**Tunable for:**
- Development: `30` (fast feedback)
- Production: `300` (balance security/usability)
- High-security: `60` (shorter window)

## Edge Cases Handled

| Scenario | Behavior |
|----------|----------|
| User logs in, closes terminal immediately | Session expires after 5 min, then can re-login |
| Server restarts | `init_data_files()` truncates `sessions.dat` → clean slate |
| Two login attempts same time | First wins (file lock serializes), second gets "Already logged in" |
| Reaper runs during login | Reaper uses non-blocking lock → skips if busy |
| Clock skew | Uses `time(NULL)` (system clock), monotonic not needed for 5-min timeout |
| Session file corruption | `read_record` validates size, returns error → treated as inactive |

## Performance

| Metric | Value |
|--------|-------|
| Max sessions | 200 |
| Session file size | 100 KB (200 × 512) |
| Lock contention | Negligible (short critical sections) |
| Reaper frequency | Every 100 accepts (~100 clients) |
| Scan time | < 1ms (linear scan of 200 records) |

## Testing Session Behavior

```bash
# 1. Test single session enforcement
./bin/bank_client 127.0.0.1 8081  # Login as admin
# In another terminal:
./bin/bank_client 127.0.0.1 8081  # Login as admin → fails

# 2. Test timeout
./bin/bank_client 127.0.0.1 8081  # Login
# Wait 5+ minutes
./bin/bank_client 127.0.0.1 8081  # Login again → succeeds (old expired)

# 3. Test explicit logout
./bin/bank_client 127.0.0.1 8081  # Login
# Choose logout option
./bin/bank_client 127.0.0.1 8081  # Login again → succeeds immediately
```

## Integration with Protocol

Server command dispatch includes session validation:

```c
// In handle_client() - before processing any command except LOGIN
if (req.command != CMD_LOGIN) {
    if (!session_is_active(req.user_id, req.role)) {
        resp.success = 0;
        strcpy(resp.message, "Session expired or invalid");
        write(client_fd, &resp, sizeof(Response));
        continue;
    }
    // ... process command ...
    session_update_activity(req.user_id, req.role);
}
```

**Note:** `CMD_LOGIN` bypasses session check (creates new session).

## Future Improvements

| Feature | Approach |
|---------|----------|
| Heartbeat protocol | Client sends periodic ping → server updates activity |
| Configurable timeout | Per-role or per-user timeout in config file |
| Session hijacking protection | Bind session to client IP/port |
| Distributed sessions | Redis/shared DB for multi-server deployment |
| Audit logging | Log all session create/destroy/expire events |