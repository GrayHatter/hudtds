#ifndef _LOG_H_
#define _LOG_H_

#include <stdio.h>

typedef enum {
    LOGLVL_NONE = 0,
    LOGLVL_FATAL,
    LOGLVL_ERROR,
    LOGLVL_WARN,
    LOGLVL_NOTE,
    LOGLVL_INFO,
    LOGLVL_DEBUG,
    LOGLVL_TRACE,
    LOGLVL_WTF
} LOGLVL;

extern LOGLVL curr_log_lvl;

#define LOG(...) printf(__VA_ARGS__)

#define LVL(x) (curr_log_lvl >= LOGLVL_##x)

#define LOG_F(...) LVL(FATAL) ? LOG(__VA_ARGS__) : (void)(0)
#define LOG_E(...) LVL(ERROR) ? LOG(__VA_ARGS__) : (void)(0)
#define LOG_W(...) LVL(WARN)  ? LOG(__VA_ARGS__) : (void)(0)
#define LOG_N(...) LVL(NOTE)  ? LOG(__VA_ARGS__) : (void)(0)
#define LOG_I(...) LVL(INFO)  ? LOG(__VA_ARGS__) : (void)(0)
#define LOG_D(...) LVL(DEBUG) ? LOG(__VA_ARGS__) : (void)(0)
#define LOG_T(...) LVL(TRACE) ? LOG(__VA_ARGS__) : (void)(0)

#endif // _LOG_H_
