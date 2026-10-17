# F-327-1: BUI and CUIManager native interfaces

Target wpse-v1, immutable original main.dol. Issue327 lists24 retained functions/4,240 instruction bytes. The unchanged9fdce911 baseline passes configure --map, plain Ninja and explicit frozen verification, all exit0. Source and metadata remain unchanged while interfaces are established. Analysis uses only an external copy of the tracked IDA database,327-reference/327-main.elf.i64; decompiler output is an outside reading aid, never reconstructed source.

BUI starts at801C8700. Its vtable803C0250 names RTTI804B6408, whose original string is BUI. The base list8039E384 explicitly identifies std::tr1::enable_shared_from_this<BUI> at offset4, ICalc at0 and ICalcDraw at0. The20 original rows before801C9680 total3,824 bytes. CUIManager begins at801C9680, has vtable803C02DC/RTTI804B8320 and the original name CUIManager, derivesBUI and adds an eight-byte weak-reference record at4C. The four CUIManager rows in327 total416 bytes; its source continues into328.

## Observed BUI storage

| Offset | Native evidence |
| --- | --- |
|0| ICalcDraw virtual pointer |
|4| Eight-byte enable_shared_from_this<BUI> weak-reference base |
|C|32-bit state flags, initially6 |
|10| Parent BUI pointer, initially null |
|14| Twelve-byte list: count, previous/next sentinel; node values hold eight-byte shared references to BUI |
|20| Four eight-byte weak neighbor references |
|40,44,48| Three initially-null owning pointers to callback lists; precise owning-wrapper template not established |

BUI occupies4C bytes. CUIManager occupies54. Callback-list nodes contain a function-wrapper pointer at8 and its shared control pointer atC. The function wrapper has a four-byte implementation pointer; invoking an empty wrapper throws the original std::tr1::bad_function_call, otherwise it dispatches through implementation virtual slot14 with BUI*. This supports list<shared_ptr<function<void(BUI*)>>> values; the native ownership of the three outer list pointers still needs template-identity evidence before publication.

BUI virtual order is destructor801C8900, Calc801C9410, inherited hooks801C9470/801C94D0, Draw801C9540, manager lookup801C8CE0, attach/detach hooks801C90D0/801C90E0, and input handling801C92A0. The two inherited hook roles and original names are unresolved. Their canonical base bodies80063DC0/80063DD0 are empty, and their overrides forward corresponding methods to children. The second hook and Draw are gated by state bit2. The manager getter walks parent links through virtual slot1C; CUIManager overrides it with801C9760, which returns its unchanged this pointer. Input handling consumes IUIInput::GetRepeat and follows weak neighbor links, using state bit8 as a traversal-cycle guard.

## Standard-container evidence

Lists have a count at0 and an embedded previous/next sentinel at4/8. Shared-reference nodes occupy16 bytes, with previous/next pointers at0/4 and object/control pointers at8/C. Range destruction unlinks the complete inclusive-node range before destroying nodes and decrementing count. These observations will guide an independent ordinary C++ implementation; no restricted MSL implementation is reused. Initial template-only diagnostics emit no unused functions under the actual IPA profile. A naturally compiled BUI constructor now reproduces all non-relocation words at the native148-byte extent; its enable_shared_from_this destructor and weak-neighbor default constructor likewise agree at100/16 bytes. These264 bytes remain diagnostic until relocation/data/full-source acceptance. Node-sentinel initialization as a self-initializing node reproduces the observed member-store order. Declaring the shared-reference destructor with its standard nonthrowing exception specification produces the native unexpected-handler profile and220-byte list destructor extent; the remaining list-loop differences are unresolved. No compiler flag or forced ordering helper was introduced.

## Hook-role evidence

Independent native derived-class analysis finds the first additional rendering hook prepares world-model state: BWorld80062980 calls CalcWorld, CalcVtx and CalcMaterial. The next hook can perform a pre-draw pass: CMiniMap800C30D0 invalidates GX caches and configures depth state, while CMsgBalloon800C7100 dispatches drawing and extra overlays. PrepareDraw and DrawBefore are behavior-supported descriptive names, not recovered original spellings. Native main Draw remains separate.

## Traps

- A leaf BLR is not necessarily void. IDA labels801C9760 as empty, but the original CUIManager virtual manager getter returns this directly in r3; BUI's recursive manager lookup and caller use prove the return value. Verify virtual callers before adopting a decompiler prototype.
- Callback-list values are shared references to function wrappers, not embedded function wrappers. The node's control pointer atC is released during destruction; invocation first follows the object pointer at8 and then the function implementation pointer. Preserve both indirections.
- Empty native hooks do not establish their semantic names. Native ICalcDraw order is known, but neither a forwarded float argument nor invented hook roles are justified. Calc currently has no proven argument; the display's apparent incoming FPR use was paired-single save disassembly.

- A reused smart-pointer destructor address does not establish a non-template storage base or the pointee type. The public reference interface stores the typed pointer and common control directly; target bodies can be shared across equivalent template instantiations. Recover identity from actual typed construction/caller evidence and prove generated calls independently.
