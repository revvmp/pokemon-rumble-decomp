# Issue 544 MetroTRK provenance

The message-handler and console implementations are adapted from `src/MetroTRK/debugger/embedded/MetroTRK/Portable/msghndlr.c` and `Export/mslsupp.c` in [SMGCommunity/Petari](https://github.com/SMGCommunity/Petari/tree/e0ab141914921726a595573dc48d3f13d0922efe), commit `e0ab141914921726a595573dc48d3f13d0922efe`. The message-buffer reconstruction coordinated with #543 uses `Portable/msgbuf.c` from that same commit. Protocol declarations derive from `libs/MetroTRK/include/trk.h` and the corresponding portable and PowerPC interfaces.

Petari's root license dedicates its code under CC0 1.0 Universal. The individual files inspected for this delivery contain no separate license notice or file-level exception. No proprietary executable bytes or analysis database are included. Comments are represented by evidence and explanations in the issue findings, and source is adapted to the target-proven layouts and canonical project headers.
