# BTE device/controller provenance for issue 517

New canonical declarations and constants adapt Petari commit `e0ab141914921726a595573dc48d3f13d0922efe`, files `src/RVL_SDK/bte/btm_api.h`, `btm_int.h`, `hcidefs.h`, and `bt_target.h`, each Copyright (C) 1999-2012 Broadcom Corporation. Includes changes by muff1n1634. Modified material narrows dependencies, moves comments into documentation, preserves existing ABI structures, and documents descriptive names for the three otherwise numeric intermediate reset states. The refreshed headers preserve the complete target-evidenced state enum already merged through F-519-2; the incompatible public vendor-command prototype is omitted to preserve existing caller code generation.

Licensed under the Apache License, Version 2.0 (the License); you may not use this file except in compliance with the License. You may obtain a copy of the License at <http://www.apache.org/licenses/LICENSE-2.0>. Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an AS IS BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.

The complete license is [preserved here](510-apache-2.0.txt). The original BTE NOTICE and upstream references are [preserved here](510-bte-NOTICE.md). The target and validation evidence are documented in F-517-1.

## Source completion additions

`src/RVL_SDK/bte/btm_dev.c` and `src/RVL_SDK/bte/btm_devctl.c` adapt the corresponding files from the same pinned Petari commit. Both carry Copyright (C) 2000-2012 Broadcom Corporation and include changes by muff1n1634, under the Apache-2.0 license and original library NOTICE preserved above. Changes use canonical includes and existing logging/reverse-stream interfaces, move comments to documentation, and use descriptive intermediate reset-state names. Ordinary inline helpers and diagnostic-producing unlinked reference APIs remain; retained and stripped coverage is distinguished in F-517-2.
