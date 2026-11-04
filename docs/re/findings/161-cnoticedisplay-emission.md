# CNoticeDisplay source emission and data layout

## 2026-11-04 source candidate

The initial 99/102 report understated source instruction coverage. The remaining three Init functions differed only in pooled-data offsets and inferred R_PPC_NONE symbol grouping. The source candidate retains their actual algorithms and restores the original data layout.

Making only Init, Run, and their helpers inline did not move the literal pool. The outer virtual Show method also needed an inline declaration and definition. This lets the shared_ptr_deleter<CNoticeDisplay> vtable, type name, and hierarchy precede the function literals. The area-pane array belongs at data offset 0xA8, followed by the deleter vtable at 0xCC, type name at 0xE0, hierarchy at 0x180, and first member-function pointer at 0x18C. The stage path then starts at 0x198. Moving the array definition early initially replaced an absolute lis/addi access with an r29-relative pooled access; declaring the same C-linkage array through AreaPaneSymbols preserves one storage definition while restoring the original addressing.

The narrow CNOTICE_EMISSION_LAYOUT memory-header gate defers cross-type shared_ptr assignment and operator-> definitions into this translation unit. The unchanged assignment body is defined out of class at the end. A genuine source specialization for shared_ptr<BNoticeDisplayImpl>::operator-> places its body immediately after BNoticeDisplayImpl::Draw. All other arrow specializations use the original generic body. The four StartNotice bodies are ordered CLose, CFight, CSimpleText, CYouWin to reproduce the later function_imp instance order. Inline constructors, implicit derived destructors, and reversed declaration order for selected overriding methods reproduce the virtual-only emission wave without changing inherited virtual slots.

The last apparent function-order mismatch was an inferred-name error, not a compiler issue. Original CFight's vtable at 0x803A5CD0 has its destructor relocation to 0x800D3ED0; original CLose's vtable at 0x803A5D38 has its destructor relocation to 0x800D3E60. Existing symbols named these identical 0x6C-byte bodies backwards. Correct mapping is CLose::~CLose at 0x800D3E60 and CFight::~CFight at 0x800D3ED0.

The isolated compiler candidate tools/local/session/notice-tail/ready.cpp preserves all 102 instruction streams and the true original function order after this name correction. Three objdiff scores remain 99.89978, 99.81707, and 99.974335 because the original large pooled .data symbol has stale grouping; identical immediate values still carry different R_PPC_NONE symbol/addend identities. This isolated result does not establish a full link match. Parent-agent full build and frozen-hash verification are pending.

## D-0016 accounting

Treat the translation-unit memory gate, explicit arrow specialization, C-linkage namespace view, and emission-motivated inline/declaration rearrangement as a documented fake emission arrangement. They contain real C++ bodies and preserve runtime behavior; no original executable bytes are injected. The namespace view denotes the same single array, and the generic template bodies retain their original implementation.

## Traps

- When two identical destructors appear swapped, inspect original vtable relocations before trying to change compiler emission. Here both inferred names were reversed, and source class order was already correct.
- Explicit destructor declarations or an unused inline helper that calls destruction move the destructor into the earlier virtual-method wave. Moving an implicit destructor's constructor definition does not change its late order.
- Deferring a virtual method body out of class does not change its first-declaration position in the vtable-only emission wave.
- A strong dummy caller can force the virtual-method order, but also changes template instantiation rounds and literal ownership; the rejected candidate regressed all four Init functions and the late template groups.
- CodeWarrior accepts pool_data, but the tested scoped off setting did not prevent the early area-array definition from using pooled addressing. pooldata, usedatapool, and options pool forms were illegal pragmas, verified with illpragmas warnings.

The final include-order probe places `<string>` before `CNoticeDisplay.h`. This restores the original global RTTI/vtable ordering, including INoticeDisplay and ICalcDraw before the exception classes, and emits the char-array shared_ptr deleter before the four function_imp records. All non-relocation bytes of the original .data extent now compare exactly against the candidate (candidate ends six bytes before the original alignment boundary). Original function-name correction also makes the complete 102-function ordered list identical. Direct compiler probe: tools/local/session/notice-tail/stringfirst.o; comparison: notice-datacmp.py and notice-orderdiff.py. Full linked verification remains the acceptance requirement.

## Final four-byte linked residual

The first complete linked candidate after the include correction differed from the frozen DOL in exactly four bytes. One byte was a real source literal error in CYouWin::Run: the final CreateMotion call used pane `TEXT`, but original relocation @17528 points to the five bytes `MISC\0`. The retained call uses `MISC`.

The other three bytes were one protected-range entry in CYouWin::Init's exception table at 0x800201FC. Original range start/length are 0x614/0x14; source used 0x58C/0x36, with the same ending address and cleanup action. These are return-PC ranges: the extra source range covers the call to ceil, whereas the original starts at the subsequent GetMessageText return. A source-local NoticeMath C-linkage declaration of ceil with `__attribute__((nothrow))`, followed by the same float-result conversion as ceilf, restores the target entry without changing any instructions. A throw() exception specification alone compiled but did not change this range; __declspec(nothrow) was rejected. The retained attribute correctly states that this mathematical function does not throw C++ exceptions and does not alter the shared math header.

This local no-throw function view is included in the D-0016 emission arrangement. It continues to call the ordinary linked ceil implementation; no wrapper body, executable bytes, or exception-table bytes are injected. The final source correction is awaiting the parent agent's fresh full build and frozen-hash result.

The parent agent's regenerated global report now measures all 102 CNoticeDisplay functions at 100%. Regenerated dtk relocation resolution removed the stale three pooled-data score differences; no additional data split is required.

The final literal correction applies only to the terminal CreateMotion(...)->Wait() call. The earlier pMotion fallback assignment uses the same argument prefix but must retain TEXT. A broad initial replacement changed both; the next linked comparison isolated the extra one-byte difference at 0x800D2D8F, and that earlier call was restored. The same linked comparison confirmed the exception table is now exact.

## Accepted result

After the targeted earlier-call restoration, the parent agent ran a fresh full ninja build with CNoticeDisplay configured Matching: exit 0. The frozen verifier passed for the complete rebuilt DOL. The regenerated report measures 102/102 functions at 100%. This accepts the source-linked unit, including actual data, exception tables, and literal targets; it is no longer a fallback-only function-score result. No expected hashes were changed.
