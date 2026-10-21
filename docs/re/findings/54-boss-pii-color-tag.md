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
| BSS | `0x803E6590` to `0x803E65EC` |

The BSS contains a 16-byte processor, its 12-byte runtime registration record, two arrays of five four-byte colors, and their 12-byte registration records. The last record ends at `0x803E65EC`; four alignment bytes precede the next unit at `0x803E65F0`. Its old 16-byte symbol extent included that padding and is reduced to twelve before splitting. The data contains the Process jump table, class vtable, class and base RTTI names, and its base list. The next literal, `PIINAME_NORMAL`, begins the following unit.

With only the complete split and that owned BSS symbol correction, configure/map, default Ninja after deleting the prior DOL, and frozen-manifest verification each exit 0. The original split object is selected. No reconstructed-source credit or issue completion is claimed by this checkpoint.

## Traps

The issue's last two functions belong to the next class's unit. Preserve that class's initializer, registration records and neighboring #55 methods together; the survey issue boundary is not a source-file boundary.
