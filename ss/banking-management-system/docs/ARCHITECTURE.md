# Architecture Deep-Dive

## System Overview

The Banking Management System follows a **fork-per-client concurrent server** architecture with **flat-file persistence** and **record-level advisory locking**. This design was chosen to satisfy the professor's requirements for:
- System calls (`open/read/write/lseek/fcntl/stat`)
- Process-based concurrency (`fork()`)
- ACID transactions without a database
- Explicit deadlock prevention

## Component Diagram

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                              CLIENT LAYER                                    │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐   │
│  │  Customer    │  │  Employee    │  │  Manager     │  │   Admin      │   │
│  │  (CLI/API)   │  │  (CLI/API)   │  │  (CLI/API)   │  │  (CLI/API)   │   │
│  └──────┬───────┘  └──────┬───────┘  └──────┬───────┘  └──────┬───────┘   │
└─────────┼──────────────────┼──────────────────┼──────────────────┼──────────┘
          │                  │                  │                  │
          └──────────────────┼──────────────────┼──────────────────┘
                             │ TCP Socket (Port 8081)
                             ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                           SERVER LAYER                                       │
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                        PARENT PROCESS                               │   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐  ┌───────────┐ │   │
│  │  │   Socket    │  │   Accept    │  │   Fork      │  │  Session  │ │   │
│  │  │   Setup     │──│   Loop      │──│   Children  │──│  Reaper   │ │   │
│  │  └─────────────┘  └─────────────┘  └─────────────┘  └───────────┘ │   │
│  │         │                                    ▲                      │   │
│  │         │ SIGCHLD=SIG_IGN                    │ (every 100 iters)   │   │
│  │         ▼                                    │                      │   │
│  │  ┌─────────────────────────────────────────────────────────────┐   │   │
│  │  │                    SIGNAL HANDLERS                          │   │   │
│  │  │  SIGINT/SIGTERM → cleanup_server() → close(server_fd)      │   │   │
│  │  └─────────────────────────────────────────────────────────────┘   │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                       CHILD PROCESS (per client)                    │   │
│  │  ┌─────────────────────────────────────────────────────────────┐   │   │
│  │  │                    handle_client(fd)                        │   │   │
│  │  │  ┌─────────┐  ┌─────────────┐  ┌─────────────────────────┐ │   │   │
│  │  │  │ read()  │─▶│  Dispatch   │─▶│ session_update_activity │ │   │   │
│  │  │  │ Request │  │  (switch)   │  │ (on every success)      │ │   │   │
│  │  │  └─────────┘  └──────┬──────┘  └─────────────────────────┘ │   │   │
│  │  │                      │                                    │   │   │
│  │  │         ┌────────────┼────────────┐                        │   │   │
│  │  │         ▼            ▼            ▼                        │   │   │
│  │  │   ┌─────────┐  ┌─────────┐  ┌─────────┐                   │   │   │
│  │  │   │Customer │  │Employee │  │Manager  │   ...             │   │   │
│  │  │   │  Ops    │  │  Ops    │  │  Ops    │                   │   │   │
│  │  │   └────┬────┘  └────┬────┘  └────┬────┘                   │   │   │
│  │  │        │            │            │                         │   │   │
│  │  │        └────────────┴────────────┘                         │   │   │
│  │  │                     │                                       │   │   │
│  │  │                     ▼                                       │   │   │
│  │  │  ┌─────────────────────────────────────┐                   │   │   │
│  │  │  │        file_ops.c Functions         │                   │   │   │
│  │  │  │  (lock → read → modify → write → unlock)               │   │   │
│  │  │  └─────────────────────────────────────┘                   │   │   │
│  │  │                     │                                       │   │   │
│  │  │                     ▼                                       │   │   │
│  │  │  ┌─────────────────────────────────────┐                   │   │   │
│  │  │  │      write() Response               │                   │   │   │
│  │  │  └─────────────────────────────────────┘                   │   │   │
│  │  └─────────────────────────────────────────────────────────────┘   │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────────────────┘
                             │
                             ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                           DATA LAYER (Flat Files)                           │
