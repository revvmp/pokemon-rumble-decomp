# BTE HID utilities and ACL provenance for issue 516

Modified sources and added declarations derive from Petari commit `e0ab141914921726a595573dc48d3f13d0922efe`, under `src/RVL_SDK/bte/`:

- `bta_hh_utils.c`: Copyright (C) 2005-2012 Broadcom Corporation.
- `btm_acl.c`: Copyright (C) 2000-2012 Broadcom Corporation.
- Added declarations/constants from `btm_api.h`, `btm_int.h`, `hcidefs.h`, and `bt_trace.h`: Copyright (C) 1999-2012 Broadcom Corporation.
- The const null address declaration follows `bd.h` from that same revision.
- Includes changes by muff1n1634.

Licensed under the Apache License, Version 2.0 (the License); you may not use this file except in compliance with the License. You may obtain a copy of the License at <http://www.apache.org/licenses/LICENSE-2.0>. Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an AS IS BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.

The complete license is [preserved here](510-apache-2.0.txt). The original library NOTICE and upstream references are [preserved here](510-bte-NOTICE.md). Modified files use canonical includes and project logging calls, move comments into documentation, and preserve complete ordinary C helpers whose inlining or diagnostic pools are required by the target. Findings F-516-1 distinguish retained code, retained diagnostic bytes, and linker-stripped code/data.
