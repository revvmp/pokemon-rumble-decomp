# F-328-1: encoding converter after the archive font wrapper

## Scope and ownership

The original #328 inventory contains 25 functions and 3,964 native code bytes. Its first fifteen functions continue the #327-owned UI file; that owner approved coordinated body work while retaining the canonical UI and generic-library interfaces. The functions at `0x801CA130`, `0x801CA180` and `0x801CA240` are the archive-font wrapper constructor, destructor and resource construction path. Those depend on shared ownership and an incompletely recovered CFileData interface, and are not accepted here.

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
| `.sdata` | `0x804B8340`–`0x804B8348` | Six-byte native `UTF-8` string including terminator, followed by two alignment bytes |
| `.sbss` | `0x804B9178`–`0x804B9180` | Two independently accessed one-byte guards, followed by six alignment bytes |

The independent Wii/1.1 candidate uses the established game flags without overrides and includes the existing `revolution/enc.h`. All seven instruction streams and sizes match. Strict proof independently resolves all 38 code relocation sites, preserves the other instruction bits, and checks their actual native targets. All 182 initialized bytes and ten data relocation sites match. The six exception records and indices match all 120 bytes and twelve relocation targets. The two guard accesses identify separate byte objects at `0x804B9178` and `0x804B9179`, using the actual native ELF `_SDA_BASE_` of `0x804BDA20`. The second previous automatic symbol included six following padding bytes; the real guard has size one. OSFont's correctly named four-byte ParseString object at `0x804B94B8` remains unchanged. The split-stage correction below supersedes the earlier erroneous guard addresses.

The encoding-name array is an ordinary anonymous-namespace object defined after Initialize, with a preceding declaration. This reproduces the native order of the two member-pointer constants before the name literals. A local-static name array emitted the same strings before those constants, and failed the strict data/target comparison despite all seven instruction streams matching. No target bytes are embedded in the candidate.

## Commands and results

The following commands ran from the isolated #328 worktree and wrote only issue-prefixed external diagnostic files:

| Command | Exit | Result |
| --- | --- | --- |
| `python C:/MassStorage/GitHub/328-compile-converter.py` | 0 | Ordinary C++ candidate compiled with canonical ENC declarations |
| `python C:/MassStorage/GitHub/328-audit-converter.py` | 0 | All seven sizes and non-relocation instruction streams exact |
| `python C:/MassStorage/GitHub/328-strict-converter.py` | 0 | Seven functions, 1,032 bytes, 38 code targets, 182 initialized bytes, ten data targets, six exception records, 120 exception/index bytes and twelve exception targets exact |

The first actual split-stage Ninja rejected the earlier erroneous guard range before compilation or linking; the correction below is now staged for repeat validation. The whole original inventory remains incomplete; no completion or source byte credit is claimed at this checkpoint.

## Provenance and naming confidence

This converter was independently authored from native executable instructions, pointer-table data and actual SDK call targets. No external implementation was imported. SDK declarations are the existing canonical ENC headers. Class, member and method names are descriptive; encoding literals, field types, method behavior, member-pointer ABI, offsets and native bytes are directly verified.

The archive-font consumer independently corroborates only limited CFileData facts: the loader at `0x80066930` returns an eight-byte shared value through `r3`; its archive-font caller supplies filename in `r4`, `0x4000` in `r5` and its remaining incoming argument in `r6`. The pointee's buffer pointer is at `0x1C` and invalid-status byte at `0x28`. The #326 owner independently corroborated those accesses, but has a caller supplying zero in `r5`; the final parameter type and remaining class fields are unresolved. No partial CFileData class is introduced.

## F-328-2: bounded archive-font construction and destruction proof

The original RTTI class `CArchiveFontWithBuf` derives from the canonical `nw4r::ut::ArchiveFont`, whose native size is `0x20`. Its constructor starts at `0x801CA130`, size `0x48`, correcting the surveyed file start of `0x801CA180`. An ordinary empty derived constructor invokes the base constructor, installs the native derived vtable at `0x803C0378`, and default-constructs its buffer member at `0x20` as an object pointer followed by a shared control pointer. Both pointers are initially null. The destructor at `0x801CA180`, size `0xB8`, is reproduced by the ordinary body `if (mBuffer.get()) Destroy();`, followed by normal member and base destruction. The diagnostic reuses the #327 owner's approved two-field shared-pointer storage, getter and nonthrowing destructor behavior with the canonical `shared_ptr_deleter_common::release` declaration. It introduces no canonical generic header.

The compiler also emits the diagnostic `shared_ptr<void>` destructor, size `0x84`, whose body independently matches the existing native routine at `0x800657E0`. The exposed native template argument is not recovered: `void` is a diagnostic choice, and identical destruction bytes do not distinguish it from another exposed element type. No canonical archive-font member type is published from this comparison. Strict comparison proves all thirteen actual code relocation targets across these three bodies, three exception records and indices, all 176 exception/index bytes, and nine exception relocation targets. The owned #328 contribution comprises the 72-byte constructor and 184-byte destructor; the 132-byte shared-pointer helper belongs to an earlier issue and receives no duplicate credit.

