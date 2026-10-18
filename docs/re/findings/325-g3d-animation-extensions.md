# F-325-1: Game G3D animation extensions and rendering helpers

Issue #325 inventories 37 functions and 5,348 native instruction bytes at `0x801C5810`–`0x801C6DAC`. Its native class names come from CodeWarrior RTTI: `nw4r::g3d::AnmObjChrGroup`, `AnmObjChrOverride`, and `AnmObjChrRootScale`. The four earlier rendering helper names are descriptive. This investigation does not take the independent #326 unit beginning at `0x801C6DB0`.

The unchanged isolated worktree completes configure, a fresh 542-target plain Ninja build, and explicit frozen executable verification with exit zero. A second fresh split-only build and explicit frozen verification also exit zero. All 535 pre-existing compiled source objects remain byte-identical as complete files, proved by independent SHA-256 comparison against the preserved unchanged baseline. The evidence below initially describes individual compiler diagnostics; source-selected acceptance remains a separate required gate.

## Native translation units

| Unit | Text range | Entries | Native instruction bytes |
| --- | --- | --- | --- |
| Rendering helpers | `0x801C5810`–`0x801C5B48` | 4 | 812 |
| AnmObjChrGroup | `0x801C5B50`–`0x801C67EC` | 21 | 3,104 |
| AnmObjChrOverride | `0x801C67F0`–`0x801C6A7C` | 6 | 636 |
| AnmObjChrRootScale | `0x801C6A80`–`0x801C6DAC` | 6 | 796 |

Separate ordinary source files preserve the native out-of-line calls from the derived classes to Group. Combining them under established game `-ipa file` changes constructor, destructor and helper inlining. RootScale has an ordinary out-of-line destructor before `GetResult`; declaring that destructor inline would defer its weak definition until after the RTTI helpers and reproduce the wrong physical function order. Override's inline empty destructor is emitted after its RTTI helpers, matching its different native order. No per-function compiler pragma or flag override is used.

The classes use Wii/1.1 and unchanged `cflags_game`, including game exceptions and CodeWarrior RTTI. Their namespace does not make them part of the separately compiled NW4R library. The canonical `AnmObjChr` declaration, its allocator, resource interfaces and inherited virtual order remain unchanged. Child traversal and binding algorithms adapt the already reconstructed project's canonical `g3d_anmchr.cpp` node algorithms, whose pinned CC0 reference provenance is recorded in F-399. Target-specific root scaling, generic child types, allocation, fields and virtual entries are independently reconstructed from this executable.

## Complete class interfaces

`AnmObjChrGroup` is `0x34` bytes. Its inherited `AnmObjChr` occupies `0x18` bytes, followed by `ResMdl` at `0x18`, signed child capacity at `0x1C`, generic `AnmObjChr**` child storage at `0x20`, root-scale flags at `0x24`, and a `math::VEC3` at `0x28`. The native vtable retains the canonical resource-only attachment stubs and appends generic child attachment, detachment and detach-all entries at `0x58`, `0x5C` and `0x60`. These three new method names and the root-scale helper names are descriptive, with high behavior/layout confidence and medium spelling confidence.

Override adds no storage. Its constructor allocates the `0x34`-byte object, aligned node-binding entries and an array of generic child pointers in one allocator block. `GetResult` visits children in reverse order and returns the first result carrying `FLAG_ANM_EXISTS`; absence clears the caller's result flags.

RootScale is `0x38` bytes. Its one child pointer is a real member at `0x34`, passed by address to the Group constructor with capacity one. Its allocator therefore begins binding storage at `0x38`; there is no invented padding or separately allocated child array. Non-root nodes forward the child's result directly. Root node zero copies a distinct returned result into the caller's buffer before applying the Group scale. Without a child, it clears the result flags and applies the model's root-node patch and scale.

The Group scale setter recognizes identity and uniform scales using the canonical `ChrAnmResult` flags. Scaling an existing animation modifies its three scale components and clears the appropriate identity/uniform flags. Without animation, the helper fills the root scale and uses `ResNode::PatchChrAnmResult` for the remaining root-node state. An ordinary nested condition preserves the common return path and exact native stream; an early identity-scale return introduces another epilogue and eight extra bytes.

## Strict class diagnostic proof

