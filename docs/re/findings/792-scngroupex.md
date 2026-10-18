# F-792-2: ScnGroupEx independently reconstructed

Target: `wpse-v1`, frozen `orig/wpse-v1/main.dol`. Issue #792 is now fully accepted: 32 original functions / 4,788 instruction bytes across four source-selected units. This change adds the remaining 24 functions / 3,628 instruction bytes. The eight functions / 1,160 bytes accepted in F-792-1 receive no duplicate source credit.

## Boundaries and identities

`nw4r/g3d/g3d_scngroupex.cpp` owns text `0x8026F880` through `0x8027074C` (3,788 bytes including alignment), rodata `0x8036C9C8` through `0x8036C9D8` (16 bytes), data `0x803C60A8` through `0x803C6150` (168 bytes), and sdata `0x804B89A8` through `0x804B89C8` (32 bytes). A separate split-only commit passed the real build and frozen DOL verifier before source replacement. The source uses unchanged Wii/1.1 `cflags_nw4r`; no manifest, compiler, dependency or shared header changes are involved.

`ScnGroupEx` is an original type name from the length-prefixed `ScnGroupEx` record at `0x8036C9C8`. Its type test accepts ScnGroupEx, ScnGroup, ScnObj and G3dObj in that order, establishing inheritance. The file basename follows that proven class name and is descriptive; the reference link-order suggestion `g3d_scnmdlexpand.cpp` is not an original filename recovered from this binary.

The range also contains two helper classes, not just ScnGroupEx methods. Their names `detail::ModelDrawGather` and `detail::GatherProxy`, their new helper method names and member names are descriptive, low confidence. Their object layouts, virtual slot order, behavior and retained addresses are proven. No original helper-class spelling is claimed. The implementation is independently written from target instructions, vtables and existing verified interfaces; no absent public ScnGroupEx implementation is assumed or copied.

## Layout and behavior

| Class | Native layout | Evidence |
| --- | --- | --- |
| ScnGroupEx | ScnGroup base `0xE8`, gather pointer at `0xE8`, total `0xEC` | Allocation arithmetic and inlined base construction at `0x80270040`; group vtable at `0x803C6110`; native pointer loads and stores |
| ModelDrawGather | ScnObjGather base `0x18`; ResMdl at `0x18`, opaque and translucent bytecode pointers at `0x1C` and `0x20`; total `0x24` | Gather construction, getter, setter and draw wrappers; vtable at `0x803C60A8` |
| GatherProxy | IScnObjGather base `0x4`; outer gather at `0x4`, model gather at `0x8`; total `0xC` | Three-word stack construction in group G3dProc; Add reads; vtable at `0x803C60E4` |

The model gather extends the existing ScnObjGather vtable with model-resource setter/getter, the `SGEX` signature and resource equality. Its CheckScnObj override accepts a ScnMdlSimple using the same resource with no MATCLR, TEXPAT, TEXSRT or SHP animation. Character and visibility animation are not rejected by this filter.

The proxy first offers eligible models to the inner gather. A nested ScnGroupEx with the same gather resource is suppressed as a separate draw item while its models are collected. Other objects go to the outer gather. Its ninth slot returns the existing GATHER status. The reconstruction declares that evidenced slot on the proxy; it does not change the shared base header or claim whether the original revision declared the default on an ancestor.

The draw routine walks eight-byte draw commands until the native opcode 1 terminator. It reads resource material, shape and node IDs, skips objects with the appropriate draw-disable option, observes copied visibility on ScnMdl, uses node visibility on ScnMdlSimple, sets each model's view matrix arrays, and suppresses redundant material setup between instances and repeated materials. Forced material omission and light-off flags follow the native branches. The simple-model path retains the target's assumptions about a successful cast; no additional runtime guards are invented.

Group gathering clears the model collection, registers the group with the outer collector, and sends each child the proxy. Its sort task supports ZSort, default Sort and caller-provided comparator pairs before forwarding to the base group. Only the second pointer slot of that opaque sort-info input is interpreted; no complete unobserved parameter structure is introduced. Opaque/translucent drawing surrounds the gather call with the existing timing-A and timing-C callbacks. Construction allocates the group, child array, two model arrays and model gather together and clears the base group's two not-gather-draw flags.

## Exact acceptance

The real `configure.py --map`, Ninja and frozen-manifest verifier all exit zero with the new unit selected from source. The full DOL is byte-identical to the frozen original. An independent ELF32 big-endian audit compares every retained function body without masking instructions and resolves all 69 text relocation targets separately. The 16 rodata bytes, 168 retained data bytes and 32 string bytes are byte-identical; all 36 vtable relocation targets also match.