| External diagnostic command | Exit | Result |
| --- | --- | --- |
| `python C:/MassStorage/GitHub/328-compile-archive-font.py` | 0 | Compiles the bounded derived-class/shared-buffer diagnostic |
| `python C:/MassStorage/GitHub/328-audit-archive-font.py` | 0 | Three native bodies, thirteen targets and 176 exception/index bytes exact |

This does not establish a complete generic shared-pointer implementation, the full archive-font construction method, a complete CFileData class, the archive-font unit's initialized data or source-selected acceptance. The seven-function converter remains the independent complete unit prepared for the next tooling stage.

## F-328-3: checked shared-from-this conversion

The native routine at `0x801C9C20`, size `0xD0`, is reproduced by the ordinary `enable_shared_from_this<BUI>::shared_from_this()` body returning `shared_ptr<BUI>(mWeakThis)`. The checked weak constructor initializes an empty result, obtains `rWeak.lock()`, throws `bad_weak_ptr` when the temporary's use count is zero, then swaps the temporary into the result. The diagnostic reuses the #327 owner's independently reconstructed weak-lock behavior and shared control layout. It adds only ordinary diagnostic getter, checked-construction and swap operations outside the repository; no generic canonical header is changed.

An independently emitted checked constructor has the same body plus the normal constructor return-this instruction, totaling 212 bytes. Returning that constructor expression through the genuine shared-from-this method instead produces the native 208-byte function exactly. This difference establishes the method/return convention, rather than just an instruction pattern with an arbitrarily omitted return.

Strict comparison validates all 208 owned instruction bytes and ten actual code relocation sites. The thrown type-name record is the native 42-byte record at `0x803C02B0`; the exception vtable's actual destructor slot points to `0x8007CC80`. The emitted shared-pointer destructor also independently matches the existing 132-byte routine at `0x8007C5F0`, including its three code targets. Both exception records and indices match all 120 bytes and five relocation targets. The existing destructor receives no duplicate byte credit. The precise exposed pointer specialization still depends on the independently established BUI consumer; destruction bytes alone do not recover a template element type.

| External diagnostic command | Exit | Result |
| --- | --- | --- |
| `python C:/MassStorage/GitHub/328-probe-checked-weak.py` | 0 | Ordinary shared-from-this method has the native size and every non-relocation instruction word |
| `python C:/MassStorage/GitHub/328-strict-checked-weak.py` | 0 | Owned method and existing destructor, thirteen code sites, two exception records, 120 exception/index bytes and five exception targets exact |

This is bounded native evidence for the #327 owner's complete UI source. It does not select another source unit, supply a complete generic smart-pointer header, or establish completion of the original #328 inventory.

## F-328-4: complete manager interface and bounded native bodies

The native routine at `0x801C9CF0`, size `0x70`, returns the object pointer from a temporary shared value obtained by locking the weak member at `this + 0x4C`. The temporary releases its control reference before returning the raw pointer. Reusing the #327 owner's complete BUI diagnostic layout and weak-lock model, an outside-only `CUIManager : BUI` diagnostic with this weak member reproduces the body `return mFocusedUI.lock().get();` exactly. `GetFocusedUI` and `mFocusedUI` are descriptive names. The neighboring constructor at `0x801C9680` corroborates an additional eight-byte weak member after the `0x4C`-byte BUI base and initializes both words to zero. Its ordinary body sets the manager flag with `mFlags |= 1`; the empty derived destructor naturally destroys the weak member and BUI base.

The complete native manager vtable at `0x803C02DC` contains 44 bytes: RTTI, a null word, destructor, four draw/calc forwarders, GetManager, and the inherited OnAttached, OnDetached and OnInput methods. Its RTTI name is `CUIManager`. GetManager returns `this`. The focused-input dispatcher at `0x801C9D60` is nonvirtual; the vtable retains BUI's OnInput at `0x801C92A0`. Treating the dispatcher as an override would match its standalone instructions but produce an incorrect class interface.

The ordinary validation helper at `0x801C9770` locks the weak member, preserves it when its target reports this manager, and otherwise resets it. The focused-input dispatcher invokes that validation, locks the current target, and forwards input only when the target's input flag (mask `0x4`) is set. Using the canonical existing safe-bool declaration, rather than a direct bool conversion, reproduces both exact native instruction streams and their sizes. IsInputEnabled and DispatchFocusedInput are descriptive names for proven behavior.

| Native entry | Size | Outside candidate |
| --- | --- | --- |
| `0x801C9680` | `0x54` | Constructor, owned by #327 |
| `0x801C96E0` | `0x74` | Destructor, owned by #327 |
| `0x801C9760` | `4` | GetManager, owned by #327 |
| `0x801C9770` | `0xD4` | ValidateFocusedUI, owned by #327 |
| `0x801C9CF0` | `0x70` | GetFocusedUI |
| `0x801C9D60` | `0x140` | DispatchFocusedInput |
| `0x801C9EA0` through `0x801C9ED0` | Four entries of `4` | Calc, PrepareDraw, DrawBefore and Draw forwarders |

