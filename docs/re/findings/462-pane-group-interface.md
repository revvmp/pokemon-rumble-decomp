# F-462-4: Pane and Group interfaces for group animation binding

Issue #462's final routine at `0x802B1790` is the free function `nw4r::lyt::BindAnimation(Group*, AnimTransform*, bool, bool)` in `lyt_util.cpp`, not a Group member. It walks the group's pane list and calls `Pane::BindAnimation` through vtable offset `0x44`. The reconstructed 0x70-byte function matches every non-relocation instruction word at that address using the existing Wii/1.1 NW4R flags.

The prerequisite headers add the complete member layouts and virtual interfaces required by this call. They do not take the Pane or Group source units, their splits, or their compiler registrations: Pane begins at `0x802A4BC0` in #456 and Group at `0x802A5D00` in #457. Their source ownership remains with those issues. The constructor, field, and virtual-method names follow the CC0 zeldaret/ss headers `include/nw4r/lyt/lyt_pane.h` and `lyt_group.h` at commit `57a709b2128eb163a18e2969d561c9abb7d1fcee`. The utility function follows that revision's `src/nw4r/lyt/lyt_util.cpp`. No comments, implementation stubs, or unrelated utility functions are imported.

## Layout evidence

| Type | Size | Members and evidence |
| --- | --- | --- |
| `detail::PaneBase` | `0x0C` | vtable pointer, then 8-byte `ut::LinkListNode` at `0x04`; constructor `0x802A4BC0` clears node words at `0x04` and `0x08` |
| `Pane` | `0xD8` | parent `0x0C`, child list `0x10`, animation list `0x1C`, material `0x28`, translation `0x2C`, rotation `0x38`, scale `0x44`, size `0x4C`, local matrix `0x54`, global matrix `0x84`, extended-user-data list `0xB4`, alpha/global alpha/base position/flags `0xB8–0xBB`, 17-byte name `0xBC`, 9-byte user data `0xCD`, allocation flag `0xD6`, padding `0xD7` |
| `detail::PaneLink` | `0x0C` | list node `0x00`, target Pane pointer `0x08`; `BindAnimation` loads the target from node offset `0x08` |
| `Group` | `0x2C` | vtable pointer, list node `0x04`, pane list `0x0C`, 17-byte name `0x18`, allocation flag `0x29`, two padding bytes `0x2A` |

Pane's resource constructor copies translation through size to `0x2C–0x50`, clears parent/material/extended-user-data at `0x0C/0x28/0xB4`, writes the allocation flag at `0xD6`, and terminates the name and user-data buffers at `0xCC` and `0xD5`. Its stored vtable is `0x803C6E70`. The reference virtual order places `BindAnimation` at `0x44`, which is directly confirmed by the utility function's `lwz r12,0x44(r12)` call. Remaining virtual declarations use the reference's order and names; no new source matching is claimed for them.

Group's pane-list size word is at `0x0C` and sentinel at `0x10`: the native utility routine loads the first node from `0x10` and compares it with `this + 0x10`. Using the header's `GetPaneList`, `PaneList::Iterator`, `PaneLink::mTarget`, and virtual `Pane::BindAnimation` produces all 28 native instruction words, with only actual ELF relocation words excluded from the instruction comparison. This is interface evidence; full source-linked DOL acceptance belongs to the subsequent utility source change.

## Traps

No new trap was found while reconstructing these interfaces. Public reference declarations are retained separately from claims that their source functions match this executable.
