# F-804-2: Complete sound Util source

Target `wpse-v1`. The complete `snd_Util.cpp` unit starts with CalcPitchRatio at `0x80290BC0` in #804 and ends after GetDataRefAddressImpl at `0x80291094` in #443. It owns eight retained functions / 1,168 native bytes. The seven later functions belong to the neighboring #443 inventory; source-unit credit is counted once. VoiceManager's eleven original #804 functions remain a separate source gate.

## Boundaries and source

| Section | Original extent | Accepted content |
| --- | --- | --- |
| .text | `0x80290BC0`-`0x80291094` | Eight named Util methods |
| .rodata | `0x8036E178`-`0x803705F8` | 9,344 bytes of reference lookup tables |
| .data | `0x803C68B8`-`0x803C68C4` | Three pan-curve pointers |
| .sdata | `0x804B89F0`-`0x804B89F4` | The four-byte local random state |
| .sdata2 | `0x804BB060`-`0x804BB090` | Twelve consumed floating constants, 48 bytes |

The code and editable tables follow CC0 zeldaret/ss commit `787b0b1e9d30e3c1d91ced61350b059ffc5a2411`, `src/nw4r/snd/snd_Util.cpp`, retaining the existing canonical class names/types and F-804-1's additional low-pass declarations. The reference identifies kiwi515/ogws as its original source. Constants use the existing canonical aliases, PanInfo uses the existing flag names, and CalcLpfFreq retains the canonical int return type; the generated target instructions agree exactly. Debug macros and comments are omitted. No original executable bytes are used as implementation data.

All eight reference lookup arrays have unique complete byte-identical occurrences at the listed target addresses. The imported SinCos placeholder had divided one 0x404-byte array at `0x8036F974`; the proven complete array replaces that internal survey alias. The pan-pointer and random-state placeholders had included following alignment padding; their native extents are 12 and four bytes. Splits stop at the actual objects' ends and leave following padding to its own allocation.

## Native and artifact acceptance

The split-only full default build and explicit frozen verifier exit 0. The source is registered NonMatching and compiled through all_source. The unit uses unchanged Wii/1.1 cflags_nw4r. Every retained function and all five code/data sections score 100% in objdiff. Twelve original literal slots are paired to the actual compiler-local labels using each original typed SDA operand and exact referenced bytes, preserving all original addresses and extents. This corrects object correspondence; it changes no source instruction or frozen expectation and claims no original semantic names for numeric compiler locals.

Normal Matching selection passes the full default Ninja and explicit frozen verifier. The independent audit confirms all eight source-native function extents and non-relocated words; all 42 paired code relocations, including exact literal destinations/values and named lookup targets; all three pan-pointer relocations; the complete 9,360 raw rodata/data/sdata bytes; table final ELF addresses; and actual source selection without the original Util object. Full DOL identity covers all 48 resolved small-data constants and every resolved instruction operand. The source-only named CALC_LPF_FREQ_INTERCEPT object is confirmed absent from the final ELF; its consumed literal remains at the original address.

The actual rebuilt DOL directly equals the frozen reference: 4,094,880 bytes, SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. Scope remains the main game executable, excluding boot loader, other package contents and WAD packaging. The final pull request records the exact committed-head gate after its last latest-main rebase and after the shared-header prerequisite merges.

## Traps

- Unpaired literal labels can lower objdiff's text score despite identical native words. Here every discrepancy was an SDA relocation to an unpaired survey label. The original typed consumer and exact referenced value establish each pairing; raw relocation and complete frozen-artifact checks remain required.
- A named unused constant shifts prelink source-local offsets. The compiler emits the class intercept object before its unnamed literals, then the linker strips that unused object. Compare the paired destinations and final artifact rather than assuming source-local offsets equal target offsets.
