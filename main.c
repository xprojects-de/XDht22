
#include "global.h"

int main(int argc, char** argv) {

    if (!bcm2835_init()) {
        return ERROR;
    }

    if (argc < 3) {
        printf("usage: %s [11|22|2302] GPIOpin#\n", argv[0]);
        printf("example: %s 2302 4 - Read from an AM2302 connected to GPIO #4\n", argv[0]);
        return ERROR;
    }
    int type = 0;
    if (strcmp(argv[1], "11") == 0) {
        type = DHT11;
    } else if (strcmp(argv[1], "22") == 0) {
        type = DHT22;
    } else if (strcmp(argv[1], "2302") == 0) {
        type = AM2302;
    }
    if (type == 0) {
        printf("Select 11, 22, 2302 as type!\n");
        return ERROR;
    }

    int dhtpin = atoi(argv[2]);

    if (dhtpin <= 0) {
        printf("Please select a valid GPIO pin #\n");
        return ERROR;
    }

    initCalibItems(argc, argv);

    int loop = TRUE;
    int result = 0;
    int tries = 10;
    while (loop) {
        result = readDHT(type, dhtpin);
        if (result == OK) {
            loop = FALSE;
        } else {
            if (tries-- > 0) {
                delay(1200);
            } else {
                loop = FALSE;
            }
        }
    }

    return (EXIT_SUCCESS);
}

