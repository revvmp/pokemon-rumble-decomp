typedef struct {
    unsigned char gpr;
    unsigned char fpr;
    unsigned char reserved[2];
    char* input_arg_area;
    char* reg_save_area;
} BTEVaList[1];

extern int vsprintf(char* p_buffer, const char* p_format, BTEVaList args);
extern int sprintf(char* p_buffer, const char* p_format, ...);
extern void OSReport(const char* p_format, ...);
extern volatile unsigned char bte_target_mode;

void LogMsg(unsigned long trace_set_mask, const char* p_format, ...) {
    static char bte_log_buffer[2000];
    BTEVaList args;
    __builtin_va_info(&args);
    vsprintf(bte_log_buffer, p_format, args);
    sprintf(bte_log_buffer, "%s\n", bte_log_buffer);
    OSReport(bte_log_buffer);
}

void LogMsg_0(unsigned long trace_set_mask, const char* p_format) {
    if (bte_target_mode == 0)
        LogMsg(trace_set_mask, p_format);
}

void LogMsg_1(unsigned long trace_set_mask, const char* p_format, unsigned long p1) {
    if (bte_target_mode == 0)
        LogMsg(trace_set_mask, p_format, p1);
}

void LogMsg_2(unsigned long trace_set_mask, const char* p_format, unsigned long p1, unsigned long p2) {
    if (bte_target_mode == 0)
        LogMsg(trace_set_mask, p_format, p1, p2);
}

void LogMsg_3(unsigned long trace_set_mask, const char* p_format, unsigned long p1, unsigned long p2, unsigned long p3) {
    if (bte_target_mode == 0)
        LogMsg(trace_set_mask, p_format, p1, p2, p3);
}

void LogMsg_4(unsigned long trace_set_mask, const char* p_format, unsigned long p1, unsigned long p2, unsigned long p3, unsigned long p4) {
    if (bte_target_mode == 0)
        LogMsg(trace_set_mask, p_format, p1, p2, p3, p4);
}

void LogMsg_5(unsigned long trace_set_mask, const char* p_format, unsigned long p1, unsigned long p2, unsigned long p3, unsigned long p4, unsigned long p5) {
    if (bte_target_mode == 0)
        LogMsg(trace_set_mask, p_format, p1, p2, p3, p4, p5);
}

void LogMsg_6(unsigned long trace_set_mask, const char* p_format, unsigned long p1, unsigned long p2, unsigned long p3, unsigned long p4, unsigned long p5, unsigned long p6) {
    if (bte_target_mode == 0)
        LogMsg(trace_set_mask, p_format, p1, p2, p3, p4, p5, p6);
}

