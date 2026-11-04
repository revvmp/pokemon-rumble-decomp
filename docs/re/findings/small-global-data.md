# Small typed global data owners

Six source owners provide 28 bytes of evidenced global state and parameters. They use existing public types and declarations rather than arbitrary byte arrays. No DSP instruction image is reconstructed or copied.

| Source owner | Symbol | Address | Actual size | Evidence |
| --- | --- | --- | --- | --- |
| game/LayoutResources.cpp | gSharedLayoutArchives | 0x804B9098 | 4 | LayoutResources.h pointer declaration; l10n_files assigns locale archive arrays and UI layouts consume it |
| Runtime/new_handler.cpp | __new_handler | 0x804B9BC0 | 4 | game/new.cpp invokes this std::new_handler on allocation failure |
| MSL_C/errno.c | errno | 0x804B9BD0 | 4 | Existing errno.h declares the C int used by MSL callers |
| RVL_SDK/exi/EXIConfig.c | __EXIFreq | 0x804BB288 | 4 | euart.c declares u32 and passes it as EXI frequency; target value is 4 |
| RVL_SDK/ax/AXDSPConfig.c | axDspInitVector, axDspResumeVector, axDspSlaveLength | 0x804B8BD8?0x804B8BDE | 6 | ax.h declares u16 values; AXOut initializes DSP task metadata; values are 0x10, 0x37, 0x2000 |
| RVL_SDK/bte/bd.c | bd_addr_null | 0x804BBC20 | 6 | Existing bd.h declares const BD_ADDR and bta_hh_utils compares six-byte addresses against it |

The new files isolate definitions from existing function compilation contexts. In particular, defining the frequency in euart.c could allow constant folding instead of its native data load. The Bluetooth null address joins its existing byte-copy/comparison owner; isolated objdiff retains both bdcpy and bdcmp at 100 percent.

All units compile with their corresponding configured compiler/flag families. Their data sections exactly match the native bytes (or zero storage), with real objects of the sizes above. Small-data split ends round the AX six bytes to eight and the Bluetooth six bytes to eight; these extra bytes are alignment, not invented structure fields. Existing inferred eight-byte pointer/int symbols must become four-byte actual objects. Integration specifications and compiled evidence are in ignored tools/local/session/small-globals; full native acceptance is pending maintainer validation.

## Native acceptance

The maintainer integrated all six owners and rebuilt through the native pipeline. Ninja exited 0 and the frozen-manifest verifier passed. At the same accepted checkpoint, ScriptWorld's already-generated CWaza shared-deleter metadata gained source ownership: .data 0x803B7348?0x803B73E0 and .sdata 0x804B7AB8?0x804B7AC8. No generated-name renames or source metadata shims were necessary. The preceding ScriptPiiPropData .data split had to extend from 0x803B7344 to 0x803B7348 because dtk's gap symbol at 0x803B7343 extends through this alignment boundary; its prior out-of-bounds extent caused report generation to fail.

The resulting accepted checkpoint has 873 Matching units out of 1385, and 1,656,651 of 1,788,773 source-linked data bytes (92.61382 percent). Original hashes remain unchanged.
