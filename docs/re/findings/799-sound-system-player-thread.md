# Issue 799: sound handles, players, system, thread and stream pool

## F-799-1: Complete retained inventory and source verification

The original issue lists 41 functions and 6,316 code bytes in `wpse-v1/main.dol`. All 41 belong to six source units, whose first retained functions are owned by this claim. The SoundHeap filename in the survey does not identify an additional retained translation unit in this span.

| Unit | Retained functions | Native code bytes | Paired text relocations | Text split |
| --- | --- | --- | --- | --- |
| `snd_SoundHandle.cpp` | 3 | 232 | 4 | `0x80287C70–0x80287D60` |
| `snd_SoundPlayer.cpp` | 18 | 2820 | 135 | `0x80287D60–0x802888D0` |
| `snd_SoundStartable.cpp` | 1 | 96 | 1 | `0x802888D0–0x80288930` |
| `snd_SoundSystem.cpp` | 6 | 868 | 93 | `0x80288930–0x80288CB8` |
| `snd_SoundThread.cpp` | 9 | 1744 | 99 | `0x80288CC0–0x802893C0` |
| `snd_StrmChannel.cpp` | 4 | 556 | 13 | `0x802893C0–0x80289600` |

These 345 instruction relocations are paired through original and source objects. Every allocated section scores 100% in objdiff. The final ELF retains each of the 41 original function addresses and sizes; the map and actual Ninja link inputs select all six compiled source objects and exclude their fallback objects. The resulting DOL is byte-identical to the immutable original and passes the unchanged frozen manifest. No original byte arrays, assembly replacement bodies, new flags, unit tests, comments or docstrings were introduced.

