# Documentation Index

## Banking Management System - CS-513 System Software

This folder contains comprehensive documentation for the Banking Management System project.

## Document List

| File | Description | Audience |
|------|-------------|----------|
| [README.md](README.md) | Project overview, features, build/run, quick reference | All |
| [ARCHITECTURE.md](ARCHITECTURE.md) | System architecture, component diagram, data flow, process model | Developers, Reviewers |
| [API.md](API.md) | Protocol specification, command codes, payload formats, examples | Client developers |
| [DATA_STRUCTURES.md](DATA_STRUCTURES.md) | All struct definitions, sizes, field details, constants | Developers |
| [SESSIONS.md](SESSIONS.md) | Session lifecycle, timeout reaper, terminal close handling | Developers, Security |
| [LOCKING.md](LOCKING.md) | Concurrency model, fcntl locking, deadlock prevention, ACID | Developers, Reviewers |
| [TESTING.md](TESTING.md) | Test suite documentation, running tests, debugging, CI | QA, Developers |

## Quick Navigation

### For Professor Evaluation
- **Start with:** [README.md](README.md) → "Professor Evaluation Criteria" section
- **Architecture:** [ARCHITECTURE.md](ARCHITECTURE.md) → Component diagram, data flow
- **Concurrency:** [LOCKING.md](LOCKING.md) → Deadlock prevention, ACID proof
- **Sessions:** [SESSIONS.md](SESSIONS.md) → Terminal close handling
- **Tests:** [TESTING.md](TESTING.md) → 77 tests covering all criteria

### For Development
- **Build/Run:** [README.md](README.md) → "Build & Run" section
- **Protocol:** [API.md](API.md) → Command codes, payload formats
- **Data:** [DATA_STRUCTURES.md](DATA_STRUCTURES.md) → Struct layouts, sizes
- **Debugging:** [TESTING.md](TESTING.md) → Common failures, debug mode

### For Client Integration
- **Protocol:** [API.md](API.md) → Complete wire format, Python example
- **Auth:** [API.md](API.md) → Login flow, session management

## Key Technical Decisions

| Decision | Document | Rationale |
|----------|----------|-----------|
| Fork-per-client | [ARCHITECTURE.md](ARCHITECTURE.md) | Lecture requirement, simpler ACID reasoning |
| 512-byte fixed records | [DATA_STRUCTURES.md](DATA_STRUCTURES.md) | Deterministic layout, simple `lseek` |
| `fcntl` record locks | [LOCKING.md](LOCKING.md) | POSIX advisory, joint account safe |
| Ordered locking | [LOCKING.md](LOCKING.md) | Global order by ID → no deadlock |
| Session timeout reaper | [SESSIONS.md](SESSIONS.md) | Handles terminal close without heartbeat |
| Single session enforcement | [SESSIONS.md](SESSIONS.md) | Hard requirement, enforced at login |
| Micro-unit money | [DATA_STRUCTURES.md](DATA_STRUCTURES.md) | No floating-point errors |

## Lecture Compliance Checklist

| Requirement | Status | Document |
|-------------|--------|----------|
| Blueprint/Class Diagram | ✅ | [README.md](README.md) |
| Working code per module | ✅ | [README.md](README.md) |
| Concurrency, sync, ACID | ✅ | [LOCKING.md](LOCKING.md) |
| Record-level `fcntl` locking | ✅ | [LOCKING.md](LOCKING.md) |
| Deadlock prevention | ✅ | [LOCKING.md](LOCKING.md) |
| Single session constraint | ✅ | [SESSIONS.md](SESSIONS.md) |
| Loan workflow | ✅ | [TESTING.md](TESTING.md) Group 5 |
| Account activation/deactivation | ✅ | [TESTING.md](TESTING.md) Group 6 |
| Terminal close session invalidation | ✅ | [SESSIONS.md](SESSIONS.md) |
| Scaling bonus (epoll/io_uring) | 📝 Design | [ARCHITECTURE.md](ARCHITECTURE.md) |

## Viva Questions Answered

| Question | Answer Location |
|----------|-----------------|
| How is deadlock prevented? | [LOCKING.md](LOCKING.md) - Global lock ordering |
| How is ACID achieved in transfer? | [LOCKING.md](LOCKING.md) - Atomic under dual locks |
| Session invalidation on terminal close? | [SESSIONS.md](SESSIONS.md) - Timeout reaper |
| Binary file structure? | [DATA_STRUCTURES.md](DATA_STRUCTURES.md) - 512-byte packed records |
| Joint account safety? | [LOCKING.md](LOCKING.md) - Record-level locks |
| Scaling to 1M connections? | [ARCHITECTURE.md](ARCHITECTURE.md) - epoll/io_uring path |

## File Structure

```
/home/ubuntu/banking-management-system/
├── docs/
│   ├── README.md           # This file
│   ├── ARCHITECTURE.md
│   ├── API.md
│   ├── DATA_STRUCTURES.md
│   ├── SESSIONS.md
│   ├── LOCKING.md
│   └── TESTING.md
├── include/
│   └── bank.h
├── src/
│   ├── file_ops.c
│   ├── server.c
│   └── client.c
├── tests/
│   ├── test_final.py
│   ├── test_basic.sh
│   ├── test_comprehensive.sh
│   └── test_protocol.py
├── data/                   # Runtime (created by init_data_files)
└── Makefile
```

## Version
Documentation generated: 2026-10-26  
Project: CS-513 System Software Mini Project  
Institution: IIIT Bangalore