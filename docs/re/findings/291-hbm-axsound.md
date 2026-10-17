# Issue 291: HBM homebutton core, block 5

## F-291-1: HBMAxSound.cpp spans 0x8017B540 to 0x8017C064

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`). The first five functions of this block (`0x8017B3A0`–`0x8017B53C`) end `RVL_SDK/hbm/HBMRemoteSpk.cpp`, matched by #621; see [F-290-1](290-hbm-remotespk.md#f-290-1-hbmremotespkcpp-spans-0x8017ac20-to-0x8017b53c). The next ten form `RVL_SDK/hbm/HBMAxSound.cpp`, the Home Button Menu sequence player built on the `HBMSEQ`, `HBMSYN` and `HBMMIX` sound libraries.

Boundaries:

| Section | Range | Contents |
| --- | --- | --- |
| `.text` | `0x8017B540`–`0x8017C064` | 10 functions |
| `.rodata` | `0x80366A68`–`0x80366A9C` | `WT_FILENAME` (`wt\HomeButtonSe.wt`), `PCM_FILENAME` (`wt\HomeButtonSe.pcm`), floats `0.0f`, `10.0f` and `20.0f` |
| `.data` | `0x803BA300`–`0x803BA650` | 28 MIDI path strings and the 28-entry `SOUND_FILENAME` table (`0x803BA5E0`) |
| `.bss` | `0x803E9B88`–`0x803E9B8C` | `sWork`, the work-area pointer (dtk guessed 8 bytes; the object is 4) |

The `.rodata` split ends at the last float. The word at `0x80366A9C` is padding before the next unit's 8-byte-aligned data.

Name evidence: CC0 xenoblade (`xbret/xenoblade`, `config/jp/symbols.txt` and `splits.txt`) lists `RVL_SDK/src/revolution/hbm/HBMAxSound.cpp` with the same ten functions in the same order and sizes. Its symbols give:

- `GetFreePlayer`, `AudioFrameCallback` and `AudioSoundThreadProc`, in a file-scope unnamed namespace;
- the seven `homebutton` functions;
- the data names `SOUND_FILENAME`, `WT_FILENAME`, `PCM_FILENAME` and `sWork`.

No reference source exists for this file, so the code was written from the target's instructions. The `HBMSEQ`, `HBMSYN` and `HBMMIX` declarations and layouts come from CC0 dusklight `34a144a4` (`libs/revolution/src/homebuttonLib/sound`). Callee addresses match their call patterns, and every function matches.

The MIDI table has 28 entries, six more than the 22-entry enum in the xenoblade and dusklight headers. The extra entries are the manual sounds `HBMSE_MANUAL_OPEN` through `HBMSE_MANUAL_RETURN_APP` (22–27), named after their file names. `HBMSE_FOCUS`, `HBMSE_MANUAL_FOCUS` and `HBMSE_MANUAL_SCROLL` play on a separate three-player cursor pool with its own active list. All other sounds share a four-player pool. When a pool is full, the oldest active sequence in that list is stopped and reused.

On the branch head, `configure.py`, `ninja` and `tools/verify/verify.py verify` exit 0 with the unit `Matching`, and objdiff reports `.text`, `.rodata`, `.data` and `.bss` at 100%:

| Address | Size | Name |
| --- | --- | --- |
| `0x8017B540` | `0x198` | `GetFreePlayer__24@unnamed@HBMAxSound_cpp@Fi` |
| `0x8017B6E0` | `0x64` | `AudioFrameCallback__24@unnamed@HBMAxSound_cpp@Fv` |
| `0x8017B750` | `0x240` | `AudioSoundThreadProc__24@unnamed@HBMAxSound_cpp@FPv` |
| `0x8017B990` | `0x13C` | `PlaySeq__10homebuttonFi` |
| `0x8017BAD0` | `0x1F4` | `InitAxSound__10homebuttonFPCvPvUl` |
| `0x8017BCD0` | `0x88` | `ShutdownAxSound__10homebuttonFv` |
| `0x8017BD60` | `0x4` | `AxSoundMain__10homebuttonFv` |
| `0x8017BD70` | `0x1C4` | `StopAllSeq__10homebuttonFv` |
| `0x8017BF40` | `0x120` | `SetVolumeAllSeq__10homebuttonFf` |
| `0x8017C060` | `0x4` | `SetSoundMode__10homebuttonFUl` |

Outside callees renamed for linking:

| Address | Name |
| --- | --- |
| `0x8019A370` | `HBMMIXInit` |
| `0x8019A570` | `HBMMIXQuit` |
| `0x8019A590` | `HBMMIXSetSoundMode` |
| `0x8019B060` | `HBMMIXUpdateSettings` |
| `0x8019BCD0` | `HBMSYNInit` |
| `0x8019BDB0` | `HBMSYNQuit` |
| `0x8019BDD0` | `HBMSYNRunAudioFrame` |
| `0x8019D7D0` | `HBMSEQInit` |
| `0x8019D800` | `HBMSEQQuit` |
| `0x8019D820` | `HBMSEQRunAudioFrame` |
| `0x8019D9B0` | `HBMSEQAddSequence` |
| `0x8019DA40` | `HBMSEQRemoveSequence` |
| `0x8019DAE0` | `HBMSEQSetState` |
| `0x8019DC30` | `HBMSEQGetState` |
| `0x8019DC40` | `HBMSEQSetVolume` |
| `0x802BC3B0` | `OSCreateThread` |
| `0x802BC8F0` | `OSJoinThread` |
| `0x802BCA40` | `OSResumeThread` |
| `0x802DA010` | `AICheckInit` |
| `0x802DA020` | `AIInit` |

Layout. Member names are descriptive except where xenoblade gives them.

`SeqPlayer` is 0x2E2C bytes:

| Offset | Member |
| --- | --- |
| `0x0` | `HBMSEQSEQUENCE` (0x2E1C bytes) |
| `0x2E1C` | `inUse` |
| `0x2E20` | `next` |
| `0x2E24` | `prev` |
| `0x2E28` | sound number |

`AxSoundWork` is 0x14700 bytes:

| Offset | Member |
| --- | --- |
| `0x0` | four players |
| `0xB8B0` | three cursor players |
| `0x14334` | player list (head, tail) |
| `0x1433C` | cursor player list (head, tail) |
| `0x14344` | a word this unit never accesses (`reserved`) |
| `0x14348` | previous AX callback |
| `0x1434C` | ARC handle |
| `0x14368` | `OSThread` |
| `0x14680` | message queue |
| `0x146A0` | four messages |
| `0x146B0` | thread stack pointer |
| `0x146B4` | wavetable |
| `0x146B8` | samples |
| `0x146BC` | 0x44 bytes this unit never accesses (`padding`) |

`InitAxSound` requires `sizeof(AxSoundWork) + 0x4000` bytes and gives the remainder to the sound thread as its stack. The thread wakes on message 1 from the AX frame callback and exits on message 8.

## F-291-2: remaining functions of this block

`0x8017C070`–`0x8017C46C` is `RVL_SDK/hbm/HBMCommon.cpp`, by xenoblade's split. It holds:

- `HBMAllocMem`, `HBMFreeMem`, `HBMCreate`, `HBMDelete`;
- `HBMInit`, which registers `__HBMVersion` (`0x803BA698`);
- `HBMCalc`, `HBMDraw`, `HBMGetSelectBtnNum`, `HBMSetAdjustFlag`, `HBMStartBlackOut`, `HBMIsReassignedControllers`;
- `HBMCreateSound`, `HBMDeleteSound`, `HBMUpdateSound`;
- one more one-argument wrapper at `0x8017C430`.

Its `.data` starts at `0x803BA650` with the version banner, and its `.bss` with `sAllocator` at `0x803E9B90`. `0x8017C470` (`initgx`, 0x1B8 bytes as in xenoblade) starts `HBMBase.cpp`, which runs well past this block.

Both units call `homebutton::HomeButton` members and read its fields, so a complete `HomeButton` header must be reconstructed first. They are left for the following issues.

## F-291-3: HBMCommon.cpp and the HomeButton layout

`RVL_SDK/hbm/HBMCommon.cpp` owns `.text` `0x8017C070`–`0x8017C464` (15 functions), `.data` `0x803BA650`–`0x803BA6A0` (the version banner, `__HBMVersion` at `0x803BA698`, `spAllocator` at `0x803BA69C`) and `.bss` `0x803E9B90`–`0x803E9BA0` (`sAllocator`). The twelve bytes after `HBMCreateInfoEx` are padding before `initgx` (`0x8017C470`), which starts `HBMBase.cpp`. That symbol carries `align:16` so the boundary holds. The banner reads `<< RVL_SDK - HBM 	release build: Mar  3 2009 13:57:36 (0x4201_142) >>`.

This target is the NAND (WiiWare) variant of the library. CC0 Kirby's Epic Yarn (`Swiftshine/key` `7f2a9ab9`, `src/hbm/homebutton/HBMCommon.cpp`) has the same function list in the same order. It gives:

- `HBMAllocMem` and `HBMFreeMem` as C++ functions, matching xenoblade's mangled `HBMAllocMem__FUl`;
- the extra API functions `HBMGetSelectBtnNum`, `HBMStartBlackOut` and `HBMIsReassignedControllers`;
- `HBMCreateInfoEx` (`0x8017C430`), which forwards to `HomeButton::createInfoEx`. Key calls this name a guess. Its `HBMDeleteInfoEx` does not exist in this target. The member and type names `mpHBInfoEx` and `HBMDataInfoEx` come from DWARF data that key and robojumper's SDK reconstruction cite.

`HBMCreateSound` calls `HomeButton::getInstance` and ignores the result, as in key. `getInstance` is out of line here (`0x8017CB30`), unlike TP.

The callees named for linking are the `HomeButton` members `createInstance` (`0x8017CA90`), `deleteInstance` (`0x8017CAE0`), `getInstance` (`0x8017CB30`), `create` (`0x8017CB40`), `init` (`0x8017D5F0`), `calc` (`0x8017E0D0`), `draw` (`0x80180680`), `getSelectBtnNum` (`0x80185990`), `setAdjustFlag` (`0x801859B0`), `startBlackOut` (`0x80185BC0`), `update_sound` (`0x80185DB0`) and `createInfoEx` (`0x80185E00`), plus `nw4hbm::lyt::Layout::mspAllocator` (`0x803E9BF0`). Each name was checked against its body: `getSelectBtnNum` returns `mSelectBtnNum` only in state 18. `update_sound` calls `AxSoundMain` and then each controller's `updateSound`. `startBlackOut` sets `mStartBlackOutFlag` and stops every motor. `createInfoEx` stores its argument at `0x8`, builds the multi-archive accessor and the second layout, and creates seven animation controllers.

`homebutton::HomeButton` is 0x828 bytes. The layout is key's NAND layout plus one change. Like xenoblade, and unlike key, this version keeps `WPADInfo mWpadInfo[4]` inside the object, between `mpText` and `mSimpleSyncCallback`, so every later member sits 0x60 bytes further on. The evidence:

- The constructor (`0x8017C630`) stores `mpHBInfo` at `0x4` and clears `mpHBInfoEx` (`0x8`), `mpLayout` (`0x240`), the second layout (`0x244`) and `mpPaneManager` (`0x268`). It constructs `mDrawInfo` at `0x274` and `mFader(30)` at `0x4E0`, whose colour-enable byte at `0x4EC` starts as 1. It creates the alarms at `0x4F0`, `0x5B0` (stride 0x30) and `0x670`, and sets `iVISetBlackFlag` (`0x6A0`).
- The destructor (`0x8017C740`) frees the members at `0x258`, `0x240`, `0x248`–`0x254` (only when `cursor == 0`), `0x330` (`mAnmNum` entries), `0x488` (15), `0x360` (74), `0x26C`, `0x270`, `0x268` and `0x2C8`–`0x2D4`. It then destroys `mpRemoteSpk` (`0x2D8`), restores the sync callback from `0x214`, and frees `mpLayoutName` (`0xB8`) and `mpAnmName` (`0xBC`).
- `createInfoEx` writes the accessor and links at `0x25C`–`0x264`, the second font at `0x304` and seven controllers at `0x4C4`. `init` (`0x8017DD30`) writes `mAppVolume` at `0x6AC`, the hooks at `0x6B4` and the reverb at `0x6BC`, whose six parameters end at `0x818`. It also writes the aux callback and context at `0x81C` and `0x820`. `startBlackOut` writes `mFadeOutSeTime` at `0x824`.
- `0x1B4` is the address passed to `getInfoAsync` for channel 0, and `0x1C8` is its `battery` byte.

A compile-time size check gives `sizeof(HomeButton) == 0x828`, `sizeof(nw4hbm::lyt::DrawInfo) == 0x54` and `sizeof(AXFX_REVERBHI) == 0x160`. Member names follow key and xenoblade. Several are descriptive because key gives no original name: `mPaneCounterEx[2]` (`0x58`, key's `at_0x058`/`at_0x05c`, only cleared in `init`), `mpLayoutEx` (`0x244`), `mpTextEx[10]` (`0x2DC`), `mpResFontEx` (`0x304`), `mpAnmControllerEx[7]` (`0x4C4`) and the fader's `fadeColorEnable_` (`0xC`). The static string tables, `HBMDataInfoEx`'s fields and the member types not used here are left to the `HBMBase.cpp` reconstruction. `HBMDataInfoEx` is declared but not defined, because key's definition contains a four-byte field it cannot name.

Shared headers gained `AXFX_REVERBHI_EXP` and `AXFX_REVERBHI` (CC0 TP layout) in `revolution/axfx.h`, and the SDK inline `MEMCreateExpHeap` in `revolution/mem/expHeap.h`. The public API prototypes and `HBMDataInfoEx` went into `revolution/hbm/HBMBase.h`. `HomeButton` is in `revolution/hbm/homebutton/HBMBase.h`, and `HBMAllocMem`, `HBMFreeMem` and `spAllocator` are in `revolution/hbm/homebutton/HBMCommon.h`.

On the branch head, `configure.py`, `ninja` and `tools/verify/verify.py verify` exit 0 with the unit `Matching`. objdiff reports all 15 functions and `.text`, `.data` and `.bss` at 100%.

`HBMBase.cpp` remains. It runs from `0x8017C470` to about `0x801862B0`, roughly 40 KB of code, and it covers the rest of this block and nearly all of #292. Key's `HBMBase.cpp` is a close CC0 reference, after the `mWpadInfo` change above.

## Traps

- **Compare chains.** An inline helper returning a `||` of equality tests becomes a bit-mask test (`subi`, `cmplwi`, `slw`, `and.`) instead of the original compare chain. Writing the condition inline, here through `IS_CURSOR_SOUND`, restored the chain and the sizes of four functions.
- **Float constant order.** Float constants are pooled in source order. Writing `10.0f * GainToDecibels(gain)` with an inline helper reorders the pool, and so does an explicit `(f32)log10(gain)`. MSL's inline `log10f`, now in `include/math.h` beside `powf`, gives the original `0.0f`, `10.0f`, `20.0f` order.
- **Parenthesized pointer arithmetic.** `stack + memSize - sizeof(Work)` and `stack + (memSize - sizeof(Work))` compile differently. Only the unparenthesized form matches.
- **Missing `extern "C"` guards.** `revolution/ax.h` and `revolution/ax/AX.h` had none, so C++ callers linked to the mangled `AXInit__Fv`. Both headers now have guards.
- **Include order.** `HBMBase.h` now includes `mem.h` before `kpad.h`, which `OSThreadQueue` requires.
- **Library variant.** This HBM is the NAND build, with `mpHBInfoEx` and a second layout, and it keeps `mWpadInfo` in the object. Neither the DVD references (TP, xenoblade) nor key (NAND, without `mWpadInfo`) give the right offsets alone. Map members against the constructor and destructor before reusing a reference header. Evidence: F-291-3.