The audit joins all 32 original issue address/size rows to the four accepted units. All retained functions and all reported allocated sections are 100%. Every native function has its exact address and size in the actual ELF and linker map, owned by the correct source object. The actual Ninja link inputs contain the four source objects and exclude their four original counterparts.

The compiled new object also emits seven unretained functions / 332 bytes: a separately emitted auto-inlined model-gather constructor and six existing weak helper copies. Its unused base-interface vtable is 40 bytes. The linker discards these copies; the accepted map retains exactly 24 native functions and the native 168-byte vtable region from this object. They receive no new function, data or source-byte credit. The 3,628-byte count covers reconstructed C++ only, with no assembly or original-object fallback in the unit.

| Address | Size | Linked native symbol | Source unit |
| --- | --- | --- | --- |
| `0x8026F880` | `0x8` | `CheckScnObj__Q44nw4r3g3d6detail11GatherProxyFPQ34nw4r3g3d6ScnObj` | `g3d_scngroupex.cpp` |
| `0x8026F890` | `0x130` | `Add__Q44nw4r3g3d6detail11GatherProxyFPQ34nw4r3g3d6ScnObjbb` | `g3d_scngroupex.cpp` |
| `0x8026F9C0` | `0x28` | `DrawOpa__Q44nw4r3g3d6detail15ModelDrawGatherFPQ34nw4r3g3d14ResMdlDrawMode` | `g3d_scngroupex.cpp` |
| `0x8026F9F0` | `0x28` | `DrawXlu__Q44nw4r3g3d6detail15ModelDrawGatherFPQ34nw4r3g3d14ResMdlDrawMode` | `g3d_scngroupex.cpp` |
| `0x8026FA20` | `0x178` | `CheckScnObj__Q44nw4r3g3d6detail15ModelDrawGatherFPQ34nw4r3g3d6ScnObj` | `g3d_scngroupex.cpp` |
| `0x8026FBA0` | `0x8` | `GetResMdl__Q44nw4r3g3d6detail15ModelDrawGatherCFv` | `g3d_scngroupex.cpp` |
| `0x8026FBB0` | `0x3F0` | `Draw__Q44nw4r3g3d6detail15ModelDrawGatherFPCUcUlPPQ34nw4r3g3d6ScnObjPQ34nw4r3g3d14ResMdlDrawModeb` | `g3d_scngroupex.cpp` |
| `0x8026FFA0` | `0x8C` | `IsSameResource__Q44nw4r3g3d6detail15ModelDrawGatherCFPCQ44nw4r3g3d6detail15ModelDrawGather` | `g3d_scngroupex.cpp` |
| `0x80270030` | `0xC` | `GetSignature__Q44nw4r3g3d6detail15ModelDrawGatherCFv` | `g3d_scngroupex.cpp` |
| `0x80270040` | `0x158` | `Construct__Q34nw4r3g3d10ScnGroupExFP12MEMAllocatorPUlUlUlQ34nw4r3g3d6ResMdl` | `g3d_scngroupex.cpp` |
| `0x802701A0` | `0x40` | `__dt__Q44nw4r3g3d6detail11GatherProxyFv` | `g3d_scngroupex.cpp` |
| `0x802701E0` | `0x328` | `G3dProc__Q34nw4r3g3d10ScnGroupExFUlUlPv` | `g3d_scngroupex.cpp` |
| `0x80270510` | `0x3C` | `__dt__Q34nw4r3g3d10ScnGroupExFv` | `g3d_scngroupex.cpp` |
| `0x80270550` | `0xA4` | `SetResMdl__Q44nw4r3g3d6detail15ModelDrawGatherFQ34nw4r3g3d6ResMdl` | `g3d_scngroupex.cpp` |
| `0x80270600` | `0x40` | `__dt__Q44nw4r3g3d6detail15ModelDrawGatherFv` | `g3d_scngroupex.cpp` |
| `0x80270640` | `0x4` | `DrawXlu__Q44nw4r3g3d6detail11GatherProxyFPQ34nw4r3g3d14ResMdlDrawMode` | `g3d_scngroupex.cpp` |
| `0x80270650` | `0x4` | `DrawOpa__Q44nw4r3g3d6detail11GatherProxyFPQ34nw4r3g3d14ResMdlDrawMode` | `g3d_scngroupex.cpp` |
| `0x80270660` | `0x4` | `Sort__Q44nw4r3g3d6detail11GatherProxyFv` | `g3d_scngroupex.cpp` |
| `0x80270670` | `0x4` | `Sort__Q44nw4r3g3d6detail11GatherProxyFPFPCQ34nw4r3g3d6ScnObjPCQ34nw4r3g3d6ScnObj_bPFPCQ34nw4r3g3d6ScnObjPCQ34nw4r3g3d6ScnObj_b` | `g3d_scngroupex.cpp` |
| `0x80270680` | `0x4` | `ZSort__Q44nw4r3g3d6detail11GatherProxyFv` | `g3d_scngroupex.cpp` |
| `0x80270690` | `0x4` | `Clear__Q44nw4r3g3d6detail11GatherProxyFv` | `g3d_scngroupex.cpp` |
| `0x802706A0` | `0x68` | `IsDerivedFrom__Q34nw4r3g3d10ScnGroupExCFQ44nw4r3g3d6G3dObj7TypeObj` | `g3d_scngroupex.cpp` |
| `0x80270710` | `0x30` | `GetTypeName__Q34nw4r3g3d10ScnGroupExCFv` | `g3d_scngroupex.cpp` |
| `0x80270740` | `0xC` | `GetTypeObj__Q34nw4r3g3d10ScnGroupExCFv` | `g3d_scngroupex.cpp` |
| `0x80270750` | `0x234` | `CalcMaterialDirectly__Q24nw4r3g3dFQ34nw4r3g3d6ResMdlPQ34nw4r3g3d12AnmObjTexPatPQ34nw4r3g3d12AnmObjTexSrtPQ34nw4r3g3d12AnmObjMatClr` | `g3d_calcmaterial.cpp` |
| `0x80270990` | `0xF0` | `G3dInit__Q24nw4r3g3dFb` | `g3d_init.cpp` |
| `0x80270A80` | `0x8` | `G3dReset__Q24nw4r3g3dFv` | `g3d_init.cpp` |
| `0x80270A90` | `0x8` | `__ct__Q34nw4r3g3d3FogFPQ34nw4r3g3d7FogData` | `g3d_fog.cpp` |
| `0x80270AA0` | `0x70` | `Init__Q34nw4r3g3d3FogFv` | `g3d_fog.cpp` |
| `0x80270B10` | `0x54` | `CopyTo__Q34nw4r3g3d3FogCFPv` | `g3d_fog.cpp` |
| `0x80270B70` | `0x20` | `SetFogRangeAdjParam__Q34nw4r3g3d3FogFUsUsRCQ34nw4r4math5MTX44` | `g3d_fog.cpp` |
| `0x80270B90` | `0x70` | `SetGP__Q34nw4r3g3d3FogCFv` | `g3d_fog.cpp` |

