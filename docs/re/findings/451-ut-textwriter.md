# F-451-1: Complete TextWriterBase investigation

## Ownership and original inventory

Issue #451 originally lists 27 entries / 8,456 instruction bytes. Its first two entries, `0x8029A170` / `0x370` and `0x8029A4E0` / `0x2AC`, belong to the preceding CharWriter source unit. The #450 owner independently reconstructs that complete unit and audits both original #451 tails without duplicate byte credit. This issue does not take its split, configuration entry or class-header ownership.

TextWriterBase starts at its char constructor, `0x8029A790`, and ends after the initializer at `0x802A18C0` / `0x84`, before the debug-text source beginning `0x802A1950`. The retained inventory contains 81 functions / 28,584 instruction bytes: #451 contributes 25 / 6,892 and neighboring #452 contributes 56 / 21,692. The exact other section boundaries and source-selected acceptance remain to be established. #452 is not yet claimed or accepted by this investigation.

## Baseline and source provenance

The unchanged isolated #451 worktree at main `8a57c58c` passes configure, the complete fresh 517-target default plain Ninja build and explicit frozen main.dol verification, all exit zero. The frozen manifest covers main.dol only, excluding the boot executable, data contents and WAD packaging. No TextWriter source object is registered by this baseline.

The reference is CC0 zeldaret/ss commit `787b0b1e9d30e3c1d91ced61350b059ffc5a2411`. SHA-256: LICENSE `a2010f343487d3f7618affe54f789f5487602331c0a8d03f49e9a7c547cf0499`; TextWriter source `7e46e57702569ee2481b6aeab9872eb7b5a818add9dbd0c7ead281f5498ae619`; TextWriter header `1456c9d09f3f827972a35208cfcb433a80ff1e0486bbbd67ce43f53970f0caae`; CharWriter source `e6228fb2f8238b386e14a5e6a75a1f20bf353e49cefdc02fb81361874c5f6447`. No file-level license exception occurs in these files. The existing canonical NW4R interfaces remain authoritative; the #450/#808 owner approves temporary diagnostic reuse of the exact PR879 declarations, including the mutable/display-list methods and private CharWriter display-list setup helpers.

## First diagnostic result

The comment-stripped reference is compiled outside the repository with established Wii/1.1 NW4R flags and a bounded external header overlay. The first probe reproduces 53 of the 81 native functions / 16,992 instruction bytes with exact sizes and every non-relocation word. This includes both constructors, destructors, line-height methods, formatting functions, VPrintf methods, CalcLineRectImpl, CalcStringRectImpl, AdjustCursor and the static initializer. This is partial diagnostic evidence, not source-selected acceptance or decompilation credit.

The overlay preserves the PR879 mutable/display-list interfaces. Reference namespace-level tag operations use the canonical TagProcessorBase enum and PrintContext flag names. Canonical `__va_list_struct` replaces the reference's differently spelled structure alias. The reference's static formatter overloads reproduce the native four-byte direct branches to vsnprintf/vswprintf; the old bounded header's non-static declaration is not the actual forwarding ABI. Its signed format-buffer count also reproduces the native formatting clamp. Canonical field layout remains width limit `0x4C`, character spacing `0x50`, line spacing `0x54`, tab width `0x58`, draw flags `0x5C` and processor pointer `0x60`; the inherited CharWriter is `0x4C` bytes.

The two normal PrintImpl methods need the original inlined CharWriter rendering operations. The display-list methods and their emitted helpers are absent from this reference. Native normal rendering first obtains the glyph through Font slot `0x54`, then uses the per-character baseline virtual at slot `0x20`; the older source's outer baseline adjustment cannot simply be retained. Glyph rendering uses the canonical texture dimensions, 15-bit texture coordinates and GX texture-loading/quad operations. The #450 owner retains signoff over any proposed CharWriter inline-helper expansion.

## Traps

The prior blocked investigation used an older ogws reference and found large layout/printing differences. The pinned SS source already reproduces both complete line-layout and cursor-adjustment methods under the established flags; that older failure does not establish that every public reference lacks the core algorithms.

The original survey divides this source file across two issues and leaves its preceding CharWriter tails in #451. Keep their independent inventories and first-function ownership explicit; never count the neighboring tails or unretained compiler helpers twice.

The display-list body calls many accessor helpers out of line while the normal rendering body inlines them. No compiler-pragma or flag explanation is established yet. This observation does not authorize a production workaround or changed flags.
