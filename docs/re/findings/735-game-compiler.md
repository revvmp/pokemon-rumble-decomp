# F-735-1: Game code compiler and flags

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Question from issue #735: which CodeWarrior build and flags compile the game's own C++ code, roughly `0x80052500`–`0x80170000` outside the library ranges in F-0012.

## Result

Game code is compiled by a Wii CodeWarrior C++ compiler at default 16-byte function alignment with these flags, registered as `cflags_game` with `GAME_COMPILER = "Wii/1.1"` in `configure.py`:

```
-nodefaults -proc gekko -align powerpc -enum int -fp hardware -O4,p -inline auto
-pragma "cats off" -pragma "warn_notinlined off" -maxerrors 1 -nosyspath -str reuse -enc SJIS
-Cpp_exceptions on -RTTI on -fp_contract off -ipa file
```

| Property | Value | Confidence |
| --- | --- | --- |
| Compiler family | Wii CodeWarrior, Ver 4.2 build 140 to Ver 4.3 build 213 except `0x4201_127` | High |
| Chosen build | Wii/1.1 (Ver 4.3 build 151) | Medium; Wii/1.3 is byte-identical on every game function tested |
| `-Cpp_exceptions on`, `-RTTI on`, `-fp_contract off`, default alignment, `-str reuse` without `pool` | As above | High |
| `-inline auto` versus `-inline auto,deferred` | `-inline auto` | Unresolved; identical under `-ipa file` |
| `-ipa file` | As above | Medium; inferred from inline emission order |
| `-O4,p` versus `-O4,s` | `-O4,p` | Unresolved; identical under `-ipa file` |

