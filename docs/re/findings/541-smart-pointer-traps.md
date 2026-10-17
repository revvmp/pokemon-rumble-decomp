# MSL shared ownership, wrappers and MetroTRK traps

## F-541-5: Original shared-pointer class identity

Target `wpse-v1`, frozen manifest `config/targets/wpse-v1.json`. The native string at `0x803997F4` is `std::tr1::detail::shared_ptr_deleter_common`; its RTTI record at `0x804B5AB8` and base vtable at `0x803997E0` establish the original library class. Derived shared-pointer deleter RTTI records refer to that base. This resolves the previously unidentified routines at `0x8034D84C` and `0x8034D8C4` as its `release()` and `release_weak()` members.

The original functional declarations and source-unit name are corroborated by [the MSL header in KooShnoo/pokemon-rumble at 11c112f7548b26ab2cecfcf52e201b8d3b76120f](https://github.com/KooShnoo/pokemon-rumble/blob/11c112f7548b26ab2cecfcf52e201b8d3b76120f/libs/PowerPC_EABI_Support/MSL/MSL_C++/MSL_Common/detail/msl_smart_pointers.hpp). That file retains a Freescale all-rights-reserved notice, overriding its repository's general license. It was consulted for interface identity; no implementation from it or `msl_thread.cpp` is imported. The two short implementations are independently expressed from the target's decrements, branches and virtual calls.

The complete class is twelve bytes: virtual pointer at zero, `ptrdiff_t use_count_` at four and `ptrdiff_t weak_count_` at eight. Its virtual order is destructor, `get_deleter(const std::type_info&) const`, then `dispose()`, placing the deleting destructor at vtable offset eight and disposal at `0x10`. `release()` decrements the shared count, disposes the owned object at zero, then releases its implicit weak reference. `release_weak()` deletes the control object when its weak count reaches zero. The ordinary C++ member functions reproduce all 168 native instruction bytes without raw code, explicit register constraints or original objects.

The source unit uses the measured Wii/1.1 MSL settings with file IPA and a local exception-support override. File IPA inlines the weak-release operation into `release`, producing its native `0x78` bytes; without it that body is only `0x50`. Exception support adds the original eight-byte extab record at `0x80044214` and twelve-byte index at `0x800524D0`, without changing native text. Those sections were discovered by dtk's split analysis and checked against the compiler output. Text alone matched with exception support disabled, but the complete unit did not.

## Narrow and wide wrappers

`strncasecmp` at `0x8034D83C` is the retained final wrapper of `MSL_C/extras.c`. The pinned CC0 Petari reference already used for #540 supplies its original interface. Its branch targets `__msl_strnicmp` at `0x803450A4`, whose signed-byte case folding, count loop and locale lower-case map independently establish that callee's identity. Extending the existing extras split by four bytes preserves its previously accepted `stricmp` body and adds no duplicate credit for those existing 160 bytes.

The twelve-byte wrapper at `0x8034D840` passes a null end pointer and decimal base ten to the already identified `wcstol`. Its original name is unresolved: several wide integer convenience APIs share this ABI and instruction sequence. `wide_string_to_decimal` and the isolated `MSL_C/wide_decimal.c` filename are explicitly chosen descriptive names, not recovered names. The independently written C wrapper expresses only that proven conversion. It has its own declaration header; adding its unused declaration to the general wide-character header needlessly changed other compiled objects, so that rejected form is excluded.

## Original MetroTRK trap source

The four eight-byte entries at `0x8034D8F4`, `0x8034D8FC`, `0x8034D904` and `0x8034D90C` follow the original `TRKAccessFile`, `TRKOpenFile`, `TRKCloseFile` and `TRKPositionFile` order in [Petari's targsupp.s at e0ab141914921726a595573dc48d3f13d0922efe](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/src/MetroTRK/debugger/embedded/MetroTRK/Processor/ppc/Export/targsupp.s), CC0-1.0 with no file-level exception found. The first name additionally has its paired file-access caller and full-width result interface from the console adapter. The remaining names use the complete consecutive reference-unit order, since the bodies alone are identical.

The retained original assembly trap instruction and return are expressed in CodeWarrior inline-assembly syntax. MWCC rejects GNU's `twui` alias; the architectural `twi` form with condition mask 31 emits the identical instruction. No opcode constants are embedded. The source-active pragma retains the complete assembly unit, including its unused public trap entries. These 32 bytes are original assembly source, separate from C/C++ decompilation credit. The chosen `.c` source container replaces the reference assembler wrapper, not its instructions or behavior.

## Validation and remaining work

Split-only and source-selected configure/Ninja/frozen-verifier workflows each exit zero. Every compared section in extras, the wide wrapper, the C++ release unit and the trap unit scores 100%. The two release functions additionally match all raw instructions without relocation exclusions. All four trap bodies match every native byte and have no relocations. Five MSL final function addresses and all four trap final addresses agree with the original; a direct audit checks the actual linker input list selects these source objects and no corresponding original fallbacks.

All 345 unrelated objects from the pre-dispatch baseline remain byte-identical; the existing extras object is the sole intentional extension. The complete 4,094,880-byte DOL remains byte-identical with frozen SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. No original or expected hash changes.

The branch now supplies 31 of 32 #541 functions: 2,644 C/C++ function bytes and 88 explicitly disclosed original assembly bytes, totaling 2,732 of the issue's surveyed 2,880 bytes. `InitMetroTRK` remains; its original source unit crosses into #542 and is being completed with that contributor. This intermediate result does not close #541.

## Traps

- A broad refcount-name search missed the original class. Search embedded `shared_ptr` RTTI, then connect base descriptors and virtual slots before inventing a class identity.
- Byte-identical C++ text can still omit the native exception tables. Inspect every automatically added section and retain a local, measured exception-support setting.
- A general header declaration can change unrelated object contents despite being unused. Keep the chosen wide conversion interface local and compare baseline objects after header edits.
- A one-instruction wrapper or identical trap body cannot be named from a relocation-masked match alone. Require paired callees/callers, complete reference-unit order and independent behavior evidence.
