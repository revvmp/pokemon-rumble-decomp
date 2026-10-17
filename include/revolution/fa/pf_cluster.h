#ifndef FA_PF_CLUSTER_H
#define FA_PF_CLUSTER_H

#include <revolution/fa/pf_volume.h>

#ifdef __cplusplus
extern "C" {
#endif

void PFCLUSTER_UpdateLastAccessCluster(PF_FILE* p_file, u32 sector);
s32 PFCLUSTER_AppendCluster(PF_FILE* p_file, u32 byte, u32* p_success);
s32 PFCLUSTER_FreeUnusedCluster(PF_FILE* p_file);
s32 PFCLUSTER_GetAppendSize(PF_FILE* p_file, u32* p_size);
s32 PFCLUSTER_CombineChain(PF_ENT_ITER* p_iter_dst, PF_ENT_ITER* p_iter_src, PF_DIR_ENT* p_ent_dst, PF_DIR_ENT* p_ent_src);
s32 PFCLUSTER_DivideChain(PF_ENT_ITER* p_iter, PF_ENT_ITER* p_iter_new, PF_DIR_ENT* p_ent, PF_DIR_ENT* p_ent_new, u32 size);
s32 PFCLUSTER_InsertChain(PF_ENT_ITER* p_iter, PF_DIR_ENT* p_ent, u32 chain_index, u32 num_cluster, u32* p_num_inserted);
s32 PFCLUSTER_DeleteChain(PF_ENT_ITER* p_iter, PF_DIR_ENT* p_ent, u32 chain_index, u32 num_cluster, u32* p_num_deleted);

#ifdef __cplusplus
}
#endif

#endif
