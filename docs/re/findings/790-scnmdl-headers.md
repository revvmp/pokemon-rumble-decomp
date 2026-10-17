# F-790-2: Scene-model hierarchy interface prerequisite

## Scope

This prerequisite supplies ScnMdlSimple and ScnMdl declarations required by the complete ScnMdl source unit shared by #790 and #791. Merged PR846 supplies the complete canonical ScnObj/ScnLeaf header, which is preserved unchanged; the temporary duplicate proposed here is removed. This prerequisite changes no source unit, split, object status, or decompilation credit. Existing canonical G3dObj, resource views, math structures, and animation base classes remain unchanged.

The owner-approved DrawResMdlReplacement and draw-mode interface from PR816, complete AnmObjShp header from PR785, and CalcVtx declaration from PR837 are reused verbatim. These temporary overlapping prerequisites are dropped from this proposal as their canonical deliveries merge; no competing partial AnmObjShp class is introduced. The draw-mode enum retains its original `REDMDL_DRAWMODE_SORT_NONE` spelling. No additions to #777's CalcWorld header are needed.

Coordinating owner #783 independently compiled all 23 ScnMdlSimple functions / 8,148 bytes and paired all 96 text relocation targets with these hierarchy headers unchanged. Its material dispatch calls CalcMaterialDirectly at `0x80270750`; this prerequisite also supplies that original four-argument declaration and its three animation forward declarations. The bounded header follows the same pinned SS reference and is preserved verbatim from the #783 owner's reviewed proposal. No source/split/config ownership is taken from the material unit's #792 owner.

After PR846 merged, both consumers were re-proved against main's full ScnObj header: all 27 ScnMdl functions and all 23 ScnMdlSimple functions retain their exact native instruction streams. The Simple owner also paired all 112 code/vtable relocation targets and all four initialized native data sections. No additions to main's ScnObj header were needed.

## Target layout and code evidence

The inheritance is G3dObj (12 bytes), ScnObj (220), ScnLeaf (232), ScnMdlSimple (312), and ScnMdl (392). ScnObj contains three 48-byte matrices beginning at twelve, two 24-byte AABBs beginning at 156, flags at 204, draw priorities at 208–209, callback pointer at 212, callback timing/delete flags at 216–217, and callback operation mask at 218. The compiler supplies ordinary alignment before the callback pointer; no invented padding members are retained. ScnLeaf's scale vector starts at 220.

ScnMdlSimple begins with ResMdl at 232, then world matrix/attribute and three view-matrix pointers at 236–252. View count/current index/matrix count occupy 256–259; flags are at 260. Four bytecode pointers occupy 264–279, followed by draw mode at 280. Its world callback pointer/timing/delete/node ID occupy 284–291, followed by five animation pointers at 292–311. ScnMdl adds the shape animation pointer at 312, visibility flags at 316, material dirty-array pointer at 320, 64-byte replacement at 324, and replacement flags at 388.

Native nested material constructors at `0x8026D270` read eleven replacement pointers at `0x14C`–`0x174` and retain resource-view strides 260, 100, 584, 40, 8, 12, 32, 128, 64, 160, and 512 bytes. Native visibility construction at `0x8026D570` reads the visibility array at `0x148`. SetAnmObj and RemoveAnmObj access shape/flag/replacement members at these same offsets. Native material processing reads ScnObj callback pointer/timing/mask at `0xD4`, `0xD8`, and `0xDA`, then dispatches through the original callback slots. The complete scene hierarchy and nested resource views reproduce all these accesses with ordinary C++ inheritance and member functions.

AnmObjShp storage is independently evidenced by native Release at `0x80258DB0`: signed binding count at sixteen, u16 binding-array pointer at twenty. Its original owner supplied the complete header, including the weak-destructor declarations needed by its own matching source unit. This prerequisite preserves that header unchanged.

The formerly placeholder bit24 buffer-option name is recorded descriptively as `BUFOPTION_RESTORE_VTX_ON_DETACH`: the constructor records it in the low replacement flag, and RemoveAnmObj restores copied vertex buffers when that flag is set. Its value and behavior are unchanged. This name is a behavior-based project name, not a claim to recover an embedded original spelling.

## Provenance and diagnostic validation

The three hierarchy headers follow CC0 zeldaret/ss `787b0b1e9d30e3c1d91ced61350b059ffc5a2411`. Their raw SHA-256 values are ScnObj `a76f3bffbfa27f47738a028122aa95d9bd606c6aa4695bbd1f116b6a033a0cac`, ScnMdlSimple `27145380a4015767a2720e3568b5d76544478f5d5f5d1cd3437a9ac194da9108`, and ScnMdl `9945194611ed95b22a245bfedce35cc3adabe2ac0b5aaa1d4f7e979fc9b767de`. No file-level license exceptions occur in these headers. The repository LICENSE is CC0 1.0 Universal, SHA-256 `a2010f343487d3f7618affe54f789f5487602331c0a8d03f49e9a7c547cf0499`.

The complete retained ScnMdl candidate compiles using only these tracked canonical headers under established Wii/1.1 NW4R flags. All 27 retained functions / 10,240 instruction bytes have their native sizes and exact non-relocation instruction streams. This diagnostic establishes the consumed interfaces; it does not yet claim linked source acceptance. The source delivery must still prove relocations, data, final native addresses, source-only inputs, and whole-DOL identity.

The paired native diagnostic also validates 167 text relocations, sixteen vtable pointer relocations, all 72 vtable bytes, and the twelve-byte type-name object. Source-owned storage resolves to `.data` `0x803C6060`–`0x803C60A8` and `.rodata` `0x8036C9B8`–`0x8036C9C4`. Both original issue inventories reconcile independently as ten / 1,344 and seventeen / 8,896 without overlap.

The header proposal passed a fresh 469-target plain Ninja build and explicit frozen verification of `build/wpse-v1/main.dol`, both exit zero. Canonical-header candidate compilation, all 27 native-stream comparisons, paired relocation/data diagnostics, and both cached original issue inventories also exit zero. No source files or build configuration change in this prerequisite; full source-unit acceptance remains in the source delivery.

## Traps

An older OGWS replacement structure lacks its leading flag word. It compiles and leaves some code sizes unchanged while shifting every material pointer by four bytes. Reusing the owner-approved 64-byte flag-first canonical declaration restores all used offsets. Preserve it across shared-header rebases.
