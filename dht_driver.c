
#include "global.h"

int bits[250], data[100];
int bitidx = 0;

int readDHT(int type, int pin) {

    int counter = 0;
    int laststate = HIGH;
    int j = 0;
    data[0] = data[1] = data[2] = data[3] = data[4] = 0;

    // Set GPIO pin to output
    bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(pin, HIGH);
    usleep(500000);
    bcm2835_gpio_write(pin, LOW);
    usleep(20000);
    // Set GPIO pin to input
    int lowCounter = 0;
    int highCounter = 0;
    int startUpError = OK;
    bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT);
    while (bcm2835_gpio_lev(pin) == 0) {
        lowCounter++;
        if (lowCounter > 5000) {
            startUpError = ERROR;
            break;
        }
    }

    if (startUpError == ERROR) {
        return startUpError;
    }

    while (bcm2835_gpio_lev(pin) == 1) {
        highCounter++;
        if (highCounter > 5000) {
            startUpError = ERROR;
            break;
        }
    }

    if (startUpError == ERROR) {
        return startUpError;
    }

    int i = 0;
    for (i = 0; i < MAXTIMINGS; i++) {
        counter = 0;
        while (bcm2835_gpio_lev(pin) == laststate) {
            counter++;
            //nanosleep(1);		// overclocking might change this?
            if (counter == 1000) {
                break;
            }
        }
        laststate = bcm2835_gpio_lev(pin);
        if (counter == 1000) {
            break;
        }

        if ((i > 3) && (i % 2 == 0)) {
            // shove each bit into the storage bytes
            data[j / 8] <<= 1;
            if (counter > 200) {
                data[j / 8] |= 1;
            }
            j++;
        }
    }

    if (XDEBUG) {
        printf("lowCounter:%d highCounter:%d", lowCounter, highCounter);
        printf(" data[0]:"BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(data[0]));
        printf(" data[1]:"BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(data[1]));
        printf(" data[2]:"BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(data[2]));
        printf(" data[3]:"BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(data[3]));
        printf(" data[4]:"BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(data[4]));
        printf(" data[5]:"BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(((data[0] + data[1] + data[2] + data[3]) & 0xFF)));
        printf(" => Data (%d): 0x%x 0x%x 0x%x 0x%x 0x%x\n", j, data[0], data[1], data[2], data[3], data[4]);
        fflush(stdout);
    }

    if ((j >= 39) && (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF))) {
        if (type == DHT11) {
            printf("%d ˚C;%d %%", data[2], data[0]);
            fflush(stdout);
        } else if (type == DHT22) {
            float f, h;
            h = (float) (data[0] & 0x7F) * 256 + (float) data[1];
            h /= 10;
            f = (float) (data[2] & 0x7F)* 256 + (float) data[3];
            f /= 10.0;
            if (data[2] & 0x80) f *= -1;
            f = getTemp(f);
            printf("%.1f ˚C;%.1f %%", f, h);
            fflush(stdout);
        }
        return OK;
    }

    return ERROR;
}

void printCalibs() {
    int i = 0;
    for (i = 0; i < CALIBITEMSLENGTH; i++) {
        printf("start:%f stop:%f factor:%f\n", calibItems[i].start, calibItems[i].stop, calibItems[i].factor);
        fflush(stdout);
    }
}

int startsWith(const char *a, const char *b) {
    if (strncmp(a, b, strlen(b)) == 0) return 1;
    return 0;
}

void initCalibItems(int argc, char** argv) {

    float u = 0;
    int k = 0;
    for (u = CALIBITEMSSTARTINDEX; u < CALIBITEMSENDTINDEX; u += CALIBITEMSSTEP) {
        if (k < CALIBITEMSLENGTH) {
            calibItems[k].start = u + 0.01;
            calibItems[k].stop = u + CALIBITEMSSTEP;
            calibItems[k].factor = 1.0;
            k++;
        }
    }


    if (argc >= 4) {
        char * line = NULL;
        size_t len = 0;
        ssize_t read;
        fflush(stdout);
        FILE * fp = fopen(argv[3], "r");
        if (fp != NULL) {
            int calibCounter = 0;
            while ((read = getline(&line, &len, fp)) != -1) {
                if (!startsWith(line, "#") && !startsWith(line, " ") && read > 1) {
                    if (calibCounter < CALIBITEMSLENGTH) {
                        calibItems[calibCounter].factor = atof(line);
                    }
                    calibCounter++;
                }
            }
        }
        if (fp) {
            fclose(fp);
        }
        if (line) {
            free(line);
        }
    }

}

float getTemp(float temp) {
    int i = 0;
    float tempneu = temp;
    float start = -1;
    float stop = -1;
    float factor = -1;
    for (i = 0; i < CALIBITEMSLENGTH; i++) {
        if (temp >= calibItems[i].start && temp <= calibItems[i].stop) {
            tempneu = temp * calibItems[i].factor;
            start = calibItems[i].start;
            stop = calibItems[i].stop;
            factor = calibItems[i].factor;
            break;
        }
    }
    //printf("Temp(alt):%f Temp(neu):%f factor:%f start:%f stop:%f\n", temp, tempneu, factor, start, stop);
    //fflush(stdout);
    return tempneu;
}
