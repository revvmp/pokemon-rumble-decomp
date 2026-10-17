# BTE HID-host provenance for issue 515

The following modified reference files come from Petari commit `e0ab141914921726a595573dc48d3f13d0922efe`, under its `src/RVL_SDK/bte/` directory:

- `bta_hh_act.c`, `bta_hh_api.c`, `bta_hh_main.c`, `bta_hh_int.h`, and `bta_hh_co.h`: Copyright (C) 2005-2012 Broadcom Corporation.
- `bta_hh_api.h` and the added `hiddefs.h` handshake enum: Copyright (C) 2002-2012 Broadcom Corporation.
- Includes changes by muff1n1634.

Licensed under the Apache License, Version 2.0 (the License); you may not use this file except in compliance with the License. You may obtain a copy of the License at <http://www.apache.org/licenses/LICENSE-2.0>. Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an AS IS BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.

The complete license is [preserved here](510-apache-2.0.txt). The original library NOTICE, including upstream source references and muff1nOS attribution, is [preserved here](510-bte-NOTICE.md#bte-source-provenance-for-issue-510).

Modifications move comments into documentation, use canonical includes and existing trace interfaces, add explicit trace argument conversions, remove disabled reference alternatives, and omit the target-absent boot-report parser. Exact reference constants and complete C structures replace no existing shared ABI declarations. Findings F-515-1 identify retained and linker-stripped functions and establish target extents.
