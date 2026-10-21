# F-227-1: Main-loop continuation producers

Issue #227 retains 25 functions and 3,908 code bytes at `0x8012E940`–`0x8012F8F4`. The first-function owner of `main.cpp` is #225; #226, #228, and #230 own neighboring original inventories. No new source split or shared class header is proposed by this continuation.

The independently established `@unnamed@main_cpp@::CMain` contract is 44 bytes: `IGameLoopCallback` at zero, then shared owners of `ISession`, `CFiber`, `IDisconMsg`, `ISaveManager`, and `IStrapReminder` at offsets 4, 12, 20, 28, and 36. The native virtual methods appear in reverse interface declaration order, consistent with inline definitions and the established game IPA context.

| Native address | Method | Native size | Current producer |
| --- | --- | --- | --- |
| `0x8012EBD0` | Present | 4 | Exact instructions and direct target |
| `0x8012EBE0` | Render | 268 | Exact instructions and direct targets |
| `0x8012ECF0` | Draw | 152 | Exact instructions |
| `0x8012ED90` | Calc | 152 | Exact instructions |
| `0x8012EE30` | Prepare | 120 | Exact instructions and direct target |
| `0x8012EEB0` | ShouldRender | 160 | Two prologue instructions exchange order |
| `0x8012EF50` | Update | 244 | Exact instructions and direct targets |
| `0x8012F050` | ResetTiming | 8 | Exact instructions and direct target |
| `0x8012F060` | IsRunning | 8 | Exact instructions |

Eight methods reproduce 956 original code bytes. All 23 direct branch relocations across the nine methods resolve to the original targets. All six eight-byte exception records reproduce the original bytes. These are outside producer results, not whole-unit or issue acceptance. Both Wii/1.1 and Wii/1.3 retain the same two-instruction `ShouldRender` difference. A raw-pointer test removes the original shared-owner boolean conversion and is rejected.

`Render` draws the active map's pause menu through `IGameMap::GetPauseMenu`, whose native virtual slot is `0x58`. `Present` tail-calls `Graphics::DisplayFrameBuffer`; its decompiler view incorrectly appeared empty. `Update` gives the disconnection message priority over the pause menu and always processes the save manager and pending reset request afterward. Callback and subsystem names without recovered original spelling remain descriptive.

## F-227-2: Retained template identities

The native font map is `std::map<std::string, std::tr1::shared_ptr<nw4r::ut::Font>>`, as proved by the original RTTI string. Its insertion producer at `0x8012F080` allocates 32 bytes and constructs the 20-byte pair at node offset 12. The current canonical insertion producer has the native 600-byte extent with five differing instruction words; the tiny diagnostic's recursive destruction producer is 1,040 bytes versus native 912 and needs the actual complete translation-unit context. No map declaration changes follow from this diagnostic.

The clone functions at `0x8012F850` and `0x8012F8A0` store vtable `0x803B52E0`, identifying the plain callback `void (*)(const CLayout::CInitializeException&)`. Its invocation at `0x8012F8E0` therefore returns void and tail-calls the stored pointer. It is distinct from the neighboring HBM callback returning bool, whose invocation normalizes the result.

## Traps

- The source methods are inline in the original class context: individually exact out-of-line bodies alone do not reproduce physical function order. Preserve the native reverse-interface order in the complete producer.
- Identify callback invocation types through the actual vtable and RTTI before interpreting a short indirect tail call; the exception callback and HBM bool callback have different return contracts.
- Native font-map allocation is 32 bytes with value offset 12. Treating the allocator's value pointer as a node pointer would incorrectly suggest a 36-byte node.
