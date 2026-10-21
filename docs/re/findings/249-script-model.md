# Issue 249: model motion script interfaces

## F-249-1: typed model motion and null-object interfaces

The original issue contains 35 functions and 9,592 code bytes at `0x80146EE0`–`0x80149520`. The first nine bindings pass a name, starting frame, and rate or requested duration to `BModelObj`; six extended/length wrappers pop rate/duration before frame and name. Play defaults to frame zero, request and request-next default to frame minus one. A scoped string-pop helper preserves the temporary lifetime observed in all six extended wrappers.

`CModel::CMotionProgram` is 48 bytes: allocation at `0x801BAF40`, initialization at `0x801BAD70`, and this issue's typed getters independently establish the following layout.

| Offset | Storage | Evidence |
| --- | --- | --- |
| 0 | state flags | script predicates, pause and resume |
| 4 | motion ID | current-motion and play result getters |
| 8 | starting frame | initializer and animation update |
| 12 | playback rate | initializer and speed getter |
| 16 | reserved state word | initializer zeros this word; no stronger meaning established |
| 20 | `std::string` name | constructor zeros all three words, subsequent string assignment |
| 32 | reserved word | remaining storage before independently evidenced frame pointer; meaning unresolved |
| 36 | pointer to current frame | initializer points into partial-animation state; frame getter dereferences |
| 40 | `nw4r::g3d::ResAnmChr` | resource lookup, frame count at resource offset 32 |
| 44 | integer length | initializer and script length getter |

The model uses pending `0x10`, playing `0x20`, looped `0x40`, finished `0x80`, stopped `0x100`, and paused `0x200`. Loop count occupies the upper 16 bits. These flags differ from the scene-animation motion records and must not be substituted globally. `GetCurrentMotion` (`0x801B9210`), `FindMotion` (`0x801B9310`), and `Stop` (`0x801B97D0`) remain external fallback implementations; declarations do not claim their reconstruction.

`CNullObj` allocates 108 bytes in `NewNullObj` (`0x80148EB0`). Its constructor at `0x800D4B50` calls the 72-byte `BWorldObj` base, installs vtable `0x803A6C68`, sets next-motion ID at 72, initializes the retained file owner at 76, constructs a motion list at 84, and initializes the weak target at 100. Animation update `0x800D5B80` establishes the float frame at 96. Vtable comparison establishes that only the destructor and `Calc` override inherited entries. `SetTarget` at `0x800D4DE0` assigns weak ownership, and `LoadAnm` at `0x800D4E20` consumes a path and background flag. The script prefix is the native literal `5:` at `0x804B7A40`.

The ordinary `BWorld::Add` entry at `0x8005EEE0` both registers an object and handles camera activation; it calls `AddObject` at `0x8005F230`. The null-object creation binding uses the former. A local world pointer expresses evaluation of `GetWorld` before constructing its converted shared-owner argument, as observed in the native call sequence.

## Validation and remaining work

An actual Wii/1.1 game-flags producer currently reconstructs all 35 functions. Strict relocation application proves 30 native instruction bodies, 7,816 code bytes, and all 29 exception records for those bodies, 1,368 bytes including actual cleanup targets. The five residual code differences are ordinary play/request/request-next (400 versus 396), motion length (216 versus 212), and null animation load (380 versus 376). All share a one-instruction difference in the ordinary string-pop path. No split, Matching object, or issue completion is claimed.

A same-base control compiled all 761 existing source objects before and after the proposed interfaces. 742 objects were byte-identical; the remaining 19 preserved every parsed section, symbol attribute, and raw relocation. The complete linked ELF preserved those records, and both fresh builds passed the frozen DOL verifier. Raw commands, objects, and comparison reports are preserved outside the repository with the issue number.

## Traps

- A native shared-owner argument can target the higher-level `BWorld::Add` entry. Substituting `AddObject` preserves the caller's shape but changes its actual call target and camera behavior; check the relocated branch, not only instructions with relocations omitted.
- Scoped Slice lifetime matters: moving the ordinary local into a string-pop helper restored all six extended wrappers without a custom record copy constructor. The remaining five ordinary-pop differences are unresolved; do not introduce a copy constructor or forced storage merely to select instructions.
- `CNullObj` inherits a 72-byte base, not the size of larger neighboring world classes. Constructor stores, virtual entries, destructor cleanup, and the 108-byte allocation jointly establish the layout.
