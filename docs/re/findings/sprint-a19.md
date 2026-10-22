# Sprint a19 findings

## FA USB MSC and kernel layer

- `puh_ker_msg.c` (`0x80225490`), `puh_ker_sem.c` (`0x80225520`) and `puh_msc.c` (`0x80222FD0`) are source-built and Matching. `puh_msc_blk.c` (`0x80223630`) is split and NonMatching: nine of eleven functions match; `uhf_msc_blk_pread` and `uhf_msc_blk_pwrite` differ only in register numbering.
- Original unit boundaries come from `docs/re/findings/374-fa-sdi-usb.md`: message sending and the three semaphore routines are separate units.

## Traps

- **An `if/else` that sets a 0/1 flag from a `u8 > 0` test compiles to a branch-free `neg/or/srwi` sequence, but initialising the flag to 0 and then conditionally setting 1 keeps the branches.** `DrawDirectionalStrategy::DrawDirectional` rose from 96.81% to 97.53% this way.
- **Local declaration order moves loop-carried values between registers.** Randomly permuting the seven locals of `uhf_msc_blk_pread` reached 99.29% but not 100%; the target keeps the 16-bit chunk count in the highest saved register.
- **A background permutation script keeps running after its parent shell is stopped.** A second queued search rewrote `puh_msc_blk.c` and made the landing script reject the worktree; stop `a19_perm` processes explicitly.
