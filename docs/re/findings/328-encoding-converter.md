# F-328-1: encoding converter after the archive font wrapper

## Scope and ownership

The original #328 inventory contains 25 functions and 3,964 native code bytes. Its first sixteen functions continue the #327-owned UI file; that owner approved coordinated body work while retaining the canonical UI and generic-library interfaces. The functions at `0x801CA180` and `0x801CA240` are the archive-font wrapper destructor and construction path. Those depend on shared ownership and an incompletely recovered CFileData interface, and are not accepted here.

The final seven functions, `0x801CA450` through `0x801CA87C`, form a separate encoding-converter unit. Its initialization references local member-function tables and encoding-name strings; its six following methods reference only its own fields and the canonical ENC interface. Its data follows the preceding archive-font type-name cluster and ends before the next layout strings. The reconstruction uses the descriptive class name `CEncodingConverter`; neither that name nor the descriptive method names are claimed as original spelling.

This checkpoint stages the unit split and the correction of its two native guard symbols. No object is registered or marked Matching yet. Full split-only build verification and source selection remain pending; the external proof below establishes candidate evidence rather than acceptance.

## Complete class and behavior

The class occupies `0x44` bytes. Two canonical `ENCContext` objects occupy `0x00` and `0x10`. Three twelve-byte CodeWarrior member-function pointers occupy `0x20`, `0x2C` and `0x38`: input decoding, output encoding and selected conversion. Ordinary member-function calls reproduce the native `__ptmf_scall` sites; no function-pointer byte encodings or custom dispatch records are used.

The separate ROM-font wrapper constructor at `0x801D9B40` corroborates the class boundary: it passes `this + 0x1C` to Initialize at `0x801D9C50`, and its next independent encoding member is at `this + 0x60`. The difference is exactly `0x44`. It supplies mode 1 as input and the actual font encoding as output. That neighboring #336 source is inspected only as an ABI consumer and is unchanged.

Initialize reuses the existing `nw4r::ut::FontEncoding` parameter type: the consumer's output argument is directly returned by the SDK's typed GetEncoding method, and its four encoding ordinals agree with all four converter table entries. No competing encoding enum or change to the canonical font header is introduced. Recompilation and the complete strict comparison remain exact with this canonical type.

Initialization selects from four modes in the native tables: UTF-8, UTF-16BE, Shift_JIS and windows-1252. The UTF names are literal native strings, not inferred aliases. Both member-function arrays begin with two null entries, followed by the Shift-JIS and single-byte helper methods. Initialization assigns the first table's selected entry before initializing and selecting from the second local-static table. Each context is initialized, assigned its external encoding name and configured with space as both alternative characters.

The native selection order is preserved exactly: use Identity when both selected member pointers are null, otherwise select the input helper if present, otherwise the output helper if present, otherwise the bridge helper. The fallback is retained as ordinary source even though the preceding tests make it appear unreachable for unchanged member pointers. This finding does not correct or reinterpret the original selection policy.

| Native address | Size | Descriptive method |
| --- | --- | --- |
| `0x801CA450` | `0x23C` | Initialize |
| `0x801CA690` | `0x08` | Identity |
| `0x801CA6A0` | `0x4C` | ConvertThroughUnicode |
| `0x801CA6F0` | `0x80` | ShiftJisToUnicode |
| `0x801CA770` | `0x48` | SingleByteToUnicode |
| `0x801CA7C0` | `0x64` | UnicodeToShiftJis |
| `0x801CA830` | `0x4C` | UnicodeToSingleByte |

## Native extents and proof

| Section | Candidate native extent | Evidence |
| --- | --- | --- |
| `.text` | `0x801CA450`–`0x801CA87C` | Seven retained bodies, 1,032 function bytes, ordinary 16-byte entry alignment |
| `extab` | `0x8003C80C`–`0x8003C83C` | Six exact eight-byte records |
| `extabindex` | `0x800513CC`–`0x80051414` | Six exact native indices |
| `.data` | `0x803C0480`–`0x803C0530` | Two member-pointer arrays, Identity/bridge pointer constants, three strings and four name pointers |
| `.sdata` | `0x804B8340`–`0x804B8346` | Native `UTF-8` string including terminator |
| `.sbss` | `0x804B94B8`–`0x804B94BA` | Two independently accessed one-byte local-static guards |

The independent Wii/1.1 candidate uses the established game flags without overrides and includes the existing `revolution/enc.h`. All seven instruction streams and sizes match. Strict proof independently resolves all 38 code relocation sites, preserves the other instruction bits, and checks their actual native targets. All 182 initialized bytes and ten data relocation sites match. The six exception records and indices match all 120 bytes and twelve relocation targets. The two guard accesses identify separate byte objects; the old imported `ParseString` four-byte label at `0x804B94B8` misidentified these guards and their following alignment padding.

