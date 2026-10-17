# BTE source provenance for issue 520

The reconstruction in `src/RVL_SDK/bte/btm_sco.c`, and the declarations it adds to `include/revolution/bte/` (`bt_target.h`, `bt_trace.h`, `btm_api.h`, `btm_int.h`, `hcidefs.h`), adapts reference code, names, and structure layouts from Petari commit `e0ab141914921726a595573dc48d3f13d0922efe` (`src/RVL_SDK/bte/`). Changes remove source comments, replace the variadic trace macros with fixed-arity ones, reduce headers to the declarations this unit uses, and rename include paths. The files are modified from the reference.

The referenced files carry these notices:

- `btm_sco.c` (bluedroid `stack/btm/btm_sco.c`): Copyright (C) 1999-2012 Broadcom Corporation.
- Includes changes by muff1n1634.

Licensed under the Apache License, Version 2.0 (the License); you may not use this file except in compliance with the License. You may obtain a copy of the License at <http://www.apache.org/licenses/LICENSE-2.0>. Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an AS IS BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.

The complete license is [preserved here](510-apache-2.0.txt). The original BTE NOTICE is reproduced in [the issue 510 notice](510-bte-NOTICE.md).
