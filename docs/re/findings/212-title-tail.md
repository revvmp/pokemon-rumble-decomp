# F-212-1: Title-session data-order tail probes

Historical baseline before the coordinator/UI-agent fix: 16/17 real functions exact. Run differs in nine non-relocated immediate offsets into the combined data pool: its literals are earlier by 0x138 because the CUIManager and CTitleSession shared-pointer deleter objects are emitted after the pool. Prior findings are in sprint-a12 and sprint-a17.

Isolated exact-flag compiles tested TU-local shared-pointer constructor specializations; explicit concrete shared_ptr_deleter class specializations with identical fields and virtual order; out-of-class strong constructors; unused inline construction and typeid emission triggers; static type-info references and deleter objects; defer_codegen and inline_bottom_up toggles. All retained the same Run offset mismatches. Compiling without file IPA regressed four additional functions without fixing Run. None of these particular probes was retained. The coordinator and UI agent subsequently resolved the data order: CTitleSession is now 17/17 exact and source-linked with full frozen SHA verification. Evidence scratch: tools/local/session/212-title.

## Traps

Literal pool offsets in these instructions are immediate operands, so assigning better symbol names cannot correct them. Their actual source data placement must be fixed.
