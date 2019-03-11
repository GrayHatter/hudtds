#include "bluetooth.h"

#include "hud.h"
#include "log.h"


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <pthread.h>
#include <time.h>


static void *bluetooth_thread(void *p)
{
    (void) p;

    int max_rsp = 255;
    inquiry_info *ii = malloc(max_rsp * sizeof(inquiry_info));
    if (!ii) {
        LOG_F("Unable to malloc for inquiry_info\n");
        exit(2);
    }
    int dev_id = hci_devid("C4:BE:84:6F:F8:EE");
    int sock = hci_open_dev(dev_id);
    if (dev_id < 0 || sock < 0) {
        perror("opening socket");
        exit(1);
    }

    while (1) {
        // struct timespec __ts_nanosleep = { .tv_nsec = _TENTH_SEC};
        // nanosleep(&__ts_nanosleep, NULL);
        sleep(3);

        LOG_E("Starting BT scan\n");
        int num_rsp = hci_inquiry(dev_id, 6, max_rsp, NULL, &ii, IREQ_CACHE_FLUSH);
        LOG_E("\nBT scan done, got %i\n\n", num_rsp);

        if ( num_rsp < 0 ) {
            perror("hci_inquiry");
        }

        char addr[19] = { 0 };
        char name[248] = { 0 };
        for (int i = 0; i < num_rsp; i++) {
            ba2str(&(ii+i)->bdaddr, addr);
            memset(name, 0, sizeof(name));
            if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name), name, 0) < 0) {
                strcpy(name, "[unknown]");
            }

            LOG_E("%s  %s\n", addr, name);
        }

        free(ii);
        close(sock);
    }

    return NULL;
}


void bluetooth_thread_start(void)
{
    LOG_E("gps thread starting up\n");

    pthread_t t;
    pthread_create(&t, NULL, bluetooth_thread, NULL);
}
