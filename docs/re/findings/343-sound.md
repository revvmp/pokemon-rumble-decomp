# Issue 343: sound.cpp

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`, analysed in the linked `build/wpse-v1/main.elf` and the dtk split assembly. Compiler and flags are the game settings of [F-735-1](735-game-compiler.md). Issue #343 lists 40 functions in `.text` `0x801E5DA0`–`0x801E8738`. No decompiler output was used.

## F-343-1: unit boundaries

The block holds the end of an unidentified file and all of `sound.cpp`.

- **`0x801E5DA0`–`0x801E6D44` are not sound code.** `0x801E5DA0` starts a new file: the preceding `0x801E5CC0` is a static initializer that registers five global objects. `0x801E5DA0` (wide-string line breaking with `nw4r::ut::TextWriterBase<wchar_t>`, called through `0x801E6670` from `0x801E09B0`) and `0x801E6480` (a `wstring` substring that throws `out_of_range`, strings at `.data` `0x803C2B88`) need MSL `basic_string` (#327). `0x801E6700`, `0x801E6710` and `0x801E6750` are called from `CDividedGround::LoadAttr`, and `0x801E6760`/`0x801E6770` walk a tree of 24-byte nodes for the `moveAlongWall` and `moveByWall` bindings, using `.sdata2` `0x804BA580`–`0x804BA588`. No static initializer, deleter or string cluster separates these groups, so they stay unsplit.
- **`sound.cpp` starts at `0x801E6D50`.** The function is the reset callback that `0x801E6D80` wraps in a `std::tr1::function<void()>` (`lis`/`addic.` of `0x801E6D50`); it calls `AxManager::PrepareReset` and `SoundSystem::WaitForResetReady`. The file name comes from the RTTI names `@unnamed@sound_cpp@::SoundWork` in `.data` `0x803C2BF4` and `0x803C2C84`.
- **`sound.cpp` ends at `0x801E8738`.** Its static initializer `0x801E8710` (`.ctors` `0x80352A30`) registers the global `shared_ptr<SoundWork>` destructor `0x801E8360`. `BSoundSource` (#344) follows at `0x801E8740` with its own deleter cluster at `.data` `0x803C2CC0`.

| Section | Range | Evidence |
| --- | --- | --- |
| `extab` | `0x80041358`–`0x800432A4` | Entries of the records below; `0x800432A4` belongs to `0x801E8760` |
| `extabindex` | `0x80051FF0`–`0x800520E0` | First record names `0x801E6D50`, last `0x801E86D0` |
| `.text` | `0x801E6D50`–`0x801E8738` | As above |
| `.ctors` | `0x80352A30`–`0x80352A34` | `__sinit_\sound_cpp` |
| `.data` | `0x803C2BE0`–`0x803C2CC0` | `SoundWork` deleter vtable, its type name, base list and the `default_delete<SoundWork>` name |
| `.bss` | `0x803EAF88`–`0x803EAF98` | Destructor chain entry of `sSoundWork` |
| `.sdata` | `0x804B8608`–`0x804B8618` | Type info of the deleter and of `default_delete<SoundWork>` |
| `.sbss` | `0x804B91E0`–`0x804B91F8` | `sBgmIndex`, `sSoundWork`, `sBgmDisabled`; `0x804B91D8` belongs to `0x801E5CC0`, `0x804B91F8` to printf-area code |
| `.sdata2` | `0x804BA588`–`0x804BA5A0` | 12, 6, 20, 60, 3 and -1; `0x804BA5A0` (1.0) is used by `BSoundSource` |

The split alone links and verifies.

## F-343-2: SoundWork

`SoundWork` (0x59C bytes, `new` size 1436) is a file-local class without a base or vtable, held by the global `shared_ptr<SoundWork>` at `.sbss` `0x804B91E8`.

| Offset | Member | Type | Evidence |
| --- | --- | --- | --- |
| `0x000` | `mSystemMemory` | `shared_ptr<u8>` | Sound system work memory, `GameMemory::Free` deleter |
| `0x008` | `mFxReverb` | `nw4r::snd::FxReverbStd` | Constructor call; `GetFxReverb` |
| `0x130` | `mFxChorus` | `nw4r::snd::FxChorus` | |
| `0x1FC` | `mFxDelay` | `nw4r::snd::FxDelay` | |
| `0x280`–`0x290` | `mFxReverbMemory`, `mFxChorusMemory`, `mFxDelayMemory` | `shared_ptr<u8>` | Effect work buffers |
| `0x298` | `mArchiveFile` | `shared_ptr<CFileData>` | Extab destructor name |
| `0x2A0` | `mArchive` | `nw4r::snd::MemorySoundArchive` | `Setup` with the file data |
| `0x3F0` | `mPlayer` | `nw4r::snd::SoundArchivePlayer` | |
| `0x4DC`, `0x4E4` | `mPlayerMemory`, `mStrmBuffer` | `shared_ptr<u8>` | Player and stream buffers |
| `0x4EC` | `m3DManager` | `nw4r::snd::Sound3DManager` | |
| `0x518` | `m3DListener` | `nw4r::snd::Sound3DListener` | |
| `0x584` | `m3DManagerMemory` | `shared_ptr<u8>` | |
| `0x58C` | `mSeHandle` | `SoundHandle` | Default handle of `PlaySound` and `StopSound` |
| `0x590` | `mBgmHandles[2]` | `SoundHandle` | Indexed by `sBgmIndex`, toggled by `PlayBGM` |
| `0x598` | `mSpareHandle` | `SoundHandle` | Only constructed and destroyed in this file |

The `DESTROYBASE` actions with the empty destructor `0x801E7640` belong to the `NonCopyable` base of the inlined `SoundHandle` destructors, not to `SoundWork`; giving `SoundWork` that base added a twelve-byte action to the constructor cleanup.

## F-343-3: functions

| Address | Function | Match |
| --- | --- | --- |
| `0x801E6D50` | `OnReset()` (file-local) | 100% |
| `0x801E6D80` | `InitSound(u32 reverbSize, u32 chorusSize, u32 delaySize)` | 99.49%: register choice only |
| `0x801E7580`–`0x801E77F0` | Weak `FxReverbStd`, `FxBase`, `NonCopyable`, `FxChorus`, `FxDelay` and `Sound3DManager` destructors, `SoundHandle()` and the `Sound3DListener` destructor | 100% |
| `0x801E7830` | `ShutdownSound()` | 100% |
| `0x801E78A0` | `UpdateSound()` | 100% |
| `0x801E78B0` | `LoadSoundArchive(const char*)` | 98.70%: register choice and the stack slots of the listener `PushBack` temporaries |
| `0x801E7C30` | `SetBGMVolume(f32 volume, f32 fadeTime)` | 100% |
| `0x801E7C90` | `StopBGM(f32 fadeTime)` (pauses; name from `ScriptBinding_StopBGM`) | 100% |
| `0x801E7D00` | `GetBGMID()` | 100% |
| `0x801E7D40` | `ContinueBGM(u32 id, f32 fadeTime)` | 100% |
| `0x801E7D50` | `PlayBGM(u32 id, f32 fadeTime, bool resume)` | 100% |
| `0x801E7F30` | `PlaySound(u32, SoundHandle*, SoundStartable*)` | 100% |
| `0x801E7F70` | `StopSound(SoundHandle*, SoundActor*, f32 fadeTime)` | 100% |
| `0x801E7FF0`–`0x801E8040` | `GetSoundArchivePlayer`, `GetSound3DManager`, `GetSound3DListener`, `GetFxReverb`, `GetFxDelay`, `GetFxChorus` | 100% |
| `0x801E8050`–`0x801E86D0` | `SoundWork` deleter `get_deleter`, `dispose`, `shared_ptr<SoundWork>` destructor, `move_ptr<SoundWork>` destructor and deleter destructor | 100% |
| `0x801E8710` | `__sinit_\sound_cpp` | 100% |

Fade times are seconds: `-1` selects 3 seconds, and frames are `60.0f * seconds`. The BGM functions read the attached `BasicSound` directly; going through the inline `SoundHandle` methods computed the frame count before the null test. `InitSound` is called from `main` with `0x204C0`, 0 and `0x2EE80`; a zero size asks the effect for `GetRequiredMemSize`. It registers the reset callback with priority 0 through `0x801B5C30` (#317), which `main` also calls with -33.

All function names are descriptive (medium confidence for behaviour; low for `ContinueBGM`, `mSpareHandle`, `sBgmDisabled` and the `StopSound` parameter order). `sBgmDisabled` (`.sbss` `0x804B91F0`) is read by every BGM function but never written in the executable.

## F-343-4: weak function order and TR1 header layout

With `-ipa file` the compiler emits deferred template members grouped by class template in reverse order of the class's first declaration in the translation unit. The original places the `SoundWork` deleter's `get_deleter` and `dispose` before the `shared_ptr<SoundWork>` and `move_ptr<SoundWork>` destructors, so `shared_ptr_deleter` must be first declared after `shared_ptr`. With a forward declaration of `shared_ptr_deleter` before `shared_ptr` the `shared_ptr` destructors stayed first. `include/memory` now defines `default_delete` itself and includes `MSL_CPP/detail/shared_ptr_deleter.h` at its end; every other game unit's objdiff counts are unchanged and the DOL verifies. `CDlgPiiList.cpp` shows the same original order (`get_deleter`, `dispose`, then `shared_ptr` destructors at `0x8008F070`–`0x8008F180`).

`include/functional` gains the function-pointer constructor of `function<R()>` and `function_imp<R (*)(), F, false, false>`. The original type name `std::tr1::detail::function_imp<void (*)(), void (*)(), false, false>` fixes the parameter order `<Signature, F, bool, bool>` (other instances show `function_imp<void (*)(), bound_func<...>, false, false>`). The plain implementation reproduces the original copies at `0x8012FAB0`–`0x8012FB40`, including the frame-pointer `stw r1` of the placement clone.

## F-343-5: names outside the unit

For the link `symbols.txt` names the weak copies the object references or emits: `SoundHandle::~SoundHandle` `0x800BCAE0`; `function_imp<void (*)(), void (*)(), false, false>` `clone` `0x8012FAB0`, `clone(void*)` `0x8012FB00`, `operator()` `0x8012FB40`, destructor `0x80131640`, vtable `0x803B5748` and type info `0x804B78F8`; `function_base<void (*)()>` destructor `0x80131600`, vtable `0x803B5800` and type info `0x804B7900`; and the `shared_ptr_deleter_common` type info `0x804B5AB8` (all high: the compiler emits these names). Descriptive names for called functions: `LoadFile(const char*, u32, bool)` `0x8005ED70`, a tail-calling wrapper of the `CFileData` loader `0x801A6420`, and `RegisterResetCallback(const function<void()>&, int)` `0x801B5C30` (low confidence).

`include/game/CFileData.h` declares the part of `CFileData` the unit needs: size at 0x10 (`ARCGetLength` in the loader at `0x8019F0C0`) and data pointer at 0x1C (`ARCGetStartAddrInMem`, `0x8019F0B4`), with the flags, path string, buffer and archive members known from F-326-5 and the loader.

## Status

`game/sound.cpp` is registered `NonMatching`: 30 of 32 compiled functions are 100%, and with `InitSound` and `LoadSoundArchive` as they are, linking the object reproduces every section except those two functions' register choices and the matching `extab` register fields. Changing source and header forms (named locals, helper functions, explicit constructors and destructors, `operator=`, list insertion forms, include order, every Wii compiler, `-inline` and `-ipa` variants, `-O4,s`) did not change the allocation.

## Traps

- Compiling a natural SliceText caller against the unadapted recovered 2006 MSL headers emits the native484-byte substring exactly, including all actual call targets and referenced exception data. `343-primary-full.py` uses the unchanged game flags and direct Wii/1.1 compiler, four original C/C++ common/PPC_EABI include directories, and an outside dolphin/os.h forwarder to the canonical Revolution header. Direct invocation avoids sjiswrap's rejection of Latin-1 copyright text in the original headers; the producer is ASCII. `343-primary-full-strict.py` records `343-primary-full-strict-proof.json`. Its original random-access do_replace name is explicitly bound to the independently established native primitive at80082260, whose canonical forward-iterator template name was inferred. This is caller proof, not acceptance of every emitted helper: original do_replace is1068 bytes versus the existing native1076-byte primitive. The full original-header WrapText remains1780 versus1760 native, including when compiled with the actual TextBox wrapper, so source/header context still needs resolution there.

- The recovered unadapted 2006 MSL string stores `compressed_pair<rep_s, allocator_type>` in representation-first order; the allocator is second. Its const long-pointer accessor returns a pointer by value, while the mutable overload returns a reference. Substr forwards the allocator to the four-argument substring constructor. The substring replacement advances its source pointer before clamping the count. The old outside #343 draft reversed those two operations. A faithful sparse outside adaptation reduces its equal-484-byte substring diagnostic from 47 to 30 nonrelocation differences; most remaining differences occur in the out_of_range exception's shared-pointer allocation path, with an additional retained-source register difference. WrapText remains unmatched at 1784 versus 1760 native. Reproduce with `343-primary-string.py` and `343-primary-opcodes.py`; full actual-target validation remains rejected and no canonical string edit is proposed from these results.
- The Menu owner independently tested a coherent representation-first snapshot in its actual producer: CUIMenuItem's constructor at 801DC6C0 becomes the exact native192 bytes, all targets and EH28 included, and eight other Menu/Item methods preserve1844 code bytes and EH92. Its receipts are `337-menu-reference/337-primary-string-code-proof.json` and `337-primary-string-eh-proof.json`. Balloon's coherent snapshot preserves280 mapped functions and267 instruction-exact functions; representation alone does not fix its separate game formatter helper context. These are paired native controls, not all-consumer or whole-string acceptance.
- Original shared_ptr construction with an explicit deleter uses `move_ptr<Y, D&>` and its pointer/deleter constructor, rather than the current private holder. A faithful outside constructor control compiles but leaves this substring's484-byte/30-difference diagnostic unchanged. It provides no native improvement justifying a shared-memory publication. `343-primary-string.py owner` reproduces it; the default regenerates the stable sparse string-only overlay.

- A fresh primary shared-pointer swap proposal removes InitSound's remaining nonrelocation differences without changing its 2036-byte extent. In LoadSoundArchive it removes the earlier shared-owner register mismatch, leaving exactly eight instruction differences in the listener-list iterator construction at offsets 2F4, 300, 304, 308, 30C, 314, 318 and 31C. This outside diagnostic skips relocation words and is not a complete actual-target or exception-table proof. Reproduce with `343-current-sound.py` and `343-current-sound-diff.py`; receipt `343-current-sound/primary-swap-opcode-proof.json` uses the immutable `280-reference/primary-swap/include/memory` proposal. No competing canonical memory edit is made.
- Bounded current LinkList controls do not resolve those eight words: a const-reference converting iterator constructor and using the existing GetIteratorFromPointer factory are unchanged; explicit base/typed iterator copy constructors shorten LoadSoundArchive to 876/880 bytes, and a named end iterator grows it to 892 bytes. The NW4R owner has no independent native declaration evidence supporting a different contract. These controls stay outside; preserve the canonical implicit copy contract.

- **An empty base destructor in an extab can belong to an inlined member.** `DESTROYBASE 0x0(r26)` in the `SoundWork` destructor path was the `NonCopyable` base of a `SoundHandle` whose address was in r26, not a base of `SoundWork`. Check which object the register holds at that PC before adding a base class.
- **Deferred template members follow class declaration order.** Moving out-of-class definitions did not change emission order; moving the class's first declaration (including forward declarations) did.
- **A Windows `python -` with a Git Bash heredoc hangs even with `< /dev/null`.** Write scripts to files.
## F-343-6: Complete CBSPTree trace contract

A fresh current-main producer reproduces all five CBSPTree functions at 801E6700, 801E6710, 801E6750, 801E6760 and 801E6770: 1,592 instruction bytes, every branch and small-data constant resolved, and both complete EH records totaling 16 bytes. The ordinary recursive segment traversal uses the canonical VEC3 dot product. Its private GetChild(node, child) first reads the data base, then the node child index; taking the index as a precomputed value changes every repeated lookup schedule. No register constraints, new assembly, compiler flags or forced emission are used.

The class remains four bytes, with its existing void-pointer storage and public constructor, destructor and SetData API preserved. Its 24-byte Node holds a VEC3 normal at0, plane distance at12 and a union at16: four u16 child indices, or a leaf marker FFFFFFFF plus a relative payload offset at20. Zero and FFFF child indices are null; other indices count 24-byte records from the data base. The root starts at byte48. Trace is36 bytes: start vector0, direction12, hit fraction24, node pointer28 and payload pointer32. Const qualifications and the names Node, Trace, TraceSegment, GetChild and their members are descriptive inferences; field uses and code generation are directly proven.

The preceding home-button owner independently establishes its last initializer ending at801E5D98. The five CBSPTree methods begin a distinct complete class group at801E6700; its only scalar constants are zero and0.001 at804BA580..804BA588, immediately before sound.cpp constants. This replaces the earlier conservative unsplit treatment of these groups as a requirement: lack of a constructor-table entry does not itself join unrelated complete utility classes. A source-selected split and full frozen verification remain required before marking this unit Matching. The first three issue functions remain separate wide-text wrapping and substring work. Current Sound.cpp still reproduces30 of32 original functions, with the same two previously documented register contexts; its extra dead-strippable weak definitions are not evidence of a new match.

The bounded shared-header prerequisite is controlled against all actual Ninja-derived source objects and the linked ELF and frozen DOL on the same base, inside official landing verification after every rebase. It introduces no source-selection or split change and does not complete issue343. Detailed source, immutable-object comparisons and receipts are retained in343-reference.


## F-343-7: Source-selected CBSPTree unit

The complete class unit spans text801E6700..801E6D44, extab80041348..80041358, extabindex80051FD8..80051FF0 and sdata2 804BA580..804BA588. Its 1,604 allocated text bytes include normal inter-function alignment; only1,592 instruction bytes count as reconstructed code. There are five real methods and no constructor table, retained RTTI, additional data or original-code fallback inside this unit. The source filename CBSPTree.cpp follows the existing canonical class spelling and is inferred rather than an embedded original filename.

The original-only split passes configure, plain Ninja and frozen verification, all exit0. The real NonMatching source object then reports100% for all five functions and all four allocated sections. Independent allocated-section comparison resolves all23 relocations and matches all1,652 bytes exactly, including alignment, both EH records and every exception-index entry. Selecting the object Matching also passes configure, plain Ninja and frozen verification, all exit0. Acceptance inspects the actual Ninja main.elf input edge, a freshly generated linker map and final linked method addresses and bytes; all checks repeat inside official landing verification after every rebase.

This is a complete source-selected utility class, not completion of the original issue. The other three pre-Sound functions and the two documented Sound register contexts remain unfinished. The wide-text draft reaches the native1760/484-byte extents but does not match; its new substring and exception declarations stay outside and are not part of this source change. No speculative string or exception header correction is published.