## Traps

- **A class-name reference does not make every function a method of that class.** The Add routine at `0x8026F890` operates on a twelve-byte gather proxy and only dynamically tests ScnGroupEx. Recover the adjacent vtables and constructors before assigning the whole range to one class.
- **A 100% function score still permits incorrect inline emission order.** The first source link put the inline resource setter after the explicitly defined helper destructor and empty proxy methods. It failed with 263 changed bytes, even though all 24 functions individually compared at 100%. Making those native inline methods inline in their declarations restored their order. For this proxy's overloaded Sort name, the two overloads retain declaration order within the emitted name group; reversing them swaps two native entries. The subsequent fresh full link and frozen comparison pass.
- **Constructor and setter placement determine duplicate literal storage.** An inline-header constructor and ordinary setter produced exact string contents with wrong string relocation targets. An ordinary auto-inlined constructor plus the inline virtual setter produces the native two ordinary constructor literals followed by two weak setter literals. No fabricated padding, literal-address loads or compiler flag change is needed.
- **Virtual table order follows class declaration context.** Declaring ScnGroupEx, GatherProxy and ModelDrawGather in that order emits the three retained tables in native model/proxy/group order. All table bytes and 36 resolved targets are independently checked.
- **Equivalent boolean control flow can change the native temporary graph.** Returning the proxy's combined group predicate from a small inline method preserves the native boolean accumulator; folding it directly into the caller's if removes four instructions. The resource-equality routine instead needs its explicit true/false return arms. Caching the model-array byte stride and the first array pointer, with the group-result declaration after the stride, recovers construction's native register allocation. These are target-specific observations, not general formatting rules.
