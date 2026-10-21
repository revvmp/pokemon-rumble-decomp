# F-336-5: CUIList source reconstruction in progress

Part of #336. These outside probes are not source acceptance or issue completion.

## Corrected types and layout

The three retained functions at 801DA0C0/801DA100/801DA110 belong to IUIListItem, whose RTTI is804B5FD0 and vtable803C14FC. They are an abstract destructor body, a default true virtual and an empty virtual. The latter receives CUIList* and CLayout* at801DB954–801DB978. Method spellings and constness remain inferred.

CUIList derives from BUILayout and is180 bytes. Its native constructor801DA120 initializes complete base134 followed by typed storage. The layout-owner vector begins13C (count140, capacity144), while the IUIListItem-owner vector begins148 (count14C, capacity150). The selected index is154. Fields134 and138 are separate input-direction and scrolling-mode words; they are not the beginning of either vector. Field158 is the first visible item index, and15C/160 bound the relative scrolling range. Native801DB450 clamps selection and calculates this range; mode1 scrolls by complete pages.

The weak owner168/16C refers to IUIListItem, and170 is a raw IUIListItem* identity cache. At801DB990 a weak lock is compared with GetSelectedItem's returned shared owner (801DB250), then the current shared owner is assigned back to the weak owner and raw cache. An earlier index interpretation of170 is rejected.

The three direct-delete callback-list owners174/178/17C have the same eight-byte shared-owner node shape as the BUI lists. The selection notification at801DBA10 invokes each function with the CUIList receiver. This call alone does not distinguish function<void(BUI*)> from function<void(CUIList*)>; complete callback payload identity still needs direct RTTI/EH evidence. A native bound callback RTTI separately proves mem_fn_1<void,CUIList,BUI*> with tuple<CUIList*,ph<1>,nat x8>. It does not by itself prove all three notification list types.

## Outside proof and rejected candidate

Using current canonical BUI/BUILayout/CLayout headers, ordinary Wii1.1 game compilation gives eight complete native instruction streams and all their resolved destinations exactly: IUIListItem destructor64/default8/empty4, CUIList constructor156, layout count8, selection setter20 and clamped selection getter52 and single-archive Load60. Total372 instruction bytes. The complete CUIList destructor is840 bytes, matching the native extent, but22 words differ in the first callback cleanup and final vector register allocation. This destructor candidate is rejected. No function is source-selected, and no matching progress is credited by these probes.

The array Load at801DA620 creates item layouts from layoutName plus `_item`, looking for sequential `ITEM%d` panes. Its first call is the inherited BUILayout array Load. The single-archive wrapper801DA5E0 creates a two-element path array and makes the virtual array call. The large Load also requires the native member-one bind/placeholder contract; canonical functional support is being coordinated separately with the base member-zero bind prerequisite.

## Traps

Do not infer a vector start from adjacent constructor zero stores. Element access and vector count/capacity operations prove13C and148. Likewise, distinguish weak owner identity caching from selection-index state by tracing shared_ptr return storage and weak assignment. Same-size callback destructors do not prove their function argument type. The840-byte destructor candidate remains rejected despite matching size and mostly identical instructions.
