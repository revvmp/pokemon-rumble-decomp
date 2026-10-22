# Sprint cx2: deferred tree lookup instantiation

## Result and scope

The value-type `tree_imp::find_leaf` can be moved to the next instantiation round without changing its body, signature, inline status, or compiler flags. Move both `find_leaf` declarations before `insert_one`, leaving the insertion members before `insert_node_at`. The overload declarations remain together.

This is a partial result. COyaInfo is still NonMatching: the requested source-selected full verification did not pass. CLayoutManager's object did not change. The change does not establish that all key-type lookup instantiations are fixed.

## Declaration traversal

The deferred member traversal has a position within a class, not just among class-template groups. When it instantiates `insert_one`, a previously visited `find_leaf` declaration is deferred to another pass; the later `insert_node_at` can still be processed in the current pass. No additional lookup caller is necessary to explain this ordering for the value-type overload.

In COyaInfo, the old retained sequence begins with the tree destructor, `find_leaf`, `insert_node_at`, and `destroy`. The new sequence begins with the tree destructor, `insert_node_at`, and `destroy`; `find_leaf` follows the allocator/deleter/lock group. It still follows `recommend`, whereas the target places it before `recommend`.

## Validation on 2026-10-22

Baseline source revision: `670a7f055`. All work stayed in the current checkout on `agent-cx2`.

The requested commit could not be created: Git denied creation of the linked worktree's `index.lock` in the parent repository's metadata, outside the session's writable root. Staging and committing both failed. The changes remain uncommitted; nothing was pushed or merged.

- Recompiled all 866 compile commands exposed by the generated Ninja compilation database. The pre-existing report was stale: it described 4,333 units and 8,714 functions at 100%; the regenerated baseline describes 1,474 units and 12,698 functions at 100%.
- Recompiled all 107 source objects whose compiler dependency files name `MSL_CPP/detail/tree.h` after the declaration change. The report still has 12,698 functions at 100%. No individual function score changed; no previously perfect function was lost. COyaInfo retains all 26 perfect entries in `report.json`.
- Ten source objects changed: BModelObj, BWorld, CDlgCollection, CMsgBalloon, COyaInfo, CPpuEngine, MapPiiTable, CWorldObjStatDisplay, l10n_files, and ScriptWorldObj. None is marked Matching. Every existing Matching source object retained its baseline SHA-256.
- Deleted `build/wpse-v1/main.dol`, linked the rebuilt objects, regenerated the DOL, and passed the frozen-manifest verifier. The final DOL is 4,094,880 bytes with SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.
- Temporarily selected COyaInfo as Matching and repeated the link after deleting the DOL. Verification failed at file offset `0x9D63E`, with 6,228 differing bytes and unchanged length. Restored NonMatching, regenerated the configuration and report, and repeated successful final verification. No target hash or original artifact changed.

Ninja execution stalled before starting a child process, including a one-command diagnostic build in a separate build directory. Dry-run and tool modes worked. The evidence above comes from executing the actual generated compile commands, linker command and response-file arguments, DOL conversion command, and objdiff report command directly, checking their exit statuses. This is not a claim that a normal Ninja build completed. The direct baseline link also passed before any source change.

The acceptance command was:

```powershell
python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1
```

Local investigation outputs are under `build/cx2-investigation/`. The final receipt is `build/wpse-v1/verification.json`; the rejected COyaInfo receipt is `build/wpse-v1/oya-verification.json`.

## Rejected broader experiments

The additional orders described in [sprint w1o](sprint-w1o.md) remain real blockers. A diagnostic combination of include-order changes, out-of-class vector wrappers and `recommend`, and an earlier `red_black_tree` forward declaration moved all retained COyaInfo functions into target relative order except `__vec_split_buffer::allocate`. These broader changes were reverted; they were not accepted or subjected to a complete shared-header regression gate.

Moving allocation into a new split-buffer constructor emitted that constructor and lost the native constructor match. Reversing the two- and three-argument allocation forwarding direction emitted the other overload and also lost the constructor match. A 32-variant sweep of split-buffer declaration/inline choices did not resolve allocation's placement. No attributes, assembly, binary patches, or linker-order workarounds were added.

## Next actions and traps

Keep COyaInfo NonMatching. Investigate how the split-buffer allocation request becomes later than its destructor and construction operations while preserving the native constructor instructions. Retest key-type `find_leaf` separately, especially CLayoutManager. Any broader solution must repeat the all-source score gate and source-selected exact verification.

- For deferred non-inline template members, test whether the requesting member appears before or after the helper: a request for an already-visited member can wait for another pass. This was verified for the value-type lookup, not every member-template overload.
- A copied report can describe another split inventory. Regenerate it from freshly compiled source objects before using its perfect-function count as a regression baseline.