The unmodified baseline at `d67eaade82efee6f7d7132892c8d3c2b82fe270e` passed configure, plain Ninja and explicit frozen verification. Each unit was split and verified with its original object selected before source selection, compiled as NonMatching, compared, then selected as Matching and linked through plain Ninja. The explicit verifier ran only after successful Ninja exit status. The map-enabled actual output is `build/wpse-v1/main.dol`. Commands: `python configure.py --map`, `.venv/Scripts/ninja`, and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1`, all exit 0. Exact committed-head and latest-main acceptance follow before merge; this reconstruction evidence alone does not replace that gate.

SoundPlayer owns the static priority-list registration node at `0x8044D720` (12 bytes), 128 twelve-byte lists at `0x8044D730` (1,536 bytes), guard at `0x804B9378` (one byte), and two floats at `0x804BAFE8`/`0x804BAFEC` (1.0 and 0.0). Its BSS ends at `0x8044DD30`, excluding the next object's alignment. Guard split alignment requires the surrounding eight-byte slot, while the symbol retains its measured one-byte extent.

SoundSystem owns the original 70-byte version string at `0x803C6788`, pointer at `0x804B89E8` (four bytes), registration node at `0x8044DD40` (12 bytes), TaskThread at `0x8044DD50` (800 bytes), initialized flag at `0x804B9380` (one byte), maximum voice count at `0x804B9384` (four bytes), and constructor entry at `0x80352A60`. Its source emits the exact original Sep 24 2009 16:54:03 banner, rather than the reference project's 2011 banner. The data split includes two alignment bytes after the 70-byte string; the source declares only the string. Native ShutdownSoundSystem calls RemoteSpeakerManager::Shutdown after TaskThread::Destroy, a call absent from the SS reference; restoring that evidenced operation reproduces the complete 116-byte function.

SoundThread owns its registration node at `0x8044E070` (12 bytes), singleton at `0x8044E080` (920 bytes), and one-byte guard at `0x804B9388`. Its measured BSS ends at `0x8044E418`. The existing mutex remains at offset 0x354. Native flag accesses initialize the create/pause bytes at 0x394/0x395; the latter suppresses AX frame messages while preserving voice callbacks. FrameProcess is a normal private member, with all nine retained function bodies exact. The reference's newer disk-error guard is absent from this target.

StrmBufferPool is 24 bytes: buffer pointer 0, buffer size 4, block size 8, block count 0xC, allocation count 0x10, and four flag bytes 0x14. Setup clears exactly four flag bytes and caps the pool at 32 blocks. The adjacent StrmChannel record is the canonical 0x34-byte declaration approved by issue 802's owner; it includes snd_Types directly for AdpcmParam. All four pool methods and their original caller identities are proved.

## F-799-2: Provenance and canonical header coordination

SoundPlayer, SoundStartable and SoundSystem use the inspected CC0 source from [zeldaret/ss](https://github.com/zeldaret/ss/tree/787b0b1e9d30e3c1d91ced61350b059ffc5a2411/src/nw4r/snd), commit `787b0b1e9d30e3c1d91ced61350b059ffc5a2411`, with the corresponding headers. SoundHandle, SoundThread and StrmBufferPool use [kiwi515/ogws](https://github.com/kiwi515/ogws/tree/f5a1bb5ed7692a9e09e737bc1236f592274dcf73/src/nw4r/snd), commit `f5a1bb5ed7692a9e09e737bc1236f592274dcf73`, also CC0. File notices and both root licenses were inspected. Separate manifests outside the checkout retain fetched URLs, commits, byte counts and SHA-256 hashes. Xenoblade commit `ea25691b2029394ab777bda7060139e517a13cba` was inspected as another CC0 reference but its relevant bodies reproduce the older OGWS revision.

Adaptations use canonical LinkList iterators, release-mode assertion removal, actual OS message macros and original target behavior. SoundHandle's temporary attach omits the reference's later temporary-special-handle detachment, which the native target does not perform. No compiler-forcing reference macros or fake constant-order functions were retained.

Task and TaskManager declarations come unchanged from merged header prerequisite 844. Voice and VoiceManager declarations are approved exact copies of the issue 803/804 owner's prerequisite 835. TaskThread is an approved exact temporary copy from the issue 802 owner's PR 851. These shared prerequisites retain their owners; copies are deduplicated against canonical main before merging the source. SoundThread gains only its owned private FrameProcess declaration, and SoundSystem and StrmBufferPool receive complete evidenced headers. The existing SoundPlayer and SoundStartable layouts are unchanged.

## F-799-3: Preserve the existing temporary-handle caller

After external Sound3DActor source PR858 landed, the first latest-main integration probe caught a one-byte DOL change at0x802824FB. Its call instruction at0x802824F8 must retain target0x80287C70, the temporary attachment implementation. The inline ForEachSound caller in the existing canonical snd_ExternalSoundPlayer.h used detail_AttachSound, which had previously been imported under the incorrect addressC70. Correcting the owned SoundHandle names makes that unchanged spelling resolve to the general attachment atC0.

The coordinated repair changes only that existing inline call to the already-declared detail_AttachSoundAsTempHandle. It is atomic with the two owned symbol-name corrections. No API signature, class storage, outside source implementation, split or source-credit count changes. Source inspection finds the existing Sound3DActor destructor as the current ForEachSound consumer. The separate798 investigation independently isolated the exact native target and the header call; final combined acceptance requires all affected existing native code and the complete source-linked frozen DOL unchanged.

## Traps

- A cached directory name does not establish source provenance. An older issue's reference cache contained OGWS-compatible SoundPlayer despite its local naming, producing apparent layout conflicts. Fetching and hashing the actual pinned SS URL recovered the matching target revision. Verify the fetched commit and content before adapting a cached source.
- PowerPC halfword relocations can begin two bytes into an instruction. A scratch comparator that masked only the raw relocation offset falsely reported differences in GetInstance and SoundThreadFunc. Mask the containing instruction with `(offset - functionStart) // 4 * 4`; retain whole-object, relocation-target and final DOL checks as acceptance gates.
- Measured data-symbol ends need not be legal automatic split starts. dtk rejects the one-byte guard boundary at 0x804B9379 and the 70-byte banner boundary at 0x803C67CE. Keep actual symbol sizes, split at the required section alignment, and verify the linked padding against the original. The BSS extents exclude the next unit's trailing alignment.

- An imported reference method name can hide a different target revision. Propagating an evidenced owned rename requires checking existing inline callers: the temporary ForEachSound handle must keep its original C70 target, rather than follow the corrected general-attachment name to C0. See F-799-3.
