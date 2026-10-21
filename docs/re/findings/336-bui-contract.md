# F-336-4: Complete BUI and BUILayout base contract

Part of #336, coordinated with the #327 first-function owner. This separate header prerequisite enables CUIList reconstruction; it does not select BUI source, change its split, or claim completion of #327 or #336.

## Complete storage and inheritance

BUI's native constructor at `0x801C8700` and destructor at `0x801C8900` establish complete size `0x4C`. RTTI `0x804B6408` names BUI at `0x804B6400`. Its base list at `0x8039E384` identifies `enable_shared_from_this<BUI>` at offset four, ICalc at zero, and ICalcDraw at zero. The declaration uses the existing canonical interfaces and shared-owner implementation.

| Offset | Size | Type and evidence |
| --- | --- | --- |
| `0x04` | `8` | Inherited weak self owner; native cleanup calls release_weak |
| `0x0C` | `4` | Flags initialized to six, later bit operations |
| `0x10` | `4` | Raw parent BUI pointer; GetManager follows its virtual parent chain |
| `0x14` | `12` | `list<shared_ptr<BUI>>` children; constructor, child insertion/removal, traversal and destructor |
| `0x20` | `32` | Four `weak_ptr<BUI>` navigation links; array construction/destruction, weak locking at `0x801C95B0`, and navigation recursion |
| `0x40` | `4` | Direct-delete owner of a callback list; focus changes invoke each callback on old and new focused BUI objects |
| `0x44` | `4` | Direct-delete owner of a callback list; visibility change at `0x801C8D10` invokes its callbacks |
| `0x48` | `4` | Direct-delete owner of the same callback-list representation; precise event identity not recovered |

Each callback list has the canonical `list<shared_ptr<function<void(BUI*)>>>` node and owner representation. Native RTTI supplies the function signature, and native list traversal calls its function interface with the BUI pointer. The private CCallbackListOwner models only observed null initialization, pointer access and direct deletion. It is noncopyable and provides no inferred transfer operation. Its spelling, source identity and member spellings are descriptive inferences, not recovered original names. The general mEventCallbacks name deliberately leaves the last list's precise event semantics open. No opaque fields or synthetic padding are used.

The third payload is supported by retained cleanup identity, not merely identical destructor instructions: native BUI destructor EH at `0x8003BF4C` repeatedly names the same retained owner destructor `0x800DA1E0` for member offsets `0x48`, `0x44`, and `0x40`. The first cleanup chain's records begin at table offsets `0x94`, `0xA0`, and `0xAC`. The latter two owners have direct callback invocation evidence. Independently compiling the descriptive owner's destructor reproduces all 240 bytes and seven instruction targets of `0x800DA1E0`.

BUILayout inherits BUI and embeds complete canonical CLayout at `0x4C`, establishing size `0x134`. Native constructor `0x801C9EE0` calls both actual base/member constructors; destructor `0x801C9F30` calls CLayout then BUI cleanup. RTTI `0x804B63F8` and base list `0x8039E360` establish the inheritance independently.

## Virtual and load interfaces

BUI vtable `0x803C0250` has nine methods: destructor, Calc, CalcMtx, CalcView, Draw, GetManager, attachment hook, detachment hook, and input handling. Its first five slots match canonical ICalcDraw. The input object is canonical IUIInput; native input handling calls its GetTrig slot to select among the four navigation links. Hook/input method names and const qualification remain inferred where original mangled evidence is absent.

BUILayout vtable `0x803C0338` overrides destructor, Calc and Draw and adds four Load overloads in native order. Slots `0x2C/0x30` forward single-archive and archive-array loads, including the background boolean, to CLayout `0x801CC350/0x801CCBD0`. Slots `0x34/0x38` supply false and dispatch through the corresponding preceding virtual. Both forms take a layout name and a const-reference shared FontMap owner. The additive single-archive CLayout declaration is supported by its wrapper and formatter; `0x801CABD0` independently confirms that the array form traverses pointers until null and makes a string for each pointee.

Existing GetLayout remains nonvirtual and unchanged: native `0x8008B7D0` returns `this+0x4C`, and CMsgBalloon calls it directly. The tail-forwarder at `0x801C9FA0` is a single-archive Load overload, not GetLayout. Its eight-byte size alone had initially suggested the wrong interpretation.

## Native and current-consumer controls

Independent ordinary C++ probes under Wii/1.1 and unchanged game flags reproduce twelve BUI/owner functions: 1,900 instruction bytes and all 45 resolved instruction targets. These include the complete 148-byte constructor and 980-byte destructor, four child traversal methods, GetManager, the two hooks and the 240-byte callback-list owner destructor. A separate BUILayout probe reproduces eight functions: 304 instruction bytes and all ten resolved targets, including constructor, destructor, Calc, nonvirtual GetLayout and the four Load forwarders. These are interface and lifetime controls; complete unit layout/source-selection acceptance remains with the eventual source work.

Complete constructor/destructor exception tables also match frozen DOL bytes after independently resolving the cleanup targets: BUI constructor 52 bytes, BUI destructor 644 bytes, BUILayout constructor 28 bytes and BUILayout destructor 28 bytes. This verifies automatic member cleanup order and the shared retained callback-owner cleanup at all three offsets.

The generated complete BUI and BUILayout vtables match all 44 and 60 native bytes after independently resolving their entries. Both eight-byte RTTI objects, original name strings and complete base arrays also match native data. Layout compilation measures BUI `0x4C`, BUILayout `0x134`, and every member offset in the table.

Baseline source head `ed814e64` has 714 actual Ninja all_source objects. With the complete declarations and single-archive overload, all 714 compile. 712 whole objects are identical; CMsgBalloon and CNoticeDisplay change only generated anonymous local symbol numbers. Their code, data, exception tables, relocation records and normalized symbol attributes are identical. This explicitly preserves the existing nonvirtual getter and motion/visibility consumers.

After rebasing onto `c7c1d0daf7f9825bd98f00bd9f6c1c83a39aa026`, the complete current all_source set has 715 objects, including the newly source-selected CBossPii. All compile. Actual Ninja dependency records identify exactly three consumers of the modified headers: CMsgBalloon, CNoticeDisplay and CBossPii. Each current source was independently compiled against that main revision's prior headers. Every code/data/EH/relocation section and normalized symbol attribute is unchanged, explicitly preserving the new accepted boss unit as well as both older consumers. The remaining current objects do not depend on the modified headers.

`python configure.py --map`, `.venv/Scripts/ninja all_source`, the native/layout/vtable/current-consumer audits, `.venv/Scripts/ninja` after removing the old DOL, and frozen-manifest verification exit zero. The official landing workflow verifies the actual rebased merge head separately.

## Traps

Four eight-byte null-initialized owners are not necessarily shared callback owners. The navigation array releases weak references and locks BUI pointers; identify both its cleanup operation and its consumers.

Direct-delete ownership does not recover a standard-library wrapper identity. The current Metrowerks move_ptr, including an independently proposed pointer-cache variation, makes BUI cleanup twelve bytes too large. A reference auto_ptr declaration has a destructor throw specification that adds 48 bytes and exception handlers. Neither is published as BUI's recovered owner type. The descriptive private owner preserves the observed operations without inventing a library revision or transfer semantics.

An eight-byte method at the end of a virtual table may be a tail-forwarder. Decode its branch destination before assigning it a getter name or changing an existing nonvirtual interface.
