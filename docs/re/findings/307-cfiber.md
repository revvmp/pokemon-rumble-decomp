# CFiber unit (#307)

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`, analysed in the linked `build/wpse-v1/main.elf`. Compiler and flags are the game settings of [F-735-1](735-game-compiler.md). Issue #307 lists 30 functions in `.text` `0x8019DC50`–`0x801A06D4`. They belong to three groups: a main-loop and random-number group (`0x8019DC50`–`0x8019E10C`, seven functions), the `CFiber` unit (`0x8019E110`–`0x8019EB94`, eight functions), and the start of `CFileData.cpp` (`0x8019EBA0` onwards, fifteen functions in this issue).

## F-307-1: unit boundaries

The unit is registered as `game/CFiber.cpp` (file name from the RTTI class name; no source-file string is embedded).

| Section | Range | Evidence |
| --- | --- | --- |
| `extab` | `0x8003504C`–`0x800353CC` | Entries of the `extabindex` records below; the record for `0x8019EBE0` uses `0x800353CC` |
| `extabindex` | `0x80050094`–`0x800500F4` | First record names `0x8019E110`, last `0x8019E9C0`; `0x800500F4` names `0x8019EBE0` |
| `.text` | `0x8019E110`–`0x8019EB94` | `0x8019E110` initializes the mutex and conditions of the object whose vptr main stores at 0x368 before calling it; `0x8019EBA0` zeroes a 0x30-byte object and belongs to the `CFileData` cluster |
| `.data` | `0x803BE408`–`0x803BE478` | `"!CFiber::Terminated!"` (exception-specification and catch type, referenced only from this unit's `extab`), `"!CFiber::Terminated!!"` and `"!std::exception!!std::tr1::bad_function_call!!"` (throw types), and the vtable `0x803BE468`. `0x803BE478` starts the second throw-type cluster used by `0x8019EBE0` and is followed by `"CFileData.cpp"` at `0x803BE548` |
| `.sdata` | `0x804B80F8`–`0x804B8108` | `"CFiber"` and the type info `0x804B8100`; `0x804B80F4` (`"/./"`) is used by `CNTOpen`, `0x804B8108` by `0x801A0760` |
| `.sbss` | `0x804B9120`–`0x804B9128` | `CFiber::sCurrent`; `0x804B9108`–`0x804B911C` belong to the main loop, `0x804B9128` to `CFileData` |

No static initializer belongs to the unit. objdiff reports 8/8 functions and 2,656/2,656 code bytes at 100%, and the unit links as `Matching`: `configure.py`, `ninja` and `tools/verify/verify.py` exit 0.

## F-307-2: CFiber layout

The class is 0x370 bytes: `CDlgPiiListImpl` holds it at 0x90 with its vptr at 0x3F8, `BNoticeDisplayImpl` at 0xF0 with the vptr at 0x458 (F-82, F-160), and main allocates it with the vptr store at 0x368.

| Offset | Member | Type | Evidence |
| --- | --- | --- | --- |
| `0x00` | `mFlags` | `u32` | Bit 0 started (`YieldFiberStarted`, `Start` loop), bit 1 finished (thread exit, `Resume`), bit 2 terminating (`Terminate`, destructor; `YieldFiber` throws), bit 3 running (`Resume`) |
| `0x04` | `mStack` | `std::tr1::shared_ptr<u8>` | `Start` resets it with `GameMemory::Allocate(size, 32)` and the deleter `GameMemory::Free`, type `shared_ptr_deleter<unsigned char, void (*)(void *) throw()>` (vtable `0x803A42AC`); `Resume` resets it after joining the thread |
| `0x0C` | `mOnSuspend` | `std::tr1::function<void()>` | Called by `YieldFiber` before it signals and waits |
| `0x18` | `mOnResume` | `std::tr1::function<void()>` | Called by `YieldFiber` after it is resumed |
| `0x24` | `mMutex` | `OSMutex` | Held by the fiber thread while it runs |
| `0x3C` | `mResumeCond` | `OSCond` | Signalled by `Resume`, waited on by `YieldFiber` |
| `0x44` | `mSuspendCond` | `OSCond` | Signalled by `YieldFiber` and thread exit, waited on by `Resume` |
| `0x50` | `mThread` | `OSThread` | `OSCreateThread` in `Start`; 0x318 bytes |
| `0x368` | vptr | | Vtable `0x803BE468` holds only the destructor |

The data members are declared before the virtual destructor, which places the vptr at the end. Callers construct the object inline: they store the vptr, zero `mStack` and the two `function` pointers, then call `Initialize()` (`0x8019E110`), which clears `mFlags` and initializes the mutex and conditions.

`std::tr1::function<void()>` is 12 bytes: a pointer to its `function_base<void (*)()>` at 0 and an 8-byte local buffer at 4. Virtual slots of the base, from the calls in this unit: destructor (0x8, called with -1 for the local buffer and 1 otherwise), `clone()` (0xC), `clone(void*)` into the buffer (0x10) and `operator()` (0x14). The class names `function_base<void (*)()>`, `function_imp<...>` and `bad_function_call` come from RTTI strings in game `.data`, for example `0x803BE718`, `0x803BE650` and `0x803A8A00`. `include/functional` declares only the zero-argument form.

## F-307-3: functions

| Address | Function | Notes |
| --- | --- | --- |
| `0x8019E110` | `CFiber::Initialize() throw()` | |
| `0x8019E180` | `CFiber::~CFiber() throw()` | Inlines `Abort() throw()`: sets started and terminating, then resumes until finished unless it is the current fiber |
| `0x8019E320` | `CFiber::Start(const function<void()>&, u32) throw()` | Creates the thread at priority 16 with a `{this, &body}` parameter and resumes until the body reports that it started |
| `0x8019E460` | `CFiber::Resume() throw()` | Switches `sCurrent`, wakes the thread and waits for it to yield; joins it and frees the stack once finished |
| `0x8019E5A0` | `TerminateFiber(int) throw(CFiber::Terminated)` | Inlines `CFiber::Terminate(int) throw(Terminated)` on the current fiber, so it always throws `Terminated`; all four callers pass 1 |
| `0x8019E770` | `YieldFiber()` | Name from #314 (F-314-3) |
| `0x8019E890` | `YieldFiberStarted()` | Sets the started bit and yields; fiber bodies such as `CDlgPiiListImpl::Run` call it first |
| `0x8019E9C0` | `CFiber::ThreadMain(void*) throw()` | Sets up the paired-single GQRs through `nw4r::g3d::fastcast::detail::Init`, copies the body, runs it, catches `Terminated`, then marks the fiber finished |

`CFiber` and `CFiber::Terminated` are RTTI and throw-type names. Method, member and parameter names are descriptive (medium); `YieldFiber` keeps its #314 name.

Outside the unit, renamed for the link because the object emits weak copies or references them: `std::tr1::function<void()>::~function` `0x80070400`, `bad_function_call` destructor `0x800E1A50`, `what` `0x800E1810`, vtable `0x803A89F0` and type info `0x804B7030`; the `shared_ptr_deleter<unsigned char, void (*)(void *) throw()>` destructor `0x800BE770`, `get_deleter` `0x800BE3A0`, `dispose` `0x800BE3F0`, vtable `0x803A42AC` and type info `0x804B6940`; and the `shared_ptr_holder` destructor `0x800BE5D0`.

## F-307-4: the rest of #307

- **`0x8019DC50`–`0x8019E10C`.** `0x8019DC50` is the frame loop called by `main` and `0x801E0F60`: it drives an object through virtual slots 2 to 10 with frame skipping from `VideoTiming`, keeps the game frame counter `0x804B9108` (read by `ScriptBinding_GetGameFrame`) and the flag `0x804B9110` (written by `ScriptBinding_IgnoreTimeConsumption`), and uses the 60-entry `.bss` array `0x803EA7F8`. `0x8019DEB0` seeds MSL `rand` from `OSGetTime`; `0x8019DEE0` is empty and called at the end of `main`. `0x8019DEF0`, `0x8019E010` and `0x8019E0A0` are the integer, real and Boolean random functions behind `ScriptBinding_RandInt`, `RandReal` and `RandB`, and `0x8019DFC0` builds a 32-bit value from three `rand` calls; the real functions use `1.0f` at `.sdata2` `0x804BA328`. No string, static initializer or data cluster separates the frame loop from the random functions; [F-307-5](#f-307-5-gameloopcpp-and-randomcpp) later split them into two units.
- **`0x8019EBA0`–`0x801A06D4`.** These open `CFileData.cpp` (source-file string `0x803BE548`), whose data names `CFileData::CWorker` with its `DecodeThread`, `LoaderThread` and `BThread`, `LoadedFileData`, `CPath::Drive` callbacks, the NAND and CNT file streams, and MSL `basic_string`, `vector` and `out_of_range` errors. The unit continues past #307 and needs those classes first; [F-307-6](#f-307-6-cfiledatacpp-split) records its split.

## F-307-5: GameLoop.cpp and Random.cpp

The first seven functions of #307 (`0x8019DC50`-`0x8019E10C`) are now two Matching units. F-307-4 recorded them as unsplit because no string, static initializer or data cluster separates them. The boundary between the two units is a reconstruction choice (medium confidence): the frame loop owns all of its data, while the six random functions share no data with it and start with the seed call that `main` pairs with an empty function at its end.

| Unit | Section | Range | Evidence |
| --- | --- | --- | --- |
| `game/GameLoop.cpp` | `extab` | `0x8003501C`-`0x80035024` | One 8-byte record |
| | `extabindex` | `0x8005004C`-`0x80050058` | One record, for `0x8019DC50` |
| | `.text` | `0x8019DC50`-`0x8019DEAC` | `RunGameLoop`, 0x25C bytes; `0x8019DEAC` is padding to the next 16-byte start |
| | `.bss` | `0x803EA7F8`-`0x803EA834` | 60 `bool` render history; the symbol was 0x40 bytes and had to be cut to 0x3C |
| | `.sbss` | `0x804B9108`-`0x804B911C` | `gGameFrame`, `gFrameStep`, `gFrameTimingReset`, render count, history index |
| `game/Random.cpp` | `extab` | `0x80035024`-`0x8003504C` | Five records, 40 bytes |
| | `extabindex` | `0x80050058`-`0x80050094` | Five records |
| | `.text` | `0x8019DEB0`-`0x8019E110` | Six functions |
| | `.sdata2` | `0x804BA328`-`0x804BA32C` | `1.0f`; the symbol was 8 bytes and was cut to 4 |

The exception ranges are the gaps left between `luckyhelper.cpp` (ends `0x8003501C`, `0x8005004C`) and `CFiber.cpp` (starts `0x8003504C`, `0x80050094`): 48 bytes of `extab` and 72 bytes (six records) of `extabindex`, one record per function in function order. Every function in an exception-enabled game unit gets a record, even leaf functions.

`RunGameLoop(IGameLoopCallback*)`: the class name `IGameLoopCallback` is the RTTI string at `0x803B5FEC`, and the implementing class is `@unnamed@main_cpp@::CMain` (vtable `0x803B5EE4`, typeinfo `0x803B5F10`). The interface has no virtual destructor; its nine virtual functions are at vtable offsets 8 to 40. `main` (at `0x8012C3DC`) and `ShowSystemMessage` (at `0x801E0F60`) call the loop with an embedded object. The slot names are descriptive and low to medium confidence: `IsRunning`, `ResetTiming` (`CMain` calls `SetPadReadDelay(1)`), `Update` (pad, controllers, sound), `ShouldRender`, `Prepare`, `Calc`, `Draw`, `Render` (resets GX state and copies the frame buffer) and `Present` (`DisplayFrameBuffer`).

The loop keeps `elapsedTicks` and `budgetTicks`, repeats `Update` while the elapsed video time exceeds the budget, and resynchronises after `gFrameTimingReset` is set. After each frame it records in a 60-entry ring whether each update was the rendered one, then waits for the retrace unless it resynchronises. `gFrameStep` is the number of rendered updates, so `FrameStepScope` restores it. `gGameFrame` (`0x804B9108`) is read by `ScriptBinding_GetGameFrame`.

Random: `InitRandom` (`srand(OSGetTime())`), `ShutdownRandom` (empty, called at the end of `main`), `RandInt(min, max)`, `Rand32`, `RandReal` and `RandBool`. MSL `rand` is `0x80345B08` and `srand` is `0x80345B28` (renamed, declared in `stdlib.h`). `RandReal` and `RandBool` share an inline that builds a float in [1, 2) from 23 random bits and subtracts one.

## F-307-6: CFileData.cpp split

`game/CFileData.cpp` is split and registered `NonMatching` without a source file, so the build still uses the original object. The split verifies (`configure.py`, `ninja` and `verify.py` exit 0). The functions of #307 from `0x8019EBA0` onward are its first fifteen; the unit continues through #308 to #312.

| Section | Range | Evidence |
| --- | --- | --- |
| `.text` | `0x8019EBA0`-`0x801ADC38` | The only `.ctors` entry between `ut_TextWriterBase.cpp` and `ut_CntFileStream.cpp` (`0x80352A10`) points at `0x801ADB40`, whose size ends at `0x801ADC38`; the next function `0x801ADC40` belongs to `CPath.cpp` (its source string is at `0x803BF000`) |
| `.ctors` | `0x80352A10`-`0x80352A14` | The gap between the neighbors |
| `extab` | `0x800353CC`-`0x80037A98` | `CFiber` ends at `0x800353CC`; the `extabindex` record for `0x801ADC40` names `0x80037A98` |
| `extabindex` | `0x800500F4`-`0x800505C8` | `CFiber` ends at `0x800500F4`; the record at `0x800505C8` is `0x801ADC40` |
| `.data` | `0x803BE478`-`0x803BF000` | Source string `CFileData.cpp` at `0x803BE548` and `0x803BEFD0`, error strings, `CWorker` thread names, deleters and RTTI; `CPath.cpp` at `0x803BF000` starts its data |
| `.bss` | `0x803EA838`-`0x803EA890` | Three objects of 0x10, 0x28 and 0x20 bytes constructed by the static initializer at `0x801ADB40`; `pad.cpp` starts at `0x803EA890` |
| `.sdata` | `0x804B8108`-`0x804B81A0` | Cross-references from the two text ranges (`lis`/`addi`/load pairs and SDA bases): `0x804B8108`-`0x804B8198` from `CFileData`, `0x804B81A0`-`0x804B81D0` (`nand`, `nand:`, `dvd:`) from `CPath` |
| `.sbss` | `0x804B9128`-`0x804B9130` | The `CWorker` singleton pointer, read by `0x8019EBE0` |

`CPath.cpp` (`0x801ADC40` onward, before the CNT file-stream group) is a separate unit that #313 owns. It has no static initializer, and no `.rodata`, `.bss` or `.sdata2`. Its `.sdata` is `0x804B81A0`-`0x804B81D8`.

The first fifteen functions, by reading only (medium or low confidence; roles are descriptive):

| Address | Role |
| --- | --- |
| `0x8019EBA0` | Zeroes a 0x30-byte request record (words at 0 to 0x24, a byte at 0x28, a word at 0x2C) |
| `0x8019EBE0` | `CWorker` job-processing loop (0xA70 bytes). Takes jobs from a queue, handles states 5, 6, 8, 9 and 10, reports errors -6000 to -6010, decompresses with `CXGetUncompressedSize`, allocates MEM1 or MEM2 by the flag bits `0x30000`/`0x20000`, opens ARC archives and validates `nw4r::g3d::ResFile`. Without a worker it reduces to the loaded check |
| `0x8019F650`, `0x8019F750`, `0x801A06C0` | `shared_ptr` safe-bool conversions (`get() != nullptr`) |
| `0x8019F670` | Tries MEM1 or MEM2 allocation by the same flag bits, with a 4-byte minimum alignment, negated for a top-down request |
| `0x8019F770` | Appends a job to a mutex-and-condition protected linked list (head at 0x24, next at 0x20) and signals |
| `0x8019F7F0` | Opens an ARC archive in memory and sets up a `ResFile` (same work as the ARC branch of `0x8019EBE0`) |
| `0x8019F9B0`, `0x8019FB80` | `shared_ptr::get` |
| `0x8019F9C0` | Job record deleting destructor: releases four `shared_ptr` members at 0x04, 0x0C, 0x14 and 0x28 and a `CScopedActivity` at 0x24 |
| `0x8019FAE0` | Invokes slot 5 of a `std::tr1::function<void(CPath::Drive, const char*, long, unsigned long)>`, throwing `bad_function_call` when empty |
| `0x8019FB90` | Resets a `shared_ptr<T>` with a custom deleter (`T*` at 0, control block at 4) |
| `0x8019FC60` | Runs `0x8019EBE0` when the flag is clear, otherwise hands the file to `0x8019FCD0` (when the singleton exists) or `0x801288F0` |
| `0x8019FCD0` | 0x9EC-byte `CFileData` and `CWorker` routine, not yet read in detail |

The `CWorker` singleton pointer is `0x804B9128`. From `0x8019EBE0` and `0x8019F770`: mutex `+0x96B0`, condition `+0x96C8`, queue head `+0x96D0` and count `+0x96D8` serve the finished-job queue, and mutex `+0x35C`, condition `+0x374` and queue head `+0x37C` the pending queue. These offsets are unverified as member names.

None of the fifteen is reconstructed in #307. They are instantiations of `shared_ptr`, `function` and queue templates, ARC and compression handling, and thread bodies, and the compiler emits weak template copies according to use across the whole file. Matching even one of them needs the unit's class model (`CFileData`, `CWorker`, `LoadedFileData` and the job record), which spans #308 to #312.

## Method

GNU objdump 2.42 on `build/wpse-v1/main.elf`, Python scans of `extabindex`, `extab`, absolute and small-data references and callers, RTTI strings from `orig/wpse-v1/main.dol`, and `tools/re/unitdiff.py`. No decompiler output was used.

## Traps

- **Exception specifications change frame sizes.** Every `throw()` or `throw(Terminated)` adds an `__unexpected` landing pad and 0x10 bytes of frame. `Start`, `Resume`, `ThreadMain` and the destructor matched only with `throw()`, the destructor only with its loop inside a separate inline `throw()` function, and `TerminateFiber` only with both it and the inlined `Terminate` declaring `throw(Terminated)`. Count the `__unexpected` blocks and read the `extab` specification entries (`0f000000` for `throw()`, `0f000001` plus a type string for one type) before changing code.
- **Inline depth orders temporaries.** `YieldFiber` placed its two `bad_function_call` temporaries in the opposite stack slots when its body was a separate inline function. Writing the body in `YieldFiber` itself and letting `YieldFiberStarted` and `Terminate` inline `YieldFiber` matched all three.
- **A `shared_ptr` null test can be `get()`.** `Resume` tests the stack pointer with `lwz`/`cmpwi`, which is `get() != nullptr`; the safe-bool conversion gives `neg`/`or`/`srwi.`.
- **CodeWarrior evaluates additive operands right to left.** `(rand() << 15) + (rand() << 30) + rand()` calls `rand` in a different order from the original; `(rand() << 30) + (rand() << 15) + rand()` reproduces the three calls in the observed order. Check call order before blaming register allocation.
- **A multiplication can need a named temporary.** In `RunGameLoop`, `elapsedTicks += (now - retraceCount) * retraceTicks` emitted `mullw r0, r27, r0`; assigning the difference to a local first emitted the original operand order.
- **Function order inside a unit is part of the match.** `Rand32` placed before `RandInt` moved `RandInt` and every call to it, and verification failed at the call sites; source order must follow address order.
- **`unitdiff.py` compares against the previous split.** After changing `splits.txt`, run `ninja` once before trusting the `extab` and `extabindex` percentages. A split that omits an exception range leaves the target with none.
- **A symbol straddling a new split edge blocks dtk.** `lbl_803EA7F8` (0x40) and `lbl_804BA328` (8) had to be resized before the split would load; leave the padding bytes unowned.
- **Exception ranges can be found by arithmetic.** When neighbors own `extab` and `extabindex`, the gap between them holds one record per function of the units in between (8-byte `extab` entries or more, 12-byte `extabindex` records); split in function order and let the build confirm.
