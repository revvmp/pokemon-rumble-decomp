# BTE HID source provenance for issue 524

The reconstruction in `src/RVL_SDK/bte/hidd_api.c`, `hidd_conn.c`, `hidd_mgmt.c`, `hidd_pm.c`, `hidh_api.c`, and the headers they include under `include/RVL_SDK/bte/`, adapts reference code, names, and structure layouts from Petari commit `e0ab141914921726a595573dc48d3f13d0922efe` (`src/RVL_SDK/bte/`). Changes remove source comments and the configurable trace macro system, drop dead-stripped functions, reduce headers to the declarations these units use, and rename include paths. The files are modified from the reference.

The referenced files carry these notices:

- `hidh_api.c`, `hidh_int.h`, `hid_conn.h`, and other bluedroid-derived files: Copyright (C) 2002-2012 Broadcom Corporation.
- Includes changes by muff1n1634.

Licensed under the Apache License, Version 2.0 (the License); you may not use this file except in compliance with the License. You may obtain a copy of the License at <http://www.apache.org/licenses/LICENSE-2.0>. Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an AS IS BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.

The complete license is [preserved here](510-apache-2.0.txt). The original BTE NOTICE is reproduced in [the issue 510 notice](510-bte-NOTICE.md).
