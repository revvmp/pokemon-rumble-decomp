# CUIMenu final tail

OnInput originally compared at99.893616%. Prior ordinary candidate, reference and inline helper attempts are recorded in sprint-t1 and sprint-a17. The remaining four operands reuse the GetSelectedIndex return register instead of its saved copy when calculating the selected row.

Using the existing candidate variable in that remainder, together with scoped opt_propagation off, reproduces all940 function bytes exactly. Candidate equals selected at this branch. Neither the variable substitution nor optimizer switch alone resolves the mismatch. This is a D-0016 fake match; no instructions or object bytes are patched.

All61 real functions compare at100% in the native build. The unit remains NonMatching because its strong/helper emission order differs; local ordering investigation continues. Ninja exited0 and frozen main.dol verification passed with this source comparison and the explicit original-unit fallback. Evidence: tools/local/session/menu-root/flagbest4.json and current build report.

## Rejected probes

Same-type selected reassignments, enum index views, reference/integer remainder helpers and local casts did not resolve the mismatch. The successful combination emerged from testing the existing candidate expression with the propagation switch after independent sweeps failed.
