#ifndef _HUDTDS_GPS_H_
#define _HUDTDS_GPS_H_

struct hud_gps_data {
    double lat;
    double lon;
    double alt;

    double gnd_speed;
    double vert_speed;

    int num_sats_visible;
    int num_sats_connected;
};


void gps_thread_start(void);

void gps_thread_stop(void);

struct hud_gps_data *gps_get_data(void);

#endif // _HUDTDS_GPS_H_