The encoding-name array is an ordinary anonymous-namespace object defined after Initialize, with a preceding declaration. This reproduces the native order of the two member-pointer constants before the name literals. A local-static name array emitted the same strings before those constants, and failed the strict data/target comparison despite all seven instruction streams matching. No target bytes are embedded in the candidate.

## Commands and results

The following commands ran from the isolated #328 worktree and wrote only issue-prefixed external diagnostic files:

| Command | Exit | Result |
| --- | --- | --- |
| `python C:/MassStorage/GitHub/328-compile-converter.py` | 0 | Ordinary C++ candidate compiled with canonical ENC declarations |
| `python C:/MassStorage/GitHub/328-audit-converter.py` | 0 | All seven sizes and non-relocation instruction streams exact |
| `python C:/MassStorage/GitHub/328-strict-converter.py` | 0 | Seven functions, 1,032 bytes, 38 code targets, 182 initialized bytes, ten data targets, six exception records, 120 exception/index bytes and twelve exception targets exact |

No full configure/Ninja/frozen-artifact verification has run for the staged split. The whole original inventory remains incomplete; no completion or source byte credit is claimed at this checkpoint.

## Provenance and naming confidence

This converter was independently authored from native executable instructions, pointer-table data and actual SDK call targets. No external implementation was imported. SDK declarations are the existing canonical ENC headers. Class, member and method names are descriptive; encoding literals, field types, method behavior, member-pointer ABI, offsets and native bytes are directly verified.

The archive-font consumer independently corroborates only limited CFileData facts: the loader at `0x80066930` returns an eight-byte shared value through `r3`; its archive-font caller supplies filename in `r4`, `0x4000` in `r5` and its remaining incoming argument in `r6`. The pointee's buffer pointer is at `0x1C` and invalid-status byte at `0x28`. The #326 owner independently corroborated those accesses, but has a caller supplying zero in `r5`; the final parameter type and remaining class fields are unresolved. No partial CFileData class is introduced.

## F-328-2: bounded archive-font destructor proof

The original RTTI class `CArchiveFontWithBuf` derives from the canonical `nw4r::ut::ArchiveFont`, whose native size is `0x20`. Its buffer member begins at `0x20` and contains an object pointer followed by a shared control pointer. The destructor at `0x801CA180`, size `0xB8`, is reproduced by the ordinary body `if (mBuffer.get()) Destroy();`, followed by normal member and base destruction. The diagnostic reuses the #327 owner's approved two-field shared-pointer storage, getter and nonthrowing destructor behavior with the canonical `shared_ptr_deleter_common::release` declaration. It introduces no canonical generic header.

The compiler also emits the diagnostic `shared_ptr<void>` destructor, size `0x84`, whose body independently matches the existing native routine at `0x800657E0`. The exposed native template argument is not recovered: `void` is a diagnostic choice, and identical destruction bytes do not distinguish it from another exposed element type. No canonical archive-font member type is published from this comparison. Strict comparison proves all ten actual code relocation targets across these two bodies, both exception records and indices, all 156 exception/index bytes, and seven exception relocation targets. The owned #328 contribution is only the 184-byte archive-font destructor; the 132-byte shared-pointer helper belongs to an earlier issue and receives no duplicate credit.

| External diagnostic command | Exit | Result |
| --- | --- | --- |
| `python C:/MassStorage/GitHub/328-compile-archive-font.py` | 0 | Compiles the bounded derived-class/shared-buffer diagnostic |
| `python C:/MassStorage/GitHub/328-audit-archive-font.py` | 0 | Both native bodies, ten targets and 156 exception/index bytes exact |

This does not establish a complete generic shared-pointer implementation, the full archive-font construction method, a complete CFileData class, the archive-font unit's initialized data or source-selected acceptance. The seven-function converter remains the independent complete unit prepared for the next tooling stage.

## Traps

- For this narrow Shift-JIS character parameter, `0x8100 <= value` and `0xE000 <= value` retain the native direct comparisons. Reversing the lower comparisons to `value >= ...` enables range fusion and changes seven instruction words. Controlled signed/unsigned literal probes showed that literal signedness was not the cause. Preserve ordinary relational operand spelling before considering type or compiler changes.
- A local-static declaration is an executable initialization point. Moving the second member-pointer table's declaration before the first assignment changes 26 instruction words, even though function size and eventual field values agree. Interleave each declaration with its corresponding assignment as the native control flow requires.
- Whole-function instruction equality can hide the wrong physical constant order. Independently compare each data target and initialized section before accepting the source.
