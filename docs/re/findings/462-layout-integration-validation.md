# F-462-2: Layout header integration gate failure and corrective verification

## Incident

PR #723 was locally verified at `0e69bef241991c092a0e2f8b7b17c3b5a348bb0b`, based on the #514 integration `a86c8e62`. Before merging, `origin/main` advanced to `4d00806e7665c11b574c6f846e09af96c35de0b3` with the independent #709 KPAD header prerequisite. The ancestry check exited 1. The agent incorrectly batched the subsequent merge as another call in the same orchestration block and did not stop after inspecting that failure. GitHub accepted the exact pull-request head and produced merge `f7bf8e2b4ae4430e4bfb8bcd654038769dcb789f`.

This violated the required latest-main local verification gate. The exact-head argument protects against a changed pull-request head; it does not establish that the base is still an ancestor. The merge was reported to the coordinator immediately and further merge actions were stopped.

## Corrective verification

The worktree was clean and moved directly to the actual merge commit, with no new source changes. A new output directory was configured, so the correction cannot accidentally validate an earlier default-output DOL:

```
git rev-parse HEAD
git status --porcelain
python configure.py --build-dir build/462-merged-f7bf8e2b
.venv/Scripts/ninja
python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/462-merged-f7bf8e2b/wpse-v1
```

The committed HEAD was the actual merge and status was empty before the build. Configure, Ninja, and the explicit frozen-manifest verifier each exited zero. The fresh DOL has SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`; the verifier validates both frozen hashes and direct byte identity. After discarding unrelated dtk metadata churn, status was again empty at the same committed HEAD.

The actual merge diff adds only `lyt_layout.h`, `lyt_types.h`, F-462-1, and the four-line symbol record. It changes no existing source, object registration, split, expected hash, or manifest. The real report retains 339 source-linked units and 708,960 source-linked code bytes; this header prerequisite adds zero source coverage. The native Layout constructor probe and complete fourteen-slot vtable evidence remain as described in F-462-1.

## Traps

- Run fetch, ancestry check, and merge in separate orchestration invocations. A failed ancestry check must stop the merge even when GitHub would accept the exact head. Do not batch a check with an action that depends on its success.
- A custom build's generated SHA-1 check can still point to the default build path. Only the explicit frozen verifier pointed at the new directory establishes the corrective artifact result.
