#include <revolution/fa/msc_drv.h>


static PFD_MSC_MEDIA_CALLBACK g_detach_func;
static PFD_MSC_MEDIA_CALLBACK g_attach_func;

s32 uhf_msc_is_media_insert(PDM_DISK* p_disk);

s32 pfd_mscdrv_registar_callback(PFD_MSC_MEDIA_CALLBACK attach, PFD_MSC_MEDIA_CALLBACK detach) {
    g_attach_func = attach;
    g_detach_func = detach;
    return 0;
}

s32 pfd_mscdrv_is_media_insert(PDM_DISK* p_disk) {
    return uhf_msc_is_media_insert(p_disk);
}
