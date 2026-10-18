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

- **`0x8019DC50`–`0x8019E10C`.** `0x8019DC50` is the frame loop called by `main` and `0x801E0F60`: it drives an object through virtual slots 2 to 10 with frame skipping from `VideoTiming`, keeps the game frame counter `0x804B9108` (read by `ScriptBinding_GetGameFrame`) and the flag `0x804B9110` (written by `ScriptBinding_IgnoreTimeConsumption`), and uses the 60-entry `.bss` array `0x803EA7F8`. `0x8019DEB0` seeds MSL `rand` from `OSGetTime`; `0x8019DEE0` is empty and called at the end of `main`. `0x8019DEF0`, `0x8019E010` and `0x8019E0A0` are the integer, real and Boolean random functions behind `ScriptBinding_RandInt`, `RandReal` and `RandB`, and `0x8019DFC0` builds a 32-bit value from three `rand` calls; the real functions use `1.0f` at `.sdata2` `0x804BA328`. No string, static initializer or data cluster separates the frame loop from the random functions, so their file boundaries are not established and they stay unsplit.
- **`0x8019EBA0`–`0x801A06D4`.** These open `CFileData.cpp` (source-file string `0x803BE548`), whose data names `CFileData::CWorker` with its `DecodeThread`, `LoaderThread` and `BThread`, `LoadedFileData`, `CPath::Drive` callbacks, the NAND and CNT file streams, and MSL `basic_string`, `vector` and `out_of_range` errors. The unit continues past #307 and needs those classes first.

## Method

GNU objdump 2.42 on `build/wpse-v1/main.elf`, Python scans of `extabindex`, `extab`, absolute and small-data references and callers, RTTI strings from `orig/wpse-v1/main.dol`, and `tools/re/unitdiff.py`. No decompiler output was used.

## Traps

- **Exception specifications change frame sizes.** Every `throw()` or `throw(Terminated)` adds an `__unexpected` landing pad and 0x10 bytes of frame. `Start`, `Resume`, `ThreadMain` and the destructor matched only with `throw()`, the destructor only with its loop inside a separate inline `throw()` function, and `TerminateFiber` only with both it and the inlined `Terminate` declaring `throw(Terminated)`. Count the `__unexpected` blocks and read the `extab` specification entries (`0f000000` for `throw()`, `0f000001` plus a type string for one type) before changing code.
- **Inline depth orders temporaries.** `YieldFiber` placed its two `bad_function_call` temporaries in the opposite stack slots when its body was a separate inline function. Writing the body in `YieldFiber` itself and letting `YieldFiberStarted` and `Terminate` inline `YieldFiber` matched all three.
- **A `shared_ptr` null test can be `get()`.** `Resume` tests the stack pointer with `lwz`/`cmpwi`, which is `get() != nullptr`; the safe-bool conversion gives `neg`/`or`/`srwi.`.
