# BTE source provenance for issue 519

The reconstructions in `src/RVL_SDK/bte/btm_inq.c`, `src/RVL_SDK/bte/btm_main.c` and `src/RVL_SDK/bte/btm_pm.c`, and the declarations they add to `include/revolution/bte/` (`bt_target.h`, `bt_trace.h`, `btm_api.h`, `btm_int.h`, `gki.h`, `hcidefs.h`), adapt reference code, names, and structure layouts from Petari commit `e0ab141914921726a595573dc48d3f13d0922efe` (`src/RVL_SDK/bte/`). Changes remove source comments, replace the variadic trace macros with fixed-arity ones, drop `BTM_SetPeriodicInquiryMode` and `BTM_CancelPeriodicInquiry`, which this SDK does not contain, reduce headers to the declarations these units use, and rename include paths. The files are modified from the reference.

The referenced files carry these notices:

- `btm_inq.c` (bluedroid `stack/btm/btm_inq.c`): Copyright (C) 1999-2012 Broadcom Corporation.
- `btm_main.c` (bluedroid `stack/btm/btm_main.c`): Copyright (C) 2002-2012 Broadcom Corporation.
- `btm_pm.c` (bluedroid `stack/btm/btm_pm.c`): Copyright (C) 2000-2012 Broadcom Corporation.
- Includes changes by muff1n1634.

Licensed under the Apache License, Version 2.0 (the License); you may not use this file except in compliance with the License. You may obtain a copy of the License at <http://www.apache.org/licenses/LICENSE-2.0>. Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an AS IS BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.

The complete license is [preserved here](510-apache-2.0.txt). The original BTE NOTICE is reproduced in [the issue 510 notice](510-bte-NOTICE.md).
