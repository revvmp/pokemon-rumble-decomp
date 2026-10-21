# F-329-3: CLayout attachment owner correction

Part of #329. This corrective header prerequisite adds no reconstructed-function credit.

## Native type evidence

At `0x801D5130`, the receiver is the parent CLayout, the second argument names a pane and the third is another CLayout. Native code finds the SDK pane through the parent Layout at `+0x48`. It checks the child attachment owner at `+0xDC`, allocates a twenty-byte attachment object at `0x801D51A4`, and creates a sixteen-byte shared control block. The control block receives vtable `0x803C0640` at `0x801D5204–0x801D520C`. The allocated attachment and its control are assigned to the child CLayout at `0x801D5428` and following instructions.

That vtable points to RTTI `0x804B83A0`; its name pointer is `0x803C0654`. The exact native name is `std::tr1::detail::shared_ptr_deleter<CLayout::CAttach, std::detail::default_delete<CLayout::CAttach>>`. This directly establishes `shared_ptr<CLayout::CAttach>` at DC/E0. The descriptive member name is `mAttach`.

The hierarchy traversal at `0x801D40D0` reads the attachment through CLayout `+0xDC`, follows its parent CLayout pointer at offset zero and its SDK pane pointer at offset four. Construction initializes the remaining twelve bytes as tree storage, not a list. This prerequisite forward-declares the native-named nested CAttach without claiming an unproved complete tree key/value contract or changing its implementation.

## Superseded interpretation

F-329-2 originally attributed nearby initialization-exception callback RTTI to DC/E0 without tracing its allocation and assignment. That attribution was wrong. It is superseded here explicitly. Both owners occupy eight bytes, so the earlier E8 size and offset measurements, native caller interfaces, and consumer preservation do not establish the earlier semantic type. CInitializeException remains a separate native exception type used by Load.

## Validation

All 715 actual Ninja all_source objects compile before and after this correction on main 3bb879c7c2. Every complete object is byte-identical, including all relocation, EH, data and symbol sections. Ninja rebuilt the three actual header consumers CMsgBalloon, CNoticeDisplay and CBossPii; the other 712 objects have no changed dependency. The official landing workflow independently rebuilds and verifies the exact rebased head against the frozen DOL manifest.

## Traps

A nearby template RTTI name and matching owner size do not identify a member. Follow the allocated payload through its specific control-block vtable and into the destination member. Tree construction at attachment offset eight also rejects the initial child-list hypothesis; no complete attachment container declaration is published without its key and mapped-type evidence.
