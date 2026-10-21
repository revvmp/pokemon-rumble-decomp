# F-218-1: UI widgets, warp zones, and CWaza

## Scope and current evidence

Original issue #218 lists 27 functions and 5,668 instruction bytes in `0x8011FC80`–`0x80121338`. The original four class boundaries are confirmed: CUIStar at `0x8011FC80`, CUIType at `0x8011FE40`, CWarpZone at `0x80120000`, and CWaza at `0x801202D0`. The next unit begins at `0x80121340`.

CUIStar has three functions / 428 bytes, CUIType four / 424, CWarpZone six / 688, and CWaza fourteen / 4,128. The CWaza inventory includes its map destructor at `0x80120340`, key lookup helper at `0x80120E10`, and secondary-base destructor thunk at `0x80121330`. All original instruction streams have native-size, zero non-relocation differences in the current producer. This statement alone does not establish linked source acceptance; relocation, exception, data, and full-DOL gates remain required.

The game compiler is unchanged Wii/1.1 with `cflags_game`. The current producer was recompiled after adopting main `50d012ef`, including the newly accepted generic vector implementation. No old vector overlay is used.

## Names and layouts

CUIStar and CUIType inherit BUILayout without additional members. Star pane names are `STAR%d` and `TSTAR%d`; type initialization loads `type`, writes `TYPE`, and colors `COLOR`.

CWarpZone inherits CItem, with effect flags at `0x104` and a shared mini-map marker at `0x108`, giving size `0x110`. Its initializer loads `minimap_warp`; its multiple-inheritance destructor adjustment is compiler-generated.

CWaza inherits BWorldObj and BSoundSource. BWorldObj occupies `0x48`, BSoundSource occupies `0x1C` with its virtual pointer at `0x60`, followed by the move ID at `0x64`, shared CPpuEngine at `0x68`, and twelve-byte count map at `0x70`; total size is `0x7C`.

The script bindings `method_CWaza_clearBlownEnemysCount`, `method_CWaza_getBlownEnemysCount`, and `method_CWaza_incBlownEnemysCount` directly call `0x80120940`, `0x80120C90`, and `0x80120D00`. Their original `Enemys` spelling is retained in the reconstructed API. Setup takes its shared parent by value: its caller at `0x8015E530` copies the argument, calls Setup, and releases the copy; the resulting 236-byte producer is exact.

The complete effect record is the existing 24-byte EffectInfo type. The read-only table at `0x803550E0` has 696 entries. Attachment lookup uses the record node-name pointer at offset eight, validates the model resource node, and returns translation from its world matrix.

## Table contract

The canonical session separately embeds WazaDescData and owns a shared CStatistic. These types must not be conflated. The table constructor at `0x8010DDE0` establishes a vector of eight-byte NPpu::CValue at zero, string and wide-string intern sets at twelve and twenty-four, and integer row/column counts at thirty-six and forty. The complete WazaDescData remains 44 bytes. The native cell accessor at `0x80110100` indexes `(row + 1) * columns + column`.

Independent typed constructor and accessor controls produce all sixty and thirty-two native bytes respectively. The earlier #140 outside layout evidence corroborates the fields but its inferred CStatistic identity was not established by RTTI and is not adopted here.

## Shared map context

The pinned 2006 Metrowerks map/tree reading references are Koo commit `11c112f7548b26ab2cecfcf52e201b8d3b76120f`, `libs/PowerPC_EABI_Support/MSL/MSL_C++/map` and `tree`. They document transformed mapped-value storage, a public map over a transformation layer, tree interface and constructor roles, and state-owning deleter storage using three compressed pairs. The compact implementation is reconstructed independently against native producers; no reference comments or raw decompiler output are included.

CWaza and BWorld share native compact-map destroy and insertion dependencies at `0x80063FB0` and `0x80063DE0`. The previous CEffect-pointer-specific names were inferred before scalar storage transformation was modeled. A typed BWorld mapped-pointer control and CWaza now emit the same unsigned-long storage helper types. BWorld destroy is exact at 656 bytes; its insertion retains the previously recorded three register differences at 460 bytes, so no new reconstruction claim is made for that outside dependency.

A current-consumer control with the candidate map header, CUIStar selected from source, and all existing Matching units passed configure, ninja, and the explicit frozen-manifest verifier. Immutable raw control is `C:/MassStorage/GitHub/218-reference/map-current-consumer-control-50d012ef`. This does not yet select CWaza and does not replace its own source acceptance gate.

## Traps

- Reusing an old include overlay can hide newly accepted generic vector declarations. Rebase the issue branch and compile against current canonical vector; preserve only the bounded map overlay under investigation.
- Findings naming a generic table CStatistic do not prove identity. Canonical CGameSession separately owns CStatistic and embeds WazaDescData; preserve that distinction while adopting independently proved storage fields.
- Equal instruction bodies can still call different shared helpers. Decode actual branch targets before claiming completion: CWaza reuses BWorld's erased scalar map helpers rather than a second CEffect-pointer instantiation.
- Combining early return conditions changed the attachment routine from native 464 bytes to 448. The independent signed range check and separate null checks reproduce the native control flow without altered flags or artificial statements.
