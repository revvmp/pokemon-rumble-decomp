# F-803-2: Complete sound Voice source

Target `wpse-v1`. The complete Voice translation unit starts at `0x8028DBB0` and ends at `0x80290414`, retaining every original #803 entry: 46 functions / 10,160 native bytes. Its retained data are the 0x24-byte pan-curve jump table and 0x14-byte Voice virtual table at `0x803C6880`-`0x803C68B8`, and thirteen four-byte literals at `0x804BB028`-`0x804BB05C`. It owns no BSS, constructor table or exception sections.

## Source and target revision

The readable class implementation follows CC0 zeldaret/ss `787b0b1e9d30e3c1d91ced61350b059ffc5a2411`, source blob `dd19828c345e64094f76677ddaee00208076dc26`, whose provenance identifies kiwi515/ogws. The canonical target Voice layout and direct ChannelParam fields are established separately in [F-804-1](804-voice-util-layout.md) and [F-803-1](803-voice-headers.md), with full unchanged-source caller proof. Source uses existing canonical flag, callback, list and pan-info names and the actual nested file AdpcmInfo type where appropriate. It adds no raw executable bytes, assembly, synthetic views or padding members.

The original Setup passes immediate SRC_4TAP_AUTO (5), rather than the reference revision's AxManager getter. The cutoff local remains int until its u16 call argument is narrowed, matching the canonical Util return type and the original mask placement. Original CalcMixParam conditionally writes each remote-volume slot when its output bit is set; the later reference adds an else-zero branch that is absent from this target. Preserving the original conditional assignments restores the exact 0xB88-byte body. No new initialization behavior is introduced.

The former placeholder at `0x8028F730` is CalcMixParam, established by its unique native body, typed arguments, call from CalcAxMix and the pinned reference. The last literal at `0x804BB058` is a four-byte float; its former eight-byte survey extent included following padding. Typed original SDA consumers and exact referenced values establish all thirteen compiler-local literal correspondences. Compiler-local numeric names and the local jump-table name describe reconstruction correspondence, without semantic-original-name claims.

## Native and artifact acceptance

The unchanged baseline full default build and explicit frozen verifier pass. After splitting all three retained sections and correcting only owned metadata, the split-only full default Ninja and explicit verifier exit 0. The source is first compiled NonMatching through all_source, using unchanged evidenced Wii/1.1 cflags_nw4r. All 46 functions, .text, .data and .sdata2 score 100% in objdiff. Normal Matching selection then passes the complete default Ninja and explicit actual-output frozen verifier.

The independent native audit confirms all 46 extents and non-relocated words, every one of 282 paired code relocation offsets/types/destinations/addends, all twelve paired jump-table/vtable relocations, all 56 raw retained table bytes, all 52 raw literal bytes and their referenced values, and every final function/data address and extent. The real link rule selects source and excludes the original Voice object. The nine named class float objects are confirmed stripped; their consumed literals remain at the original addresses. Other unused out-of-line helpers strip normally.

The complete rebuilt DOL directly equals all 4,094,880 frozen reference bytes, SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. The final combined #803/#804 source PR retains separate original issue inventories and counts code/data once. Exact committed-head configure/full Ninja/frozen gates repeat after the shared header prerequisite merges and the final latest-main rebase. Scope covers main.dol only, excluding boot loader, other contents and WAD packaging.

## Traps

- A scratch include context can assign numeric compiler locals differently from the actual repository compilation. Objdiff can pair these by value, but the strict native relocation audit still exposes differing spellings. Pair the actual compiled source labels using typed original operands and exact referenced bytes; never alter source instructions or frozen expectations to hide the difference.
- The later reference's remote-volume else-zero assignments add 36 bytes and shift the whole mixer body. The target's conditional stores are direct instruction evidence; preserve that release behavior rather than importing the later initialization change.
- A native float at a unit end can have a survey extent that includes padding. The actual four-byte source object and typed original consumer establish its size; stop the split at the object end and verify the complete DOL.
