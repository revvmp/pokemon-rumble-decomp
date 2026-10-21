# F-57-1: Controller interfaces and native unit boundaries

Issue #57 owns 26 original functions, 5,300 instruction bytes, from `0x8006E6A0` through `0x8006FBE4`. The preceding issue ends with an unrelated destructor; the following issue begins `CDisconMsg`. RTTI, vtable targets, exception records and independent controller-global constructors establish five complete classes in this interval.

| Class | Native text | Object size | Additional state |
| --- | --- | --- | --- |
| `CControllerClassic` | `0x8006E6A0`–`0x8006EA50` | `0x34` | None |
| `CControllerComposition` | `0x8006EA50`–`0x8006F0B8` | `0x40` | `std::vector<IController*>` at `0x34` |
| `CControllerGCPad` | `0x8006F0C0`–`0x8006F350` | `0x34` | None |
| `CControllerNunchaku` | `0x8006F350`–`0x8006F5C8` | `0x34` | None |
| `CControllerWiiRemote` | `0x8006F5D0`–`0x8006FBE4` | `0x38` | Classic-input selection Boolean at `0x34` |

The class spellings are original RTTI names. Their interfaces override the existing `BController` virtual slots without adding virtual slots. Method and field names are descriptive. The Wii Remote constructor calls `BController()` and clears its additional Boolean. The composition vector has data, size and capacity words at `0x34`, `0x38` and `0x3C`. Its assignment method at `0x8006EC10` checks self-assignment, grows using the same three capacity regimes as the existing MSL vector, copies pointer elements with `memmove`, and updates size.

`GetControllerTable()` at `0x80124A50` returns the composition object at `0x803E9088`, also returned through the existing `GetUIInput()` interface. `IsExtensionControllerEnabled()` at `0x80124970` reads the Boolean written by `0x80124960`. Native initialization at `0x801248C0` sets `BController::mChannel`, offset 4, for all sixteen entries in the four-by-four controller table. The additive `SetChannel(s32)` setter exposes that existing field without altering object layout.

The free `GetControllerDisconnectState()` at `0x8006FA00` walks the composition vector. It returns the maximum state across non-null controllers: 0 connected, 1 disconnected GameCube, 2 disconnected Nunchaku with its Wii Remote still connected, 3 disconnected Classic with its Wii Remote still connected, 4 disconnected Wii Remote or missing parent Wii Remote, and 5 unsupported extension while the separate extension-controller mode is disabled. The original returns a signed integer and materializes temporary integer arguments for a reference-taking maximum operation.

## Motor pattern record

`ScriptBinding_StartRumble`, `0x80151F70`–`0x801520E4`, converts the script pattern index with the existing signed `CValue::ToInt` and selects a record from `0x803550B8` with stride 12 before calling controller virtual slot 9. The three records are `{0, 0x804B7728, 0x804B772C}`, `{1, 0x804B7730, 0x804B7734}`, and `{2, 0x804B7738, 0x804B773C}`. The first word is the sequential pattern index. Its signed `int` declaration follows the script index; only nonnegative stored values are observable here, so signedness is not independently distinguished by those constants.

`CControllerWiiRemote::StartMotor` passes the pointer at offset 4 to the existing typed `StartWiiRumble(int, const RumbleStep*)`. `CControllerGCPad::StartMotor` passes offset 8 to `StartGCRumble`. Consequently `MotorPattern` is a complete twelve-byte record with one index and two typed pattern pointers. No opaque padding or guessed payload is required.

## Reconstruction control

Unchanged `GAME_COMPILER` Wii/1.1 and `cflags_game` reproduce all five Classic functions, all five GameCube functions, all three Nunchaku functions, all six Wii Remote member functions and the disconnect-state query: twenty original functions. The four concrete units have been linked from source together and the full frozen DOL passes. Local controls also reproduce composition `GetType` and `GetButtonSymbol`. The four other composition bodies remain under investigation, so this finding does not establish completion of the original issue.

The native Boolean stages in the GameCube and Wii Remote input checks correspond to nested readability, connection and processing predicates. Collapsing all stages into one expression shortens the routines. The Classic button mapper declares the native hold word before its result and compares positive stick thresholds with the constant as the left operand; the equivalent reversed comparison selects different floating comparison instructions.

## Traps

- A seeded `main.elf.MAP` can be stale even after a successful current link. Default `configure.py` does not enable map generation. The first controller control had a current ELF with source symbols while its seed MAP still named original fallback objects. Generate with `configure.py --map`, relink and verify before using a MAP to attribute source coverage; inspect timestamps and actual symbol entries. Never infer source attribution from an inherited map.
- Composition scratch controls are not accepted source. Its update currently has the correct 424-byte extent with three prologue scheduling differences; its two rumble loops differ only in prologue ordering. Pointer-vector assignment still needs the original iterator and allocation context. Wii/1.3 gives the same residuals as Wii/1.1; changing compiler flags is not justified.
