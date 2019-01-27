#ifndef _LOG_H_
#define _LOG_H_

#define LOG(...) printf(__VA_ARGS__)


#define LOG_F(...) LOG(__VA_ARGS__)
#define LOG_E(...) LOG(__VA_ARGS__)
#define LOG_W(...) LOG_E(__VA_ARGS__)
#define LOG_N(...) LOG_E(__VA_ARGS__)
#define LOG_I(...) LOG_E(__VA_ARGS__)
#define LOG_D(...) LOG_E(__VA_ARGS__)
#define LOG_T(...) LOG_E(__VA_ARGS__)

#endif // _LOG_H_
