# F-327-2: shared standard allocator interface

Part of #327, prerequisite for #53. This header defines an empty standard allocator with type aliases, rebind and ordinary allocation, placement construction and destruction. It was independently written from the standard interface and native storage evidence. No restricted MSL implementation is copied. No source unit becomes Matching and no new decompiled bytes are counted.

The BUI diagnostic uses this empty allocator for its twelve-byte child list. Its constructor at801C8700 reproduces all148 instruction bytes after pairing seven relocation sites: BUI vtable803C0250, weak constructor801C88F0, weak destructor8007DAD0 and construct_array8033FECC. The enable_shared_from_this destructor at801C87A0 reproduces100 bytes with release_weak8034D8C4 and delete801B1580. The weak constructor reproduces16 bytes; its reused destructor at8007DAD0 reproduces96 bytes with the same two external targets. These360 diagnostic bytes establish storage and generated calls, not whole-unit source acceptance. The remaining list destructor is unresolved.

The #53 owner independently substituted this exact allocator into its clean-room wstring and two map-storage diagnostics. The wstring reproduces280 native instruction bytes, all three instruction targets,32 exception bytes and both exception indices. Both map constructors reproduce48 bytes in total with allocator inheritance and preserve the evidenced12/16-byte storage alternatives. These diagnostics remain outside the repository until their owning source units pass full acceptance. Accepted allocator SHA256 is d08690a68d37e7907b0e47b4c26405d88a2d518efca8322fff306c09a215c9a1. Reproduce the independent probes from rumble-53 with python C:/MassStorage/GitHub/53-reference/compile-wstring.py and python C:/MassStorage/GitHub/53-reference/compile-map-layout.py. The wstring object allocates text0x124, extab0x20 and index0x18; retained code is280 bytes, with trailing alignment excluded. The map diagnostic emits two24-byte constructors and8 trailing text padding bytes.

## Repository validation

Baseline7856006dae2dcd299c0cdc30ee503fdbc46fb194: python configure.py --map, plain .venv/Scripts/ninja and explicit frozen verification all exit0. All510 actual compiled source objects were captured by whole-file SHA256 before adding this unused header. The same commands on committed header candidate07d1bca exit0 and all510 whole-file source-object SHA256 values remain identical. The final documentation-only commit receives the same exact-head checks before publication. No build metadata, existing header, source unit, frozen input or manifest is changed.

## Traps

No new project-wide trap emerged. This allocator's empty storage is directly tested by independent native consumers; it does not establish the internal base hierarchy or complete algorithm of the target list implementation.
