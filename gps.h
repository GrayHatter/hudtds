#ifndef _HUDTDS_GPS_H_
#define _HUDTDS_GPS_H_

#define _POSIX_C_SOURCE 200809L

#define GPSCOUNT 72  // this magic number courtesy gps.h

#include <stdbool.h>
#include <stdint.h>

struct hud_sat_data {
    bool visable;
    bool used;
    int PRN;
    int elevation;
    int azimuth;
    double snr;
};


struct hud_gps_data {
    double lat;
    double lon;
    double alt;

    double gnd_speed;
    double vert_speed;

    uint8_t satellites_visible;
    uint8_t satellites_used;
    struct hud_sat_data satellites[GPSCOUNT];
};


void gps_thread_start(void);

void gps_thread_stop(void);

struct hud_gps_data *gps_get_data(void);

#endif // _HUDTDS_GPS_H_
