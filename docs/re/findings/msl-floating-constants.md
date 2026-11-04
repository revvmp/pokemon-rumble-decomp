# MSL floating-point constants

The canonical math interface declares integer storage arrays for floating-point special values. The16bytes at0x804B8FB0 through0x804B8FC0 encode __float_nan (0x7FFFFFFF), __float_huge (0x7F800000) and __double_huge (0x7FF0000000000000). The existing parsers access the final value through an incomplete extern int array, which preserves their evidenced absolute addressing.

The new C definition retains those integer array types and adds the original eight-byte alignment to the double storage. This is IEEE data, not instructions or a fake function. The native build and frozen main.dol verifier both pass after the exact .sdata split is linked from source. No original hashes were changed.