The proof unit `game/BController.cpp` (all 19 functions of #38, `.text` `0x80052500`–`0x800528D8`) is linked as `Matching` with these flags, and the full DOL verifies.

## Binary evidence

- **Function alignment.** Of 4,851 functions from `0x80052500` to `0x80131780`, 4,850 start on 16-byte boundaries, as do all 1,240 from `0x80131780` to `0x8016FC10` and all 1,029 from `0x8019DC50` to `0x801E8DE0`. Gaps between functions are 0, 4, 8 or 12 bytes. This is the Wii default alignment, not the packed `-func_align 4` of MSL and Runtime.
- **Exceptions.** `extab` is 0x3DB5C bytes and `extabindex` holds 4,839 entries, of which 3,037 index functions in `0x80052500`–`0x80131780` and 966 in `0x80131780`–`0x8016FC10`. Every game function with a stack frame has an entry, so game code uses `-Cpp_exceptions on`. The entry for `BController::SetStick` (`extab` `0x800066C0`, `104A0000 00000000`; `extabindex` `0x80044220`) is reproduced exactly.
- **RTTI.** Game classes carry CodeWarrior type-info objects: 8 bytes in `.sdata` (name pointer and base list), with the name strings and base lists in `.data`. `-RTTI on` reproduces `__RTTI__11BController`, `__RTTI__11IController` and `__RTTI__8IUIInput` at `0x804B5A20`–`0x804B5A38`.
- **Loop alignment.** The counted loop of the four-slot container `Add` at `0x80093CD0` is preceded by a `nop` at `0x80093D3C` that aligns the loop head to 16 bytes. Wii compilers insert it; GC compilers do not.
- **Strings.** 2,968 of 2,971 NUL-terminated strings in game `.data` (`0x80399380`–`0x803B9D88`) start on 4-byte boundaries, so literals are separate aligned objects rather than one `-str pool` blob. The three exceptions are tails of UTF-16 wide strings, not separate literals.

## Matching functions

All comparisons mask relocation-bearing words and compare every other instruction word with the original at the given address. In the table, "all Wii" means Wii/1.0RC1, 1.0a, 1.0, 1.1, 1.3, 1.5, 1.6 and 1.7.

| Address | Class and function | Size | Matches with `cflags_game` | Excluded compilers (differing words / compared) |
| --- | --- | --- | --- | --- |
| `0x80052500`–`0x800528D8` | `BController`, all 19 functions including `BController()`, `SetStick`, `UpdateButton`, `SetStickFromCross` | 0x3D8 | All Wii | GC/3.0a3 and 3.0a5.2: ctor 1/14, `SetStick` 7/41, `UpdateButton` 6/34; `0x4201_127`: `SetStick` 7/41 |
| `0x80054940` | `BModelObj`, position and bounds setter (descriptive name `SetPosition`) | 0x80 | All Wii, GC/3.0a3, GC/3.0a5.2 | None |
| `0x80093CD0` | Four-slot container in the `CDlgPiiSelector` block (#93), `Add` (descriptive) | 0xCC | All Wii and `0x4201_127` | GC/3.0a3 and 3.0a5.2: 24/50 |
| `0x80093DA0` | Same container, indexed `Get` (descriptive) | 0x10 | All compilers | None |

Only `BController` is committed source. The `BModelObj` and container functions were reconstructed in scratch files to test compilers; their class names come from vtable references (`BModelObj` slot 4 at `0x800549C0`) and the issue block, their method names are descriptive, and they belong to #39 and #93.

## Flag evidence

- **`-fp_contract off`.** `SetStick` computes the stick length as `x * x + y * y` with `fmuls`, `fmuls`, `fadds`. With `-fp_contract on` every Wii compiler fuses it into `fmadds`, and 29 of 40 compared words differ.
- **`-Cpp_exceptions on`.** With `off` no `extab` or `extabindex` entry is produced for `SetStick`; code is otherwise identical.
- **`-ipa file`.** The twelve inline virtual accessors at `0x80052820`–`0x800528D8` appear in the reverse of the order a header would naturally declare them (`GetButtonSymbol`, `StopMotor`, `StartMotor`, `GetDisconnectSerial`, `GetRepeat` down to `IsConnected`, then `GetChannel`). Without `-ipa file` every Wii build emits inline functions in declaration order; with `-ipa file` it emits them in reverse declaration order. The declaration order in `include/game/BController.h` therefore follows the vtable, and `-ipa file` is required to reproduce the layout. Without it the same bytes need a reversed declaration list, which is why confidence is medium.
- **Inline definitions in the class.** The empty wide string `L""` returned by `GetButtonSymbol` is at `0x804B5A38`, after the three type-info objects. A literal in an out-of-line function is emitted before the type info; one in an inline function is emitted with the inline functions after it. The accessors are therefore inline in the class.
- **Inlining mode.** Without `-ipa file`, `-inline auto,deferred` reverses the order of the seven out-of-line functions. With `-ipa file` it produces the same unit as `-inline auto`, so `deferred` is not excluded; `cflags_game` keeps the base `-inline auto`.
- **Default function alignment.** `-func_align 4` shrinks the unit to 0x350 bytes.
- **Optimization level.** `-O3` produces 0x350 bytes and a different constructor. `-O4,s` without `-ipa file` stops inlining `nw4r::math::VEC2Len` and `FSqrt` into `SetStick`, emitting them out of line, but with `-ipa file` it is identical to `-O4,p` for every tested function, so the level is left at the project default `-O4,p`.
- **`-enum int`.** Irrelevant to the tested functions; kept from `cflags_base`.

## Compiler version

Ruled out:
- **GC/3.0a3 and GC/3.0a5.2.** Differ on the `BController` constructor, `SetStick`, `UpdateButton` and the container `Add`, which has no loop-alignment `nop`.
- **GC/1.x to GC/2.7.** Reject `-ipa`, `-enc` and the NW4R paired-single headers; they predate the Wii and the paired-single register saves (`psq_st f31`) in every game prologue.
- **Wii/0x4201_127.** `SetStick` differs in 7 of 41 words.

Not separable by the game functions tested: Wii/1.0RC1, 1.0a, 1.0, 1.1, 1.3, 1.5, 1.6 and 1.7. To find where they differ, every source unit already linked in the repository was compiled with each build and compared function by function (NW4R units with `cflags_nw4r`; SDK units with their configured flags):
- **Wii/1.0 against 1.1** differ only on narrow integer arithmetic and scheduling: 1.1 extends the `int` operand of a compound assignment to a `u8`, `u16` or `s16` (`clrlwi`, `extsh`; `snd_EnvGenerator.cpp` `Update`), clears bits of a `u8` or `u16` with `andi.` where 1.0 uses a wrapping `rlwinm` (`__OSInitAudioSystem`, `GXInitTexObjCI`, `PFCACHE_InitPageList`), and adds `+128` where 1.0 adds `-128` for `s8 -= 128` (`Pad.c`). A scan of game code found no narrow `&= ~mask` sites outside bitfields, whose code is the same in both builds, and no narrow compound assignment from an `int`.
- **Wii/1.1 against 1.3** differ only where inline assembly converts with quantized paired-single stores (`F32ToU16` in `math_triangular.cpp`, the `g3d_resanm*.cpp` key-frame readers): 1.3 adds an `frsp` and a second store. All 717 paired-single loads and stores in game code use GQR 0, so game code has no such conversion.
- **Wii/1.0RC1 and 1.0a against 1.0** differ in register allocation in larger functions (`AllocFromHead_`, `NANDCreate`, `GXSetTevColor`).

Wii/1.1 is chosen because a game links the CodeWarrior runtime and MSL of its own toolchain release, and F-0007 confines those to Wii/1.1 or 1.3; the Wii/1.1 linker reproduces the DOL (F-0010); and NW4R in the same executable carries the `0x4302_151` banner. A game function using a quantized paired-single conversion would separate 1.1 from 1.3, and one with narrow compound assignment would confirm 1.1 over 1.0. If such a function fails to match with Wii/1.1, test Wii/1.3 before changing flags.

## Proof unit

`game/BController.cpp` covers the whole of #38:

| Section | Range |
| --- | --- |
| `extab` | `0x800066C0`–`0x800066C8` |
| `extabindex` | `0x80044220`–`0x8004422C` |
| `.text` | `0x80052500`–`0x800528D8` |
| `.data` | `0x80399380`–`0x80399440` (jump table, vtable, type names and base lists end at `0x80399439`) |
| `.sdata` | `0x804B5A20`–`0x804B5A40` (type info and `L""` end at `0x804B5A3A`) |
| `.sdata2` | `0x804B9C60`–`0x804B9C88` |

The compiler also emits weak vtables for the interfaces `IController` and `IUIInput`, because their implicit constructors are inlined into `BController()` and their stores eliminated. Nothing references them, so the linker strips them and `.data` matches. objdiff 3.8.2 reports 19/19 functions and 848/848 code bytes; `configure.py`, `ninja` and `tools/verify/verify.py` all exit 0. Layout and names are in [symbols](../SYMBOLS.md#game-bcontroller-735).

## Method

- Tools: CodeWarrior compilers from `tools/local/compilers` (Wii/1.1 `mwcceppc.exe` SHA-256 `14f99570…fbe5`, Wii/1.0 `88b1cb54…3e71`, Wii/1.3 `1c5b66ed…69dd9`, and every other GC and Wii build in the bundle), GNU objdump 2.42, dtk and objdiff-cli 3.8.2 through the normal build, Python 3.14.2.
- Candidate sweeps compiled each scratch source with 12 compilers and every combination of `-O4,p`/`-O4,s`, `-Cpp_exceptions on`/`off`, `-fp_contract on`/`off`, `-inline auto`/`auto,deferred`, with and without `-ipa file`, plus single-flag checks of `-O3`, `-func_align 4`, `-enum min` and the `-str` variants. Scripts and objects stayed in an ignored scratch directory named for #735.
- Function names and classes come from RTTI names and vtable slots. Decompiler output was not used.

## Traps

- **A split end inside the next unit's alignment padding is rejected.** Ending `.data` at the last object (`0x80399439`) or `.sdata` at `0x804B5A3A` made dtk fail with `Invalid alignment for split: auto_07_80399439_data`. End data splits at the start of the next object (`0x80399440`, `0x804B5A40`). This differs from `.text`, where F-495-1 ends the split at the last function.
- **dtk rejects a split that cuts a symbol.** dtk had sized the `L""` literal at `0x804B5A38` as 8 bytes, and the build failed with `ends within symbol 'lbl_804B5A38'`. Resize the symbol to its real 2 bytes in `symbols.txt`.
- **Inline accessor order depends on `-ipa file`.** Without it, inline functions follow declaration order; with it, the reverse. When a run of inline functions appears in an unexpected order, check the flag before reordering a header.
- **Reference-taking `min`/`max` change code.** `std::min`/`std::max` written over `const T&` turned the stick clamp in `SetStick` into 0xCC bytes instead of 0xBC. The game's clamp uses by-value templates with `std::min` operand order (`include/game/MathUtil.h`); `nw4r::ut::Min` has the opposite comparison and also differs.