│                                                                             │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌─────────────┐         │
│  │ customers.  │ │ employees.  │ │  managers.  │ │  admins.    │         │
│  │    dat      │ │    dat      │ │    dat      │ │    dat      │         │
│  └─────────────┘ └─────────────┘ └─────────────┘ └─────────────┘         │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌─────────────┐         │
│  │  loans.     │ │transactions.│ │ feedback.   │ │ sessions.   │         │
│  │    dat      │ │    dat      │ │    dat      │ │    dat      │         │
│  └─────────────┘ └─────────────┘ └─────────────┘ └─────────────┘         │
│                                                                             │
│  All files: 512-byte fixed records, __attribute__((packed)) structs      │
└─────────────────────────────────────────────────────────────────────────────┘
```

## Data Flow: Customer Transfer (ACID Example)

```
Client A                                    Server (Child Process)
  │                                           │
  ├─ Request: TRANSFER, payload="4|50000000"─▶│
  │                                           │
  │                                    ┌──────▼──────┐
  │                                    │ lock_record │
  │                                    │ (min(1,4)=1) │
  │                                    └──────┬──────┘
  │                                           │
  │                                    ┌──────▼──────┐
  │                                    │ lock_record │
  │                                    │ (max(1,4)=4) │
  │                                    └──────┬──────┘
  │                                           │
  │                                    ┌──────▼──────┐
  │                                    │ read_record │
  │                                    │ (cust 1)    │
  │                                    └──────┬──────┘
  │                                           │
  │                                    ┌──────▼──────┐
  │                                    │ read_record │
  │                                    │ (cust 4)    │
  │                                    └──────┬──────┘
  │                                           │
  │                              ┌────────────▼────────────┐
  │                              │  Validate:              │
  │                              │  - from.balance >= amt  │
  │                              │  - both ACTIVE          │
  │                              │  - not self-transfer    │
  │                              └────────────┬────────────┘
  │                                           │
  │                                    ┌──────▼──────┐
  │                                    │ from.bal -= │
  │                                    │   amount    │
  │                                    │ to.bal +=   │
  │                                    │   amount    │
  │                                    └──────┬──────┘
  │                                           │
  │                                    ┌──────▼──────┐
  │                                    │ write_record│
  │                                    │ (cust 1)    │
  │                                    └──────┬──────┘
  │                                           │
  │                                    ┌──────▼──────┐
  │                                    │ write_record│
  │                                    │ (cust 4)    │
  │                                    └──────┬──────┘
  │                                           │
  │                                    ┌──────▼──────┐
  │                                    │ log_trans-  │
  │                                    │ action(x2)  │
  │                                    └──────┬──────┘
  │                                           │
  │                                    ┌──────▼──────┐
  │                                    │ unlock_rec  │
  │                                    │ (cust 4)    │
  │                                    └──────┬──────┘
  │                                           │
  │                                    ┌──────▼──────┐
  │                                    │ unlock_rec  │
  │                                    │ (cust 1)    │
  │                                    └──────┬──────┘
  │                                           │
  │                                    ┌──────▼──────┐
  │                                    │ session_upd │
  │                                    │ _activity() │
  │                                    └──────┬──────┘
  │                                           │
  │◀──────── Response: success=1 ─────────────┤
  │                                           │
