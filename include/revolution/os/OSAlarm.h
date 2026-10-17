#ifndef RVL_SDK_OS_ALARM_H
#define RVL_SDK_OS_ALARM_H

typedef signed long long OSTime;
typedef struct OSContext OSContext;
typedef struct OSAlarm OSAlarm;
typedef void (*OSAlarmHandler)(OSAlarm*, OSContext*);

struct OSAlarm {
    OSAlarmHandler handler;
    unsigned long tag;
    OSTime fire;
    OSAlarm* prev;
    OSAlarm* next;
    OSTime period;
    OSTime start;
    void* userData;
};

#ifdef __cplusplus
extern "C" {
#endif

void __OSInitAlarm(void);
void OSCreateAlarm(OSAlarm*);
void OSSetAlarm(OSAlarm*, OSTime, OSAlarmHandler);
void OSSetPeriodicAlarm(OSAlarm*, OSTime, OSTime, OSAlarmHandler);
void OSCancelAlarm(OSAlarm*);
void OSSetAlarmUserData(OSAlarm*, void*);
void* OSGetAlarmUserData(const OSAlarm*);
void __OSCancelAlarmsForUserData(void*);


#ifdef __cplusplus
}
#endif

#endif
