# opcp

Minimal bare-syscall file copy utility in C11 (`open`, `read`, `write`, `close`, `posix_fadvise`).

## Build & Usage

```bash
make
./opcp [-b size] <src> <dst>
make test
```

- Default buffer size is `4096` bytes (`4K`).
- `-b <size>` sets the buffer size (`B`, `K`, `M`, `G` suffixes).
- Handles `EINTR` retries, short writes, and evicts processed `16 MiB` chunks from the Page Cache via `POSIX_FADV_DONTNEED`.

## Benchmark Notes

Measured on a `64 MiB` random file (`perf stat`) and a `16 MiB` file (`strace -c`):

| Buffer (`-b`) | Time (64 MiB) | `read` + `write` Syscalls (16 MiB) |
| :---: | :---: | :---: |
| `64` | 0.993 s | 524,290 |
| `4K` | 0.051 s | 8,194 |
| `256K` | 0.023 s | 130 |
| `512K` | 0.025 s | 66 |
| `1M` | 0.029 s | 34 |

At small buffer sizes, runtime is dominated by Ring 3 ↔ Ring 0 syscall overhead (`~95%` kernel time at `64 B`). Beyond `256K`, syscall count is negligible and throughput plateaus as the working set reaches the per-core L2 cache (`1 MiB` on my machine).
