# F-497-1: IPC allocator and request profiling

Issue #497 owns `0x802F1320–0x802F342C`. This contribution reconstructs the final two translation units; the IPC client at `0x802F1320–0x802F2D10` remains original fallback code.

## Evidence and boundaries

The target allocator uses eight 16-byte heap records at `0x804772A0–0x80477320`. Each record contains base, owner, size, and free-list pointer. Chunk headers are 16 bytes: status, size, previous free chunk, and next free chunk. The allocation status constants are `0xBABE0000`, `0xBABE0001`, and `0xBABE0002`. Heap creation validates 32-byte alignment; the unaligned-allocation wrapper at `0x802F3040` supplies alignment 32 before tail-calling the allocator.

`memory.c` owns `.text 0x802F2D10–0x802F3248` and `.bss 0x804772A0–0x80477320`. Its five functions are `iosCreateHeap`, `__iosAlloc`, `iosAlloc`, `iosAllocAligned`, and `iosFree`.

`ipcProfile.c` owns `.text 0x802F3250–0x802F342C`, `.bss 0x80477320–0x80477420`, and `.sbss 0x804B9A00–0x804B9A08`. Target initialization clears the pending and unissued counters and initializes 32 request-pointer/file-descriptor pairs. Queueing increments both counters, acknowledgement decrements the unissued counter, and reply decrements pending and removes the matching pair. These four functions and all their data match the reference-derived reconstruction. The reference `IPCGetQueueStatus` is retained in source but dead-stripped from the final target. Its first reference to `IpcFdArray` establishes the original BSS emission order. Omitting it reversed the two arrays and produced ten differing relocation bytes despite 100% objdiff.

## Client names needed by ESP

`0x802F1DF0` is `IOS_Close`: the target creates command 2 with the supplied file descriptor, stores null callback and argument fields, then calls `__ios_Ipc2` with a null callback. This agrees with Petari's synchronous close wrapper.

`0x802F2A10` is `IOS_IoctlvReboot`: the target checks the relaunch guard under disabled interrupts, returns -10 when already active, builds command 7 with vector arguments, sets the relaunch flag, copies the request to dedicated storage, and queues/waits for reboot acknowledgement. These operations agree with Petari's reboot wrapper. Both names lie inside #497; their source implementations are not reconstructed in this contribution. They unblock the ESP external names tracked in #496.

## Provenance and validation

Source follows CC0 Petari commit `e0ab141914921726a595573dc48d3f13d0922efe`, `src/RVL_SDK/ipc/memory.c` and `ipcProfile.c`. The file bodies carry no additional license notices. The target's `iosAlloc` wrapper was independently reconstructed from its two instructions. Only the required types and OS interrupt declarations are used, and no comments or docstrings are added to source.

Compiler: Wii/1.0 with the established `cflags_sdk`, default 16-byte function alignment. objdiff reports all five allocator functions and four profiling functions at 100%, with 1,312 and 456 code bytes respectively, plus all 392 bytes of unit data. Split/fallback build and frozen-manifest verification passed before source promotion. The final source-linked build and frozen-manifest verifier both exit 0. Separating the two trailing gaps adds two padding entries totaling 12 bytes to objdiff's reporting denominator; these are not reconstructed functions or additional source progress. Final command results are recorded in the pull request.

## Traps

The initial source-link attempt failed at DOL offset `0x6F`: `.ctors` and `.dtors` had moved eight bytes despite 100% objdiff. Source objects end at the last function instruction (`memory.c` ends `0x802F3248`, profiling ends `0x802F342C`); including the following alignment gaps in their splits removed original zero padding that the compiler does not emit. Retaining those gaps as automatic padding splits preserves their bytes. This is another instance of the existing warning that objdiff alone does not establish link identity. The remaining 22 client functions need a separate reconstruction pass, including target timeout-related storage absent from the inspected Petari client revision.
