# F-802-1: Stream sounds and task execution

Issue #802 owns 35 functions and 4,160 native code bytes. Five complete retained units reproduce the original inventory under Wii/1.1 and the existing `cflags_nw4r` policy. No compiler override or original-object fallback supplies these functions.

| Unit | Native text range | Functions / code bytes |
| --- | --- | --- |
| `snd_StrmSound.cpp` | `0x8028CAA0–0x8028D14C` | 16 / 1,612 |
| `snd_StrmSoundHandle.cpp` | `0x8028D150–0x8028D188` | 1 / 56 |
| `snd_Task.cpp` | `0x8028D190–0x8028D1D0` | 1 / 64 |
| `snd_TaskManager.cpp` | `0x8028D1D0–0x8028D980` | 12 / 1,896 |
| `snd_TaskThread.cpp` | `0x8028D980–0x8028DBA8` | 5 / 532 |

## Source and interfaces

Source and headers derive from CC0 `zeldaret/ss` revision `787b0b1e9d30e3c1d91ced61350b059ffc5a2411`. The corresponding five sources and their headers have no separate file-level license notice. Comments and disabled debug checks are removed, specific existing headers replace aggregate includes, C++ null pointers use `nullptr`, and C++03 iterator types replace reference `decltype` syntax. The original ordinary constructors, virtual-method ordering and runtime control flow are preserved.

The complete Task, TaskManager and SoundInstanceManager declarations reuse the owning #795 prerequisite #844, including its correction that removes unavailable release-disabled assertion dependencies. The stream-file declaration reuses #800 / #821. Those exact headers are temporarily staged together until the corresponding PRs merge, then their duplicate dependency commit is dropped. No competing types or source units from those issues are copied.

The new headers provide complete StrmSound, StrmSoundHandle, TaskThread and StrmPlayer declarations, plus the StrmChannel record required by the embedded player. StrmBufferPool remains a forward declaration; no unrelated #799 class implementation or #801 player source/split is taken. The unretained SoundHandle-taking StrmSoundHandle constructor body and debug-only GetSoundType helper are omitted; neither has native code here. Other unretained reference methods remain available where they require no unrelated interface.

StrmSound has the existing BasicSound prefix through `0x100`, then its `0xD08`-byte player, temporary handle at `0xE08`, instance manager at `0xE0C`, eight 16-byte track-volume values at `0xE10`, file-stream pointer at `0xE90`, and a 128-integer buffer at `0xE94`, total `0x1094`. Native constructor stores, player offsets, volume loops and all retained methods confirm the consumed layout. The ordinary initializer list matches without cast-based store ordering. The embedded player's prepared byte is at `0xF3`; its eight `0x38`-byte tracks begin at `0xB48`. StrmChannel is `0x34` bytes and preserves its buffer pointer and complete ADPCM records.

Task is `0x10` bytes: vtable pointer at zero, list node at four and busy byte at `0xC`. TaskManager is `0x3C`: three 12-byte lists at zero, current-task pointer at `0x24`, cancellation byte at `0x28`, and thread queues at `0x2C` and `0x34`. TaskThread is `0x320`: the existing `OSThread` at zero, stack-end pointer at `0x318`, finish byte at `0x31C` and creation byte at `0x31D`. Plain boolean finish accesses reproduce the inlined worker loop; no added volatile qualifier is needed.

The two previously unnamed functions are `StrmSoundHandle::DetachSound` at `0x8028D150` (56 bytes) and `Task::~Task` at `0x8028D190` (64 bytes). Matching native bodies and the original reference ordering identify them. The player Prepare callee at `0x8028A4F0` receives the complete reference header's integer-parameter mangling; paired native calls prove its address and 32-bit ABI, while its existing size, type and split remain unchanged.

## Storage and validation

StrmSound owns its 56-byte vtable at `0x803C6848`, four-byte type record at `0x804B9398`, sixteen constant bytes at `0x804BB018`, and four-byte constructor entry at `0x80352A64`. TaskManager owns `0x48` BSS bytes at `0x80452438`: the twelve-byte destructor-chain record and `0x3C`-byte static instance. Its one-byte guard is at `0x804B93A0`. The compiler also emits an unreferenced abstract Task vtable, but the original has no such retained table or relocation to it, and the linker discards it.

Configure, the original-object split-only Ninja build and the actual-directory frozen verifier exit zero before source selection. All five actual source objects then compile as NonMatching with only project include paths. Every allocated original object section scores 100% in objdiff. A separate audit resolves all 136 instruction relocations to native targets and compares every instruction byte in all 35 functions, all 76 initialized bytes, and actual zero-storage extents.

After source selection, configure, plain Ninja and `tools/verify/verify.py` against `build/wpse-v1` each exit zero. The actual linker selects all five reconstructed objects, with no original fallback for these units. All 35 final ELF function addresses and extents, and all named static-storage addresses and extents, remain native. All 451 baseline source objects preserve their code, data and relocation targets. The complete DOL is byte-identical to the unchanged frozen original, SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.

## Traps

- A diagnostic reference include path can conceal a missing project dependency. Actual Ninja rejected SoundInstanceManager's unavailable assertion header after the initial diagnostic probe succeeded. Its owner removed the release-disabled include and complete Assert/Check calls; actual compilation now uses only project headers. No substitute assertion implementation or additional debug API is introduced.
- Keep guard storage and padding distinct. The compiler emits one guard byte, so its original eight-byte estimated symbol is corrected to one byte. In this SBSS layout dtk rejects an automatic split beginning at the following unaligned byte; ending the owned split at the next four-byte boundary preserves the native zero padding. The source contains no padding declaration, and full source-selected verification confirms all following addresses. This does not impose the same rule on data sections.
- An abstract vtable can exist in the source object without being retained in the target. Searching the frozen data found no pointer to Task's destructor, and the native split has no Task data section. Do not invent a native table or allocate a data split for that unreferenced compiler output.
