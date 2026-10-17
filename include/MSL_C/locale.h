#ifndef MSL_C_LOCALE_H
#define MSL_C_LOCALE_H

#include <wchar_t.h>

#ifdef __cplusplus
extern "C" {
#endif

struct _loc_coll_cmpt;
struct _loc_mon_cmpt;
struct _loc_num_cmpt;
struct _loc_time_cmpt;

struct _loc_ctype_cmpt {
    char CmptName[8];
    const unsigned short* ctype_map_ptr;
    const unsigned char* upper_map_ptr;
    const unsigned char* lower_map_ptr;
    const unsigned short* wctype_map_ptr;
    const wchar_t* wupper_map_ptr;
    const wchar_t* wlower_map_ptr;
};

struct __locale {
    struct __locale* next_locale;
    char locale_name[48];
    struct _loc_coll_cmpt* coll_cmpt_ptr;
    struct _loc_ctype_cmpt* ctype_cmpt_ptr;
    struct _loc_mon_cmpt* mon_cmpt_ptr;
    struct _loc_num_cmpt* num_cmpt_ptr;
    struct _loc_time_cmpt* time_cmpt_ptr;
};

extern struct __locale _current_locale;

#ifdef __cplusplus
}
#endif

#endif
