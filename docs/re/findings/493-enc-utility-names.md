# F-493-1: ENC utility symbol names

Target `wpse-v1`, frozen executable manifest `config/targets/wpse-v1.json`. This bounded dependency change names three functions within #493 to allow the source-reconstructed Latin unit in #494 to link. All other work in #493 remains fallback; no new source coverage is claimed by this naming change.

| Address | Size | Previous label | Name |
| --- | --- | --- | --- |
| `0x802EA030` | `0xF8` | `fn_802EA030` | `ENCiCheckParameters` |
| `0x802EA130` | `0x34` | `fn_802EA130` | `ENCiCheckBreakType` |
| `0x802EA170` | `0x100` | `fn_802EA170` | `ENCiWriteBreakType` |

Names have high confidence from [wii-ipl encutility.c](https://github.com/koopthekoopa/wii-ipl/blob/f4c09f4001c5a0b9af734ad1e3f80a54c5e5723c/libs/RVL_SDK/src/enc/encutility.c), declarations at the same pinned revision, and the target's call sites and instructions inspected with binutils 2.42-2. The reference is CC0-1.0 with no additional notice on the consulted ENC files. No reference source is copied in this change.

The parameter checker takes eight PowerPC integer arguments: destination validity, input/output destination counts, output destination validity, source validity, input/output source counts, output source-limited flag. The two break helpers accept unsigned current/next characters and destination pointer/character width/break enum/destination validity respectively. These call signatures are exercised by the two Latin generic converters reconstructed in #494, whose object code matches 100%.

All functions are in `.text`; DOL file offsets are runtime address minus `0x8004FD40`. Symbol addresses, sizes and attributes remain unchanged. No split or source registration changes occur. No new reusable tooling trap emerged.

Validation commands: `python configure.py`, `.venv/Scripts/ninja` and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1`. Results are recorded in the pull request after execution.

During the final rebase, the incoming `docs/re/SYMBOLS.md` at `origin/main` contained byte `0x96`, causing strict UTF-8 decoding of `git show origin/main:docs/re/SYMBOLS.md` to fail. The conflict resolution preserved incoming bytes and appended only this issue's ASCII section; no global encoding conversion was performed. New files remain UTF-8. This is an observation of that revision, not evidence that all project Markdown uses another encoding.