The external `325-audit-classes.py` audit verifies all 33 class functions, 4,536 instruction bytes and 86 independently paired text relocation targets. It also compares 710 initialized bytes and all 96 data relocation targets: native NW4R type-name records, vtables, CodeWarrior RTTI names/base chains, RTTI handles and Group float constants. Every one of the 23 retained exception records and indices matches: 480 combined bytes and 47 independently paired relocation targets. Compiler and audit commands exit zero. The same proof passes after compiling the actual three worktree source files using only their tracked canonical include tree, without an external interface overlay.

Native Group data ends after its actual final RTTI string at `0x803BFF82`, leaving alignment to Override at `0x803BFF88`. Its NW4R type-name record is `0x14` bytes at `0x80366FD8`; the survey's `0x18`-byte symbol included the four bytes before the next aligned type-name record. The corrected owned size follows the compiler's actual string payload and target bytes. Override and RootScale own only their newly retained RTTI chains; redundant emitted base-class RTTI definitions are not additional source credit.

## Rendering diagnostic and remaining work

The vertex-color setup, overlay-state setup and screen quad each reproduce their entire native stream under Wii/1.1. Their SDK calls and data references still require the final actual-source relocation/link acceptance. The projection helper has the exact `0xF0` size but remains NonMatching because its ordinary width/height conversion and argument scheduling differ. Wii/1.0 and Wii/1.3 do not improve it. Raw SDK matrices and canonical math matrix wrappers also retain the discrepancy; no flag override is justified by these measurements.

The quad takes `nw4r::ut::Color` by value. Native callers at `0x800BA98C`, `0x801E4DEC` and `0x801E4E10` pass separate caller-owned copies; the latter two make distinct word copies of the same local RGBA color before each call. The ordinary by-value signature reproduces the native repeated packed-color loads and exact `0xF8` body. A const-reference signature incorrectly allows one color load to be reused and omits twelve bytes. No volatile qualifier is needed or supported.

## F-325-2: Source-selected class acceptance

After the verified split-only gate, all four units are registered NonMatching and compiled through plain Ninja. Configure, the nine-command incremental Ninja pipeline, its real objdiff report and explicit frozen executable verification all exit zero. All 33 retained class functions score 100 percent. The strict diagnostic repeats successfully against the actual Ninja-built objects, including every retained data and exception relocation.

Only the three complete class registrations then become Matching. Configure, plain Ninja and explicit frozen executable verification exit zero again. The actual link selects each compiled class source object once and selects none of their corresponding fallback objects. All 33 final function addresses and all 4,536 relocated instruction bytes equal the native executable. The linked audit also compares all 710 initialized bytes and all 480 retained exception/index bytes at their actual native addresses. The unretained extra `AnmObj` destructor is absent from this range. All 535 original compiled source objects remain identical complete files; the candidate adds only the four owned compiler objects, with rendering still using its fallback.

Raw Override/RootScale `.data` each score approximately 69 percent because the compiler also emits duplicate base-class RTTI definitions and their strings. The linker discards those redundant weak definitions, leaving each exact original `0xA8`-byte retained data extent. Group's extra unretained destructor similarly adds raw exception bytes and an index; those records are discarded. No claim of universal raw-section equivalence is made. Explicit retained relocation/data/table proof and the exact source-selected final ELF/DOL establish the accepted native coverage.

This accepts 33 of the original 37 functions / 4,536 of 5,348 native instruction bytes. The four-function rendering unit remains NonMatching, receives no linked source-byte credit, and prevents issue closure. Its projection helper still differs under the additional isolated `-O4,s` measurement, so the production flags remain unchanged. The final latest-main rebase and exact committed-head merge gate remain required before integration.

## Traps

An instruction-only search can incorrectly pair identical destructors or RTTI helper bodies with another class. The class audit instead pairs the full ordered inventory, validates every relocated target, and compares the class-specific vtables and type-name data. Raw masked comparison remains diagnostic only.

The Group compiler object also emits an unretained weak `AnmObj` destructor and its exception metadata. It is excluded from the 33-function inventory. Its extra raw object bytes do not justify claiming another native function or whole-section equivalence; the actual source-selected final ELF proves that it is discarded.

The canonical allocator spelling matters. Using `ut::RoundUp` and one typed `u8*` allocation result reproduces both derived allocation routines, whereas a redundant `void*` alias changes several register assignments. Include `ut_algorithm.h` before the G3D headers and undefine the SDK `RoundUp` macro before qualified template calls, as already established in F-399. Do not invent replacement rounding helpers or compiler settings.
