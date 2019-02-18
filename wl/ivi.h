#ifndef _HUDTDS_IVI_H_
#define _HUDTDS_IVI_H_

#include <stdint.h>

struct wl_registry;

void hudivi_registry(struct wl_registry *reg, uint32_t id, const char *iface, uint32_t ver);


#endif // _HUDTDS_IVI_H_
