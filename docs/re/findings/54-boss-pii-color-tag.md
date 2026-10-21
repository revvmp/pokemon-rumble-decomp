# F-54-1: Boss and color-button tag processor boundaries

Target: `wpse-v1`, immutable original DOL and frozen manifest. The issue's survey boundary divides two source units. `CBossPii` begins at `0x8006BEE0` and ends after its secondary-base destructor thunk at `0x8006C7A8`. Its RTTI, two shared-pointer control types, and default-deleter names occupy the preceding data cluster. The class extends `CEnemyPii`, whose constructor, destructor and initialization body begin at `0x800A47E0`, `0x800A4890` and `0x800A48F0` in #115.

The next retained function, `0x8006C7B0`, installs the singleton `CColorBtnTagProc` on an NW4R TextBox after checking its runtime type. `Pane::GetRuntimeTypeInfo` at `0x8006C860` is the following weak helper. The class's remaining functions lie in neighboring #55. Its RTTI supplies the exact spelling `CColorBtnTagProc`; the descriptive source filename follows that class name. First-function ownership assigns the complete unit to #54. No #115 split is taken.

| Section | Complete color-tag unit extent |
| --- | --- |
| Text | `0x8006C7B0` to `0x8006CE2C` |
| Exception tables | `0x8000AF60` to `0x8000B030` |
| Exception index | `0x80044F40` to `0x80044F70` |
| Constructor entry | `0x803529C4` to `0x803529C8`, naming `0x8006CD00` |
| Data | `0x8039A960` to `0x8039A9E8` |
| Small data | `0x804B5C90` to `0x804B5CA0` |
| BSS | `0x803E6580` to `0x803E65E0` |

The first split checkpoint used BSS `0x803E6590` to `0x803E65EC`. That hypothesis is superseded by the initializer's exact address arithmetic. Its pool base is `0x803E6580`: the first 12-byte registration record precedes four padding bytes and the 16-byte processor at `0x803E6590`. The next record is at `0x803E65A0`, followed by five colors at `0x803E65AC`; the final record is at `0x803E65C0`, followed by five colors at `0x803E65CC`. The unit ends at `0x803E65E0`. The original symbol extent at that next address is restored. The data contains the Process jump table, class vtable, class and base RTTI names, and its base list. The next literal, `PIINAME_NORMAL`, begins the following unit.

With only the complete split and that owned BSS symbol correction, configure/map, default Ninja after deleting the prior DOL, and frozen-manifest verification each exit 0. The original split object is selected. No reconstructed-source credit or issue completion is claimed by this checkpoint.

# F-54-2: Complete color-tag source acceptance

`game/CColorBtnTagProc.cpp` is reconstructed as ordinary C++ with the established Wii/1.1 game flags. No reference implementation, executable bytes, inline assembly or compiler override is imported. The complete 16-byte class extends the canonical `nw4r::ut::TagProcessorBase<wchar_t>`. Its saved gradation, start color and end color occupy offsets 4, 8 and 12. `Process`, `CalcRect` and the destructor retain the inherited SDK virtual interface and order. The class spelling is original RTTI evidence. `Attach`, `SetColor`, static-storage and member spellings are descriptive, with medium confidence.

Three additive `CharWriter` accessors expose the already established start/end colors and gradation mode without changing layout. Their names are inferred. Both agents working on NW4R/font headers confirmed no conflicting change. Compiling each of the four rebuilt existing consumers with the original header and the candidate header preserves all allocated section payloads/metadata and normalized relocation targets: `ut_TagProcessorBase`, `ut_CharWriter`, `db_DbgPrintBase`, and `lyt_textBox`. The outside receipt is `54-reference/54-charwriter-control-proof.json`.

The processor consumes a tag after control character 1. Tag 1 restores the saved colors and then gradation. Tag 2 saves that state, enables vertical gradation, and peeks the next glyph without consuming it. Glyphs `E103`, `E104`, `E109` and `E10C` select color pairs 0 through 3; `E102`, `E105`, `E106`, `E107`, `E108`, `E10A` and `E10B` select pair 4. An unknown glyph still completes the tag operation; an unknown tag delegates to the SDK processor after consuming the tag. Rectangle calculation consumes tags 1 and 2 without drawing and otherwise delegates. Attachment checks the pane and SDK runtime type before installing the singleton. The color setter preserves the native unchecked array index.

| Native function | Address | Instruction bytes |
| --- | --- | --- |
| `Attach` | `0x8006C7B0` | 168 |
| `Pane::GetRuntimeTypeInfo` | `0x8006C860` | 8 |
| Destructor | `0x8006C870` | 88 |
| `Process` | `0x8006C8D0` | 904 |
| `CalcRect` | `0x8006CC60` | 52 |
| `SetColor` | `0x8006CCA0` | 96 |
| Static initializer | `0x8006CD00` | 216 |
| `Color` constructor | `0x8006CDE0` | 12 |
| Start-color array destructor | `0x8006CDF0` | 28 |
| End-color array destructor | `0x8006CE10` | 28 |

All ten retained functions, 1,600 instruction bytes, compare at 100%. With the unit source-selected, configure/map, default Ninja after deleting the previous DOL, and the explicit frozen verifier exit 0. The DOL is byte-identical with SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. The independent outside `54-source-audit.py` additionally verifies every function's actual ELF address/size and raw resolved bytes, source ownership in the final map, all native exception/index/constructor/data/small-data bytes, the three BSS globals and complete 96-byte BSS contribution, and selection of the compiled object without its original fallback.

The source object also emits the already existing weak `Color` destructor. The map reports an unreferenced duplicate; the actual definition stays at `0x8005C9A0` in `BWorld.o`. Its extra source-object exception record and index entry are discarded. The retained unit contributes exactly 208 exception bytes and 48 index bytes at their native addresses. The source's 132 data bytes match the native payload; four linker padding bytes complete the 136-byte native range. The BSS has 92 storage bytes plus four internal alignment bytes. No fabricated padding or credit for the outside destructor is added.

The first two accepted functions account for 176 instruction bytes in #54. The remaining eight account for 1,424 bytes in #55. The twenty `CBossPii` functions, 2,108 instruction bytes, remain original fallback dependencies, so this partial delivery does not complete #54. The distinct `CColorConst` unit remains with #55's owner.

## Traps

The issue's last two functions belong to the next class's unit. Preserve that class's initializer, registration records and neighboring #55 methods together; the survey issue boundary is not a source-file boundary.

- A decompiler's returned address can be incidental. Attachment is a `void` operation: forcing a pointer return adds a branch and explicit null result. The ordinary void method reproduces every native instruction.
- The setter's color parameters are values. Reference parameters retain the same ABI registers but schedule their byte copies differently; by-value `Color` parameters reproduce all 96 native bytes.
- A successful original-only split cannot prove global ownership. The first BSS estimate still verified because the original bytes remained linked. The initializer's base/addend arithmetic establishes the registration record before the singleton and the true end after the second color array.
