# F-166-3: Primary fill replacement and resize contracts

The recovered 2006 MSL `string` provides two `resize` overloads and counted-character `replace`. These are dependencies of the numeric-input producer in #166 and the formatter producer in #347. This prerequisite adds their ordinary out-of-class definitions and the counted `char_traits::assign` overloads. It changes no source-unit selection or source coverage.

The two-argument resize grows through counted replacement, or shrinks with an explicit zero count and zero character. The one-argument resize is inline and delegates with a value-initialized character. Preserve that original distinction even though the shrinking replacement does not insert its character argument.

Counted replacement uses the existing `max_size()` and `recommend()` contracts. The latter produces sixteen-byte allocation rounding for characters and eight-byte rounding for wide characters. A wide-character-only reconstruction using an unconditional eight-byte formula is not a valid generic implementation: the independently compiled character producer has four differing algorithm words under that formula. The recovered generic implementation reproduces both native variants.

## Native producer evidence

The actual #347 formatter producer reproduces both 340-byte `VFormatString` wrappers and the 848-byte character fill replacement, including all actual call targets and their 96 exception bytes. The independent wide-character numeric-input producer preserves all twenty original function extents and nineteen complete algorithm streams, including its 2868-byte `Run`. Its 876-byte wide fill replacement has matching algorithm instructions and four remaining pooled-data offsets in the unresolved whole-unit declaration context. All 144 original direct calls and all 46 cleanup/type relocations resolve to their native targets. This prerequisite does not represent completion of #166 or #347.

The complete original MSL oracle separately reproduces all 876 wide-replacement instruction words, including those four offsets. The recovered source, rather than a change of compiler options, establishes the generic algorithm and declaration layers used here.

The native `std::out_of_range` vtable at `8039A040` identifies its destructor at `80068530`. The compiled 148-byte destructor matches its non-relocation words and all three actual call targets. Naming that existing placeholder preserves its original extent and prevents a duplicate source-selected exception destructor from growing the link. The separate length-error vtable points to `8005A830`, independently distinguishing the otherwise similar bodies.

## Consumer controls

On baseline `8b07def40fc95f17cff823c74e92e1ffa5265857`, all 767 actual source objects were rebuilt before and after this header change. Exactly 708 whole objects are byte-identical. The other 59 preserve every allocated section byte, symbol attribute, and relocation after normalizing compiler-generated numeric labels. The linked ELF preserves allocated bytes and the frozen DOL verifies exactly. No previously exact instruction stream changes.

The first strict parser rejected only CLayout's two generated `__FUNCTION__$number` labels: 5955/5992 become 5987/6024. Their names, numeric suffixes, and six corresponding relocation label names are the only differences; payloads, extents, offsets, and targets are unchanged. The normalized control handles only that established generated-label pattern in addition to ordinary `@number` labels. It does not normalize meaningful names or instructions.

The same complete-consumer, native formatter, and frozen artifact controls run again inside the official merge verifier after each rebase. Raw baseline/candidate objects, ELF, DOL, build graph, and native receipts are retained as the acceptance evidence.

## Traps

A wide-only positive does not establish a generic string allocation formula. Retain `recommend()` and test both character widths; their target rounding differs.

Do not report a generated-symbol parser rejection as either a code regression or a pass without inspection. Compare the allocated payloads and each affected symbol/relocation attribute, and record the exact generated-name exception.
