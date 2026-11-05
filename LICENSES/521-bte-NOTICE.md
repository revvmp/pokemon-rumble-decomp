# BTE source provenance for issue 521

The reconstructions in `src/RVL_SDK/bte/btu_hcif.c` and `src/RVL_SDK/bte/btm_sec.c`, and the declarations they add to `include/revolution/bte/` (`bt_trace.h`, `bt_types.h`, `btm_api.h`, `btm_int.h`, `btu.h`, `gki.h`, `hcidefs.h`, `hcimsgs.h`, `l2c_int.h`), adapts reference code, names, and structure layouts from Petari commit `e0ab141914921726a595573dc48d3f13d0922efe` (`src/RVL_SDK/bte/`). Changes remove source comments and the configurable trace macro system, drop the dead-stripped `btu_hcif_send_host_rdy_for_data`, replace the variadic trace macros with fixed-arity ones, reduce headers to the declarations this unit uses, and rename include paths. The files are modified from the reference.

The referenced files carry these notices:

- `btu_hcif.c` (bluedroid `stack/btu/btu_hcif.c`): Copyright (C) 1999-2012 Broadcom Corporation.
- `btm_sec.c` (bluedroid `stack/btm/btm_sec.c`): Copyright (C) 1999-2012 Broadcom Corporation.
- Includes changes by muff1n1634.

Licensed under the Apache License, Version 2.0 (the License); you may not use this file except in compliance with the License. You may obtain a copy of the License at <http://www.apache.org/licenses/LICENSE-2.0>. Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an AS IS BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.

The complete license is [preserved here](510-apache-2.0.txt). The original BTE NOTICE is reproduced in [the issue 510 notice](510-bte-NOTICE.md).