`python C:/MassStorage/GitHub/328-probe-manager.py` and `python C:/MassStorage/GitHub/328-strict-manager.py` both exit 0. All ten manager bodies match: six owned #328 entries / 448 bytes and four neighboring #327 entries / 416 bytes. Including independently checked existing 132-byte shared-pointer and 96-byte weak-pointer destructors, strict comparison proves 33 actual code sites, seven exception records, 448 exception/index bytes and eighteen exception relocation targets. The complete 44-byte vtable and all ten pointer targets also match. Existing helpers receive no duplicate credit, and the neighboring entries remain attributed to #327.

The first owner approved contributing these native-proven bodies to its eventual whole source unit. The candidate remains outside the repository pending interface review: its BUI diagnostic makes the evidenced flags protected for the derived constructor and adds the descriptive input-bit getter. It changes no canonical BUI/manager header, split or source registration. Complete UI file acceptance still depends on the remaining list, callback and shared-pointer implementation evidence.

An independent second variant uses actual canonical `include/game/ICalc.h` and `ICalcDraw.h` unchanged, rather than locally redeclaring those classes. It consistently adapts the descriptive PrepareDraw and DrawBefore hooks to canonical CalcMtx and CalcView. `328-probe-manager.py --canonical-bases` and `328-strict-canonical-manager.py` both exit zero with the same ten exact manager bodies, 33 code sites, 448 exception/index bytes, eighteen exception targets and complete vtable. Separate original/canonical objects and proof records are preserved, together with SHA-256 hashes of the exact canonical header snapshots. This source adaptation respects the shared interface and requires no conflicting declaration prerequisite.

A stronger third variant also removes every outside shared_ptr, weak_ptr, enable_shared_from_this and bad_weak_ptr declaration, including the diagnostic control-header overlay. It uses the actual unchanged canonical `memory`, shared control and deleter headers, alongside both canonical calculation interfaces. `328-probe-canonical-memory-manager.py` and `328-strict-full-canonical-manager.py` exit zero with the same complete code, exception and vtable proof. Its five canonical input SHA-256 values and separate object are preserved. The remaining BUI/list/auto_ptr declaration diagnostics are explicitly separate from these existing shared interfaces; no generic header reconstruction is claimed here.

## F-328-5: split-stage guard-address correction

The first real seeded split-stage Ninja exited 1: the proposed guards at `0x804B94B8` overlapped the canonical OSFont split. The outside strict audit had used an incorrect hardcoded r13 base, `0x804BDD60`. The native ELF actually declares `_SDA_BASE_ = 0x804BDA20`; displacements `-0x48A8` and `-0x48A7` therefore resolve to `0x804B9178` and `0x804B9179`. The old address/OSFont-misattribution claim was wrong. No source object had been registered or accepted, and no full-artifact success was claimed.

The corrected audit derives both small-data bases directly from the native ELF symbols and again passes all seven instruction streams, all 38 actual code targets, all initialized data and exception records. The converter split now owns only the two actual guard bytes through `0x804B917A`. Canonical OSFont metadata and its ParseString name are restored exactly to main. The native one-byte guard at `0x804B9179` is distinguished from its six subsequent alignment bytes. The repeat split/source pipeline remains required; successful outside comparison still does not establish source selection.

The corrected split then exposed dtk's aligned automatic-gap requirement: ending the six-byte string at `0x804B8346` creates an automatic small-data object at an invalid two-byte-aligned start. The linked extent therefore ends at the next actual eight-byte-aligned object, `0x804B8348`; the two-byte guard payload similarly has linked extent through `0x804B9180`. The pinned dtk 1.8.4 implementation uses default alignment eight for both small-data sections. The compiled source payloads remain six and two bytes, each with section alignment eight. The extra two and six bytes are native inter-object alignment, not source storage, foreign payload or additional data credit. No padding field or invented source byte is added. Full split and source-selected verification must confirm those physical boundaries.

## Traps

- Derive r13/r2 bases from the actual native ELF symbols. A wrong hardcoded r13 base made a relocation audit accept mislocated guards; the real split overlap check rejected them before source registration.

- For this narrow Shift-JIS character parameter, `0x8100 <= value` and `0xE000 <= value` retain the native direct comparisons. Reversing the lower comparisons to `value >= ...` enables range fusion and changes seven instruction words. Controlled signed/unsigned literal probes showed that literal signedness was not the cause. Preserve ordinary relational operand spelling before considering type or compiler changes.
- A local-static declaration is an executable initialization point. Moving the second member-pointer table's declaration before the first assignment changes 26 instruction words, even though function size and eventual field values agree. Interleave each declaration with its corresponding assignment as the native control flow requires.
- Whole-function instruction equality can hide the wrong physical constant order. Independently compare each data target and initialized section before accepting the source.
- A standalone method's exact instructions do not establish whether it is virtual. The manager's focused-input dispatcher matched even with an incorrect override declaration; independently validating every native vtable slot caught that mistake before any shared header was published.
