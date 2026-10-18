# F-385-3: Complete smooth-stripe issue inventory

Target: `wpse-v1`, original issue #385. This independently accepts all 17 original functions, 13,224 instruction bytes, after the coordinated source acceptance in PR #908. It adds no function implementation or source-byte credit. All original addresses and sizes are preserved.

## Accepted implementation

[F-383-4](383-ef-stripe-native.md#f-383-4-source-selected-stripe-and-coordinated-smooth-stripe-acceptance) and [F-384-3/F-384-4](384-ef-smooth-stripe-strategy.md) provide the shared-header and full-unit acceptance evidence. The existing const particle-link getters forward to their existing nonconst overloads; no overload is added. That equivalent interface implementation reproduces both independent loop-offset loads in `CalcAhead_ParticleBoth_Origin` without changing its body. The ordinary empty `Vertex()` definition placement preserves the retained weak constructor order. The original #385 function bodies are canonical and unchanged by this acceptance audit.

The prior F-385-2 result was diagnostic and is superseded: all 28 retained functions / 20,196 bytes in `ef_drawsmoothstripestrategy.cpp` now compare at 100%, and the unit is selected from source. All five sections are 100%. Its native text is `0x80245EF0` to `0x8024AE80`; initialized vtable/constants are 28/52 bytes, BSS48 and SBSS4. Thirteen additional compiler-emitted weak copies are discarded in the link and receive no credit. The compiled source object therefore has more raw text than the retained native unit; the final ELF/map addresses and full executable are the acceptance evidence.

## Original issue inventory

| Native address | Original size | Function | objdiff | Final ELF/map |
| --- | --- | --- | --- | --- |
| `0x80247A70` | `0x418` | `DrawSmoothStripe_BSpline_ConnectRing` | 100% | Exact |
| `0x80247E90` | `0x49C` | `DrawSmoothStripe_BSpline_ConnectEmitter` | 100% | Exact |
| `0x80248330` | `0x4C` | `GetYoungestDrawParticle` | 100% | Exact |
| `0x80248380` | `0x38` | `Vertex constructor` | 100% | Exact |
| `0x802483C0` | `0x3C` | `Vertex assignment` | 100% | Exact |
| `0x80248400` | `0x2F4` | `DrawSmoothStripeTube_BSpline_ConnectNone` | 100% | Exact |
| `0x80248700` | `0x3A8` | `DrawSmoothStripeTube_BSpline_ConnectRing` | 100% | Exact |
| `0x80248AB0` | `0x57C` | `DrawSmoothStripeTube_BSpline_ConnectEmitter` | 100% | Exact |
| `0x80249030` | `0x7C8` | `DrawSmoothStripeTube` | 100% | Exact |
| `0x80249800` | `0xA18` | `Draw` | 100% | Exact |
| `0x8024A220` | `0x2AC` | `GetInitialPrevAxis` | 100% | Exact |
| `0x8024A4D0` | `0xCC` | `GetCalcAheadFunc` | 100% | Exact |
| `0x8024A5A0` | `0x11C` | `CalcAhead_Particle_Stripe` | 100% | Exact |
| `0x8024A6C0` | `0x278` | `CalcAhead_ParticleBoth_Stripe` | 100% | Exact |
| `0x8024A940` | `0x2C8` | `CalcAhead_ParticleBoth_Ring` | 100% | Exact |
| `0x8024AC10` | `0x228` | `CalcAhead_ParticleBoth_Origin` | 100% | Exact |
| `0x8024AE40` | `0x40` | `DrawSmoothStripeStrategy destructor` | 100% | Exact |

## Independent checks

The original issue table was joined to the fresh `build/wpse-v1/report.json` by native address. Every row has its original size and 100% instruction comparison. `powerpc-eabi-readelf -sW build/wpse-v1/main.elf` confirms every address and size in the actual linked ELF. The owning contribution in `main.elf.MAP` is `ef_drawsmoothstripestrategy.o` for every retained function, including the earlier local `GXEnd` distinguished by its native address rather than name alone.

`ninja -t query build/wpse-v1/main.elf` includes `build/wpse-v1/src/nw4r/ef/ef_drawsmoothstripestrategy.o` and excludes its original `obj/` fallback. All 28 retained functions are checked, not just the issue's 17. All five allocated sections compare at 100%; source selection is confirmed in both configuration and the actual link dependency graph. Exact comparison and the frozen verifier confirm the final executable. Original files, manifests, hashes, compiler flags, source, headers, and splits are unchanged by this audit.

Commands, each exit 0:

```
python configure.py --map
.venv/Scripts/ninja
python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1
python tools/re/unitdiff.py nw4r/ef/ef_drawsmoothstripestrategy.cpp
```

The DOL was removed before rebuilding, and Ninja completed successfully before verification. The complete original-inventory/ELF/map/source-input audit also exits 0. Final acceptance is repeated through `tools/collab/land.py` against the merge head.

## Traps

- **Check accepted shared-interface updates before retaining a local compiler workaround.** A private volatile-list snapshot experiment reproduced the old origin function's instructions, but did not establish the original qualifier and was discarded. The accepted getter-forwarding implementation reproduces the complete source-selected unit while retaining the original function bodies. Binary identity alone does not justify a new semantic qualifier when an accepted ordinary interface implementation already matches.
- **Raw source-object text can include discarded weak copies.** Check retained functions, relocations, source inputs, final ELF/map contributions and frozen DOL; do not count the thirteen extra emitted weak copies as reconstructed native functions in this unit.
