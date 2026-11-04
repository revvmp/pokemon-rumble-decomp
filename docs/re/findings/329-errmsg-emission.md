# Errmsg final metadata ordering

Root completed the39 function bodies and their exact order using the existing geometry source and a second same-basename source at src/game/errmsg_main/errmsg.cpp. RTTI strings proved the private types belong inside NErrMsg, and original vtable relocations corrected swapped identical deleter destructor names. The geometry helper uses the evidenced mutable Pane pointer. The following OnHomeButtonSound entry requires16-byte alignment to preserve the trailing gap.

The final handoff had218 differing linked bytes. Source metadata placed the CRomFont shared-pointer deleter before CErrMsgFatalLoop, whereas the original puts the fatal-loop vtable and name first. A sizeof probe for std::tr1::detail::shared_ptr_deleter<CRomFont, std::detail::default_delete<CRomFont>> immediately before the complete CErrMsgFatalLoop definition requests that template specialization early enough to restore reverse metadata order. The same probe restores the retained small-data ordering of the button string and heap boolean.

The probe is a D-0016 emission-layout fake. It emits no instructions. Direct compilation of the second source with actual project flags preserves all36 owned functions at100% and in exact order; root's three geometry functions remain unchanged. Normalized target data is an ordered subsequence of the source object, with the remaining extra weak blocks subject to the real linker's normal discard/deduplication. Full acceptance requires the root's frozen-manifest build.

Scratch evidence is in ignored tools/local/session/329-errmsg, especially baseline.json and fontprobe.json. The preceding linked diagnostic is tools/local/session/errmsg-tail/linked4.elf/.dol. No shared header changes were made in this final pass.

## Trap

Moving ordinary method definitions is unnecessary when the remaining mismatch is RTTI/vtable order. In this unit, an early sizeof specialization request before the non-template class complete definition corrects retained metadata order without disturbing any function body or function order. This is a verified local timing mechanism, not a claim that arbitrary sizeof probes preserve code elsewhere.

## Source reconstruction and full-link acceptance

GetPaneRectangle's prologue mismatch was resolved honestly by restoring its mutable nw4r::lyt::Pane* parameter in source and declaration. The original accesses justify that signature; no scheduler pragma is retained for this fix. The recovered NErrMsg namespace follows original emitted type-name strings rather than stylistic preference. Original vtable destructor relocations, rather than identical destructor byte streams, establish the CDisable and CRomFont deleter identities.

The geometry helpers remain in src/game/errmsg.cpp, while the rest is src/game/errmsg_main/errmsg.cpp with the same basename. Separate compilation preserves the original duplicate floating-point constants while retaining anonymous-namespace spelling. The inferred alignment of the following OnHomeButtonSound entry is16, preserving the original eight-byte trailing gap; it is not fabricated instruction padding.

After the final sizeof probe was applied, root's actual ninja build exited0 and the frozen-manifest verifier returned PASS. Errmsg is accepted as Matching with all39 functions, linked data, exception metadata, and full executable SHA verified. The only new fake mechanism from this final pass is the explicit specialization-timing sizeof probe; the signature, namespace, destructor identities, constant ownership, and alignment corrections are evidence-based reconstruction fixes.
