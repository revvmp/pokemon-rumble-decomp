# Anonymous storage and native alignment audit

## OSThread's retained IdleThread

The 808-byte automatic `.bss` range at 0x80453018–0x80453340 contains a source-evidenced unused `OSThread` followed by sixteen bytes of alignment. It is not a single padding block. The typed object's extent is 0x80453018–0x80453330, size 0x318, followed by native alignment to OSStateTM's first 32-byte-aligned buffer at 0x80453340.

The existing OSThread source follows Petari, whose pinned version does not contain this storage. The independently pinned [ogws OSThread.c](https://github.com/doldecomp/ogws/blob/f5a1bb5ed7692a9e09e737bc1236f592274dcf73/src/revolution/OS/OSThread.c) declares `static OSThread IdleThread;` immediately after IdleContext and explicitly retains it with `DECOMP_FORCEACTIVE(OSThread_c, IdleThread)` at the file end. Its [license at the same revision](https://github.com/doldecomp/ogws/blob/f5a1bb5ed7692a9e09e737bc1236f592274dcf73/LICENSE) is CC0-1.0, and the source file contains no separate license exception. The name is recovered from this corroborating SDK source; it is not embedded in this stripped executable.

The reconstructed target OSThread layout is already independently established by DefaultThread and the scheduling routines: its size is 0x318, beginning with the 0x2C8 OSContext. Existing native `.bss` contains DefaultThread at offset zero, RunQueue at 0x318 and IdleContext at 0x418, ending at 0x6E0. Retaining the additional typed declaration emits IdleThread at 0x6E0 and extends the section to 0x9F8. With the original unit base 0x80452938, this gives exactly the object extent above. No byte array, invented fields, binary payload or explicit padding object is introduced.

Three private ordinary-compiler probes distinguish the declaration from retention. A plain unused static declaration is discarded and leaves the section at 0x6E0. Wrapping that declaration with `#pragma force_active on` and `#pragma force_active reset` retains its 0x318 bytes. Placing this controlled declaration beside IdleContext or at the file end produces identical object SHA-256 `3b75f39b747ed97c8d0cbb633bbe6c18e8e69ad1c8bc910aff32a21f5e6ce72f`. The adjacent declaration is retained for readability. All 23 original functions remain byte-identical, every other allocated section is unchanged, and the complete text relocation sequence is unchanged. The object keeps ordinary eight-byte `.bss` alignment; the following OSStateTM object supplies its own thirty-two-byte alignment.

The coordinator's private native link replaces only OSThread's source object and removes the old 808-byte automatic object. Linker and elf2dol both exit 0, the full executable has zero differing bytes, and the frozen SHA-256 passes unchanged. This proves that the compiler and linker naturally restore the surrounding layout. Native BSS allocation has no stored payload, so this acceptance rests on the typed reference declaration, established structure layout, source-object audit and complete relocation/layout result, not on observing zero bytes.

The minimal patch is `tools/local/tail-20261104/vm_bss_idle_ready.patch`. It adds the source declaration, records the local IdleThread symbol at 0x80453018 with size 0x318, extends OSThread's `.bss` split only to 0x80453330, and marks the remaining sixteen bytes as native alignment before OSStateTM. `vm_bss_thread_probe.py` and its JSON report preserve the positive and negative compiler controls. The coordinator installed this patch: the normal ninja build exits 0 and the frozen SHA-256 passes. The 792-byte object is source-owned; its following sixteen bytes are separately excluded as native alignment.

## Existing source alignment accounts for fourteen more gaps

The following fourteen automatic ranges total 256 bytes. Each is exactly the padding required by the next already-compiled object's section alignment. The attributed declarations already exist in accepted source. No source initializer, declaration, compiler flag or object change is needed.

| Section and range | Bytes | Native alignment | Existing declaration responsible |
| --- | ---: | ---: | --- |
| `.bss` 803E6648–803E6660 | 24 | 32 | CRemoteMemory's aligned RemoteDataBlock type |
| `.bss` 8042F9A8–8042F9C0 | 24 | 32 | g3d_state's sTex and sTlut |
| `.bss` 8044E418–8044E420 | 8 | 32 | StrmPlayer::sLoadBuffer |
| `.bss` 80452510–80452520 | 16 | 32 | OS.c's DriveInfo |
| `.bss` 804525D4–804525E0 | 12 | 32 | OSExec's id |
| `.bss` 804528D4–804528E0 | 12 | 32 | OSRtc's Scb |
| `.bss` 804535F8–80453600 | 8 | 32 | OSStateFlags' StateFlags |
| `.bss` 8045D598–8045D5A0 | 8 | 32 | AXVPB's __s_AXPB and __s_AXITD |
| `.bss` 8046ECC8–8046ECE0 | 24 | 32 | NANDCore's s_homeDir |
| `.bss` 8046ED20–8046ED40 | 32 | 64 | NANDLogging's s_message |
| `.data` 803C5744–803C5760 | 28 | 32 | g3d_resfile's named resource records |
| `.data` 803C5924–803C5940 | 28 | 32 | g3d_resanmscn's named resource records |
| `.data` 803DC3D0–803DC3E0 | 16 | 32 | NANDCore's s_currentDir |
| `.sbss` 804B9710–804B9720 | 16 | 32 | dvd.c's __DVDNumTmdBytes |

The responsible declaration need not be the first object in its section: its alignment raises the entire input section's alignment. For example CRemoteMemory starts with an ordinary pointer array, while its later RemoteDataBlock buffers establish 32-byte section alignment. The audit uses the actual native ELF section requirements and object offsets, not just the visible alignment of the first inferred symbol.

The coordinator's `links/native-data-alignments` trial removes all fourteen automatic objects and uses the existing source objects unchanged. Linker and elf2dol exit 0, direct DOL comparison finds zero differing bytes, and the unchanged frozen SHA-256 passes. The proposed `vm_alignment_skips_ready.patch` replaces these automatic objects with explicit skip ranges before their source owners. This excludes 256 alignment bytes from the denominator without adding source credit. The set includes the 112 bytes identified by the earlier inventory and an additional 144 bytes now independently verified. `vm_anonymous_inventory.py` and its JSON report record each neighboring source owner, native alignment and symbol layout.

## Five source corrections account for the remaining ninety-six bytes

| Section and range | Bytes | Source correction |
| --- | ---: | --- |
| `.sdata` 804B88F8–804B8900 | 8 | Thirty-two-byte alignment on billboard_tex0_u8 |
| `.sdata` 804B8908–804B8920 | 24 | Thirty-two-byte alignment on directional_tex0_u8 |
| `.sdata` 804B8928–804B8940 | 24 | Thirty-two-byte alignment on free_tex0_u8 |
| `.data` 803D8608–803D8620 | 24 | Thirty-two-byte alignment on __AXCompressorTable |
| `.bss` 8044DD30–8044DD40 | 16 | Retain the reference SoundSystem overload during compilation; its unused aligned buffer disappears during linking |

All three effect-coordinate tables have the same explicit alignment in the pinned Forecast source: [billboard](https://github.com/doldecomp/forecast/blob/a1f28f8a0d777ebd12953f5c6997ee2decfd07f5/src/nw4r/ef/drawstrategy/ef_drawbillboardstrategy.cpp), [directional](https://github.com/doldecomp/forecast/blob/a1f28f8a0d777ebd12953f5c6997ee2decfd07f5/src/nw4r/ef/drawstrategy/ef_drawdirectionalstrategy.cpp) and [free](https://github.com/doldecomp/forecast/blob/a1f28f8a0d777ebd12953f5c6997ee2decfd07f5/src/nw4r/ef/drawstrategy/ef_drawfreestrategy.cpp), commit a1f28f8a0d777ebd12953f5c6997ee2decfd07f5. Its [pinned license](https://github.com/doldecomp/forecast/blob/a1f28f8a0d777ebd12953f5c6997ee2decfd07f5/LICENSE) is CC0-1.0 and these files contain no separate license exception. The existing declarations had omitted this attribute. All table values, 41 retained functions, allocated contents, section sizes and relocation destinations remain unchanged. The directional object's compiler-generated constant names are renumbered, but their section offsets, sizes, contents and all 229 text relocation identities agree.

The [ogws AXComp.c](https://github.com/doldecomp/ogws/blob/f5a1bb5ed7692a9e09e737bc1236f592274dcf73/src/revolution/AX/AXComp.c) declaration independently confirms thirty-two-byte compressor-table alignment. It shares the CC0-1.0 license checked above, with no separate file exception. Adding the attribute changes only the native `.data` section alignment from eight to thirty-two. The existing const type, 4,032 initialized bytes and data-section placement are unchanged. This is a data-table alignment correction and does not incorporate adjacent DSP executable bytes.

The last gap requires the compiler's input-section behavior, not an invented aligned marker. The already-declared two-int SoundSystem::InitSoundSystem overload is present in [the pinned SS source](https://github.com/zeldaret/ss/blob/787b0b1e9d30e3c1d91ced61350b059ffc5a2411/src/nw4r/snd/snd_SoundSystem.cpp), commit 787b0b1e9d30e3c1d91ced61350b059ffc5a2411, using a thirty-two-byte-aligned defaultSoundSystemWork buffer. The [license at that revision](https://github.com/zeldaret/ss/blob/787b0b1e9d30e3c1d91ced61350b059ffc5a2411/LICENSE) is CC0-1.0; the file credits the older ogws source and has no separate exception. Restoring its ordinary body uses existing stack-size and voice-work constants, initializes SoundSystemParam, and forwards to the retained three-argument overload. No new header declaration or fabricated behavior is needed.

The native compiler gives that input `.bss` section thirty-two-byte alignment. The unused overload's 72-byte body and its 87,520-byte work buffer are then absent from the final linked ELF. The linker nevertheless preserves the input section's alignment, placing the retained destructor registration node at 0x8044DD40 and the 800-byte sTaskThread at 0x8044DD50. All six retained functions remain raw-byte-identical. This is the missing source cause of the sixteen-byte boundary gap.

The combined `links/all-native-data-alignments` trial uses these five ordinary compiled source objects, removes all nineteen automatic alignment objects, and passes the unchanged frozen SHA-256 with zero differing bytes. Linker and elf2dol exit 0. The total excluded alignment is 352 bytes; the separate sixteen bytes after IdleThread were handled with that typed object. The combined ready packet is `vm_all_alignments_ready.patch`, superseding the fourteen-range-only packet. It contains five source corrections and nineteen skip ranges, with no byte arrays, padding objects, object edits or extra source-credit claim for the discarded overload. Native compiler audits are recorded by `vm_remaining_alignment_probe.py` and its JSON report. The coordinator installed the complete packet, and the normal ninja build exits 0 with the frozen SHA-256 passing unchanged.

## Final controller-table alignment object

ControllerTable's one-byte sExtensionControllerEnabled ends at 0x804B77D9. The next source `.sdata`, StageInfo, already has native alignment eight and size seventy-one; it starts at 0x804B77E0. Its unchanged source object therefore supplies exactly the seven-byte gap. The standalone ControllerTableAlignmentPadding object is unnecessary.

The private native link omits that object and passes the unchanged frozen SHA-256. The coordinator then removes its split-only unit and assigns the range 0x804B77D9–0x804B77E0 as `align:1 skip` before StageInfo. Keeping explicit one-byte range alignment avoids the previous dtk failure at this odd start address. The normal ninja build exits 0 and the frozen verification passes. No source object changes and no seven-byte source-credit claim are made. `vm_controller_alignment_ready.patch` records this configuration-only correction.

All formerly anonymous automatic storage and alignment ranges in the audited inventory are now accounted for: 792 bytes of real IdleThread storage and 375 bytes of native alignment, comprising sixteen bytes after IdleThread, 352 bytes across nineteen ranges, and the controller's seven bytes. The embedded system-message U8 archive and the DSP instruction image remain separate original-data dependencies; this audit does not classify either as source reconstruction.

## Traps

- An automatic BSS gap with no references can still contain retained unused SDK storage. The 808-byte range after IdleContext resolves to the independently documented IdleThread plus sixteen alignment bytes. Check matching SDK declarations and native layouts before treating an entire anonymous range as padding or inventing a zero array.
- A discarded variable can still determine its input section's alignment. SoundSystem's unused overload and default work buffer vanish from the linked ELF, while the buffer's thirty-two-byte alignment survives and positions the retained registration node correctly. Omitting the unused source body lost that requirement and left an apparent sixteen-byte fallback gap.
