# Issue #543 MetroTRK source provenance

The initialization, packet polling, string, file-support, target-continuation, processor-state assembly, message-send, and message-buffer sources are adapted from `SMGCommunity/Petari` commit `e0ab141914921726a595573dc48d3f13d0922efe`, under `src/MetroTRK/debugger/embedded/MetroTRK/` and `libs/MetroTRK/include/`. Petari's root license is CC0 1.0 Universal. The inspected reference files carry no additional file-specific copyright or license notice.

Reference comments are not retained in source. Target-specific initialization, protocol, diagnostic, register-bank, and storage differences are independently established from the frozen `wpse-v1` executable and recorded in [F-543-1](../findings/543-metrotrk-initialization-support.md). The complete message-buffer implementation is coordinated with issue #544 and uses the same pinned reference.
