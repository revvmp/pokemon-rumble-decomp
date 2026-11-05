#define CMODEL_COPY_EMISSION_LAYOUT

#pragma section code_type ".text.model_copy"
#include <nw4r/g3d/res/g3d_resmdl.h>

nw4r::g3d::ResMdl::ResMdl(const ResMdl& rOther) : ResCommon<ResMdlData>(rOther) {}
