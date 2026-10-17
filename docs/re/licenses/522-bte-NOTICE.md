# BTE source provenance for issue 522

The reconstruction in `src/RVL_SDK/bte/btu_init.c`, `wbt_ext.c`, `gap_api.c`, `gap_conn.c`, `gap_utils.c`, `hcicmds.c`, and the declarations they use in `include/revolution/bte/` (`gap_api.h`, `gap_int.h`, `hcimsgs.h`, `l2c_int.h`, `sdp_int.h`, and additions to `bt_target.h`, `bt_trace.h`, `bt_types.h`, `btm_int.h`, `btu.h`, `gki.h`, `hcidefs.h`, `l2c_api.h`, `sdp_api.h`, `sdpdefs.h`), adapts reference code, names, and structure layouts from Petari commit `e0ab141914921726a595573dc48d3f13d0922efe` (`src/RVL_SDK/bte/`). Changes remove source comments and the configurable trace macro system, drop dead-stripped functions, reduce headers to the declarations these units use, and rename include paths. The files are modified from the reference.

The referenced files carry these notices:

- `hcicmds.c` (bluedroid `stack/hcic/hcicmds.c`): Copyright (C) 1999-2012 Broadcom Corporation.
- `btu_init.c` (bluedroid `stack/btu/btu_init.c`): Copyright (C) 2000-2012 Broadcom Corporation.
- `gap_api.c`, `gap_conn.c`, `gap_utils.c` (bluedroid `stack/gap/`): Copyright (C) 2009-2013 Broadcom Corporation.
- Includes changes by muff1n1634.
- `wbt_ext.c` carries no Broadcom notice in Petari, which is CC0.

Licensed under the Apache License, Version 2.0 (the License); you may not use this file except in compliance with the License. You may obtain a copy of the License at <http://www.apache.org/licenses/LICENSE-2.0>. Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an AS IS BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.

The complete license is [preserved here](510-apache-2.0.txt). The original BTE NOTICE is reproduced in [the issue 510 notice](510-bte-NOTICE.md).
