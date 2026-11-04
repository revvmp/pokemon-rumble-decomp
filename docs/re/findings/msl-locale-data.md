# MSL C locale definition

The locale descriptors at0x803E6078 through0x803E6270 are reconstructed using the pinned CC0 Petari locale implementation and canonical structures. The reference values match this target: ASCII classification/case maps, no-conversion multibyte callbacks, C collation weights, empty monetary fields, decimal point, English date/time formats and the current-locale pointer graph. Original wide-map names __wctype_mapC, __wlower_mapC and __wupper_mapC replace the earlier descriptive names after comparison with this reference.

The recovered source owns504bytes of .data,262bytes of date/day/month strings at0x80398888 through0x8039898E, and19bytes of short strings at0x804BB680 through0x804BB693. Following alignment stays outside these source ranges. All pointers are ordinary compiler relocations. The canonical locale header now completes its previously forward-declared component types with the reference fields; no ABI layout used by existing code changed.

The native configure/ninja pipeline exits0 and frozen main.dol verification passes. No function, executable instruction blob, binary patch or unit test was added. Source provenance and license are recorded in [the notice](../licenses/msl-locale-NOTICE.md).
