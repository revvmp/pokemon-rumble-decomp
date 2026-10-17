# F-444-3: Complete WsdPlayer source

Target `wpse-v1`. The owned WsdPlayer unit retains 25 functions / 4,032 native bytes at `0x80291BF0`-`0x80292C68`, its 0x60-byte virtual table at `0x803C6900`-`0x803C6960`, and 0x28 bytes of floating constants at `0x804BB098`-`0x804BB0C0`. It follows the pinned CC0 zeldaret/ss reference used in F-444-1, adapted to the canonical target interfaces and the prerequisite accessors from F-444-2. No original executable bytes implement the source.

## Layout and virtual dispatch

The complete WsdPlayer class is 0x118 bytes. Its DisposeCallback base vptr is at 0xB4 and its SoundThread::PlayerCallback base vptr is at 0xC0. The active flag starts at 0xCC, LfoParam is at 0xF8 and Channel pointer at 0x114. Original constructor stores and typed member accesses establish the layout. The original virtual table, anchored uniquely by Start/Stop/Pause, establishes the six adjustment thunks and the addresses of all tiny virtual methods. This avoids assigning repeated short instruction streams by similarity alone.

The WsdPlayer destructor body is at `0x80291490`, outside this unit in the preceding WaveSound file owned by #443. This unit emits its weak duplicate, which resolves to that original location; it does not take the neighboring split. OnUpdateVoiceSoundThread likewise resolves to the previously registered shared SoundThread::PlayerCallback definition. Source-only base virtual tables strip or resolve to existing definitions. The table contains 18 relocation entries, including these inherited/adjusted targets.

## Acceptance

Split-only full default Ninja and explicit frozen verification exit 0. NonMatching all_source compilation succeeds with unchanged Wii/1.1 cflags_nw4r. Every retained function, .text, .data and .sdata2 scores 100% in objdiff. Normal Matching selection then passes the complete default Ninja and explicit actual-output frozen verifier.

The independent audit confirms all 25 native function extents, every non-relocated instruction word, all 188 paired code relocations with exact offset/type/destination/addend, every final function address, all 96 raw vtable bytes and 18 paired vtable relocations, and all 40 raw literal-section bytes. Typed original SDA operands and exact values establish each constant's correspondence; numeric compiler-local names claim no semantic original names. The real link rule selects source and excludes the original WsdPlayer object. The complete rebuilt DOL directly equals all 4,094,880 reference bytes, SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.

This checkpoint supplies 30 functions together with the four WsdFile methods and ADPCM decoder. Full #444 acceptance still requires its WaveSoundHandle entry and six preceding WaveSound tail methods supplied by the owned #443 file. The final PR records the committed-head gate after its final latest-main rebase. Scope covers main.dol only.

## Traps

- Identical tiny methods and adjustment thunks have several possible instruction matches. Here original vtable entry order, source relocation identity and exact original addresses disambiguate them before acceptance.
- Weak inline destructor bodies can first be retained in the preceding source file. Keep the evidenced original address and owning split; source-native comparison and full resolved artifact identity must both pass.
- Added canonical inline declarations change numeric compiler-local counters in otherwise identical source objects. Pair the actual compiled literals through typed original SDA operands, native extents and complete raw values, then repeat the strict relocation and full-artifact audits. Do not reuse counters from a previous header context.