```

## Process Model

### Parent Process
- **Single-threaded event loop**
- Owns listening socket
- Calls `accept()` → `fork()`
- Runs `session_reaper()` periodically (non-blocking)
- Installs signal handlers
- Ignores `SIGCHLD` (kernel reaps children automatically)

### Child Process
- **One per client connection**
- Inherits open file descriptors (data files)
- Runs `handle_client(client_fd)`
- **Stateless** - all state in data files
- Exits on client disconnect (`read() <= 0`)
- No shared memory with parent or siblings

### Why Fork-Per-Client?
| Factor | Fork | Threads | epoll/io_uring |
|--------|------|---------|----------------|
| Simplicity | ✅ | ❌ | ❌ |
| ACID reasoning | ✅ (isolated) | ❌ (shared state) | ❌ (complex) |
| Lecture requirement | ✅ | ❌ | Bonus only |
| 1M connections | ❌ | ❌ | ✅ (bonus) |

**Current choice satisfies core requirements.** Bonus scaling would refactor to epoll.

## File I/O Design

### Record Layout
```
File: customers.dat
Record 0 (512 bytes): Customer{id=1, ...} + zero padding
Record 1 (512 bytes): Customer{id=2, ...} + zero padding
...
```

### read_record()
```c
ssize_t read_record(int fd, int record_num, void *buf, size_t size) {
    off_t offset = record_num * RECORD_SIZE;
    lseek(fd, offset, SEEK_SET);
    return read(fd, buf, size);
}
```

### write_record() - Zero Padding
```c
ssize_t write_record(int fd, int record_num, const void *buf, size_t size) {
    off_t offset = record_num * RECORD_SIZE;
    lseek(fd, offset, SEEK_SET);
    char padded[RECORD_SIZE] = {0};
    memcpy(padded, buf, size < RECORD_SIZE ? size : RECORD_SIZE);
    return write(fd, padded, RECORD_SIZE);
}
```
- Ensures fixed 512-byte alignment
- Prevents reading garbage from previous larger records
- Enables `st.st_size / RECORD_SIZE` for record count

### Atomicity Guarantees
- Single record write = single `write()` syscall (atomic on POSIX)
- Multi-record operations (transfer) protected by locks
- `fsync()` not called explicitly (reliability vs performance tradeoff)

## Signal Handling

```c
void cleanup_server(int sig) {
    if (server_fd >= 0) close(server_fd);
    exit(0);
}

signal(SIGINT, cleanup_server);   // Ctrl+C
signal(SIGTERM, cleanup_server);  // kill command
signal(SIGCHLD, SIG_IGN);         // Auto-reap zombies
```

- `SIG_IGN` on `SIGCHLD` prevents zombie processes
- Parent doesn't need `waitpid()` loop
- Clean shutdown closes listening socket

## Scalability Considerations (Bonus)

### Current Limitations
- `fork()` per client → ~10K max (process table)
- `accept()` loop serializes new connections
- Session reaper scans all sessions linearly

### Path to 1M Connections
```
┌────────────────────────────────────────────────────────────┐
│                    EPOLL REFACTOR                          │
│                                                            │
│  Single Process                                            │
│  ┌────────────────────────────────────────────────────┐   │
│  │  epoll_wait() on listening socket + client sockets │   │
│  │         │                      │                   │   │
│  │         ▼                      ▼                   │   │
│  │   New Connection              Data Ready           │   │
│  │   accept() +                  read() +             │   │
│  │   epoll_ctl()                 dispatch()           │   │
│  └────────────────────────────────────────────────────┘   │
│                                                            │
│  Session Store: sharded hash map (not file scan)          │
│  Locking: per-record mutex + lock striping                │
└────────────────────────────────────────────────────────────┘
```

### io_uring (Linux 5.1+)
- Async `open/read/write/fsync` without blocking
- Batch syscalls via submission/completion queues
- Eliminates syscall overhead for high throughput

---

## Configuration Constants

| Constant | Value | Purpose |
|----------|-------|---------|
| `RECORD_SIZE` | 512 | Fixed record size (all files) |
| `MAX_SESSIONS` | 200 | Max concurrent sessions |
| `SESSION_TIMEOUT` | 300 | Session TTL (seconds) |
| `MAX_CLIENTS` | 200 | Listen backlog |
| `PORT` | 8080 | Default server port |
| `MICRO_UNITS` | 1,000,000 | Money precision |

---

## Error Handling Philosophy

| Layer | Strategy |
|-------|----------|
| System calls | Check return, `perror()`, return error code |
| Locking | Blocking `F_SETLKW` for ops, non-blocking `F_SETLK` for reaper |
| Protocol | Invalid format → error response (not disconnect) |
| Business logic | Validate before mutate, return -1/0 codes |
| Client disconnect | `read() <= 0` → child exits, session expires via timeout |

---

## Security Notes

| Aspect | Implementation |
|--------|----------------|
| Passwords | Plaintext in file (assignment scope) |
| Authentication | Single-factor (password only) |
| Session | In-memory timeout + file persistence |
| Network | No TLS (local assignment) |
| Input validation | `strtok`/`sscanf` with bounds |

*Production hardening would add: bcrypt, TLS, rate limiting, audit logs.*