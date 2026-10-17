# F-35-1: Five Wii linkers reproduce the frozen executable

Issue #35 is answered by a controlled comparison of the installed Wii/1.0,
1.0a, 1.1, 1.3 and 1.7 linkers. All five reproduce the complete frozen
`wpse-v1` DOL, both from original split objects and from the actual
source-selected build inputs. This does not uniquely identify the original
linker. The existing Wii/1.1 build policy remains valid, and
`config/toolchain.json` is unchanged.

## Inputs and method

The experiment starts at main `0508c558cda09065316a28086f746c4788591e51`. The real `python configure.py`,
plain Ninja and frozen-manifest verifier all exit zero before the experiment.
The generated Ninja link rule supplies 9,509 explicit object inputs in order.
Two response files preserve that order:

- `original-split`: all 9,509 original objects from dtk's `obj/` output.
- `source-selected`: the actual 9,509 build inputs, including 477 compiled
  source objects and the unchanged original fallbacks for remaining units.

The original-only list replaces each selected `src/` object with its existing
`obj/` counterpart. Every listed file exists. No compilation, object bytes,
linker script, linker flags, symbols, section ordering, or source selection is
changed between linker versions. The sole case-specific arguments are the
linker executable and separate output path. The generated linker script has
SHA-256 `8d6109d9ca76c492d56c65ba6ce1cd511c5a0074164bc232a48f27175329e220`.

Every ordered input record contains its worktree-relative path, byte size and
SHA-256. A final comparison checks all records and the linker script again
after the ten links. All inputs remain unchanged. Portable manifest digests,
using JSON arrays of these records with forward-slash paths, sorted keys and
compact separators, are:

- `original-split`: `fac811822f0bcc32bac1c353bc6817f04f985170a561f55e3cf02f672a49778e`.
- `source-selected`: `5f36c34c663776fe1ff7ff3915a4f480b67e91569cb0fb4a5ca0ad7ccbde2ecb`.

The binaries come from the existing digest-pinned compiler bundle in
`config/toolchain.json`, archive SHA-256
`69b0d4980a2eccea4ae6e70b486714b5190cb6c75e3bc7b8fb5878a8854d0d1c`.
Each executable's own `-version` command exits zero. dtk 1.8.4 converts each
resulting ELF to DOL, and the unchanged manifest verifier independently checks
every case's actual output directory.

## Results

| Linker directory | Executable banner | Original-only and source-selected DOLs | Linker SHA-256 |
| --- | --- | --- | --- |
| Wii/1.0 | Version 4.3 build 145 | Both pass | `511f03933dd01562ebd3174a403862ef55c20371b0ea25ef454829c28a7643dd` |
| Wii/1.0a | Version 4.2 build 142 | Both pass | `4abefbf70c469e721de57624e413cd299a9e1bc10b8954021ba3963274aa14ac` |
| Wii/1.1 | Version 4.3 build 151 | Both pass | `ad6746c6add523db1893c7351e27c535cf608d539313f8936b02df896bbc5a76` |
| Wii/1.3 | Version 4.3 build 172 | Both pass | `46483da7f98e9968c4a4ffd0dee69f9cc769737fbd0970632b1517a6dc275bfd` |
| Wii/1.7 | Version 4.3 build 213 | Both pass | `8518d2471e0b08a856c942fe1ffcabe8864b6b2571bbd277716c1b03c1996c17` |

All ten link commands, all ten ELF-to-DOL conversions and all ten frozen
verification commands exit zero. Every output has 4,094,880 bytes and
SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. Direct complete-file comparisons find no differing byte.
No normalization, patched output, ignored region, changed hash, or substituted
original executable is used.

The original-only controls emit no linker warnings. The source-selected cases
retain the generated script's existing warnings about inactive FORCEACTIVE
names; the parameters are not altered to suppress those warnings, and all
complete artifacts still pass exact verification.

The commands for each of the two ordered response files are:

```text
tools/local/compilers/Wii/<version>/mwldeppc.exe -version
tools/local/compilers/Wii/<version>/mwldeppc.exe -fp hardware -nodefaults -lcf build/wpse-v1/ldscript.lcf -o <case>/main.elf @<input-set>.rsp
tools/local/dtk.exe elf2dol <case>/main.elf <case>/main.dol
python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir <case>
```

The response files are the explicit input list of the generated
`build/wpse-v1/main.elf` Ninja rule, one object per line. Retain their exact
order; replace only `src/` with `obj/` for the original-only control. The
comparison scripts, full input manifests, linker logs, version outputs and
ten output directories are retained as issue-prefixed scratch evidence
outside the repository. The repository change is this findings document only.

## Interpretation and scope

F-0010 established Wii/1.1 as a working linker, without testing other releases.
These controls extend that fact to all four requested alternatives. They
exclude neither an earlier nor a later tested linker as the original. Linker
identity needs independent provenance or a discriminating original feature;
successful reconstruction alone cannot supply it. The experiment does not
change the independently evidenced per-library compiler choices of F-0007.

No new reconstruction unit was scoped, so no follow-up decomp issue is needed.
No functions, reconstructed bytes or matching units are added by this research.

Rebasing this documentation-only delivery onto main `2a763430` preserves the
experimental evidence above. The real configure, plain Ninja and unchanged
actual-output frozen verifier pass again with exit zero; the input/version
comparison remains explicitly scoped to its recorded `0508c558` baseline.

## Traps

- Successful byte-identical linking is not a unique linker fingerprint. The
  same frozen executable results from five distinct installed versions under
  identical inputs and flags.
- Ninja removes its successful link response file by default in this build.
  Preserve the ordered list before linking or recover the explicit inputs
  from the generated link rule; do not reconstruct order from a filesystem
  listing.
