# Issue 486: AX command lists

## F-486-1: Shared output-mode symbol

For `wpse-v1` / `orig/wpse-v1/main.dol`, `lbl_804B98B0` is the original SDK global `u32 __AXClMode`. The frozen manifest remains `config/targets/wpse-v1.json`.

Evidence is independently checked against [ogws AXCL.c](https://github.com/doldecomp/ogws/blob/f5a1bb5ed7692a9e09e737bc1236f592274dcf73/src/revolution/AX/AXCL.c): `__AXClInit` initializes this global to stereo (0), `AXSetMode` stores its argument, and `__AXNextFrame` dispatches modes 0, 1, and 2. Target `0x802DBC90` initializes the word with `stw r7,-16752(r13)` at `0x802DBCB4`, where r7 is zero. Target `0x802DBCE0` consists of `stw r3,-16752(r13)` and `blr`. Target `0x802DB344` loads that same word and branches on 0, 1, and 2. With the established r13 base `0x804BDA20`, the displacement resolves exactly to `0x804B98B0`. These observations corroborate AXAux's mode-2 comparisons documented by #485.

The symbol has four-byte `.sbss` storage with no file-backed offset. The initial dependency change renames only this symbol, preserving its section, address, size, and attributes. It does not reconstruct code or claim additional source coverage. AXCL begins inside #486 at `0x802DB290`; the rest of this issue remains to reconstruct. This dependency unblocks AXAux PR #553.

Validation: baseline configure/build succeeds. After the rename, `python configure.py`, `.venv/Scripts/ninja`, and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` all exit 0. The full DOL remains exactly identical. This symbol-only change adds no source units; objdiff coverage is unchanged. No new reusable tooling trap emerged.
