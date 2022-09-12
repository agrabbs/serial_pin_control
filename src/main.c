#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define RETURN '\r'
#define BUF_SIZE 10

//#define WATCH_1 24

const uint WATCH[] = {26, 27, 28, 29, 24, 25};
int usb_connected = 0;

int main() {
    // Init Serial
    stdio_init_all();

    // Init Pins
    int i;
    for (i = 0; i < 6; i++) {
        gpio_init(WATCH[i]);
        gpio_disable_pulls(WATCH[i]);
        gpio_set_dir(WATCH[i], GPIO_IN);
    }

    while(true) {
        char ch;
        char buffer[BUF_SIZE];
        int buffer_index = 0;
        while(!stdio_usb_connected()) {
            sleep_ms(1000);
        }
        if(stdio_usb_connected() && !usb_connected) {
            usb_connected = 1;
            printf("[READY]\r\n");
        } 
        else if (!stdio_usb_connected() && usb_connected) {
            usb_connected = 0;
        }
        while (ch != PICO_ERROR_TIMEOUT) {
            ch = getchar_timeout_us(100);
            switch (ch) {
                case 0x08: // backspace
                case 0x7F: // delete
                    break;
                case RETURN: // linefeed
                    break;
                case 32 ... 126: // only valid ascii characters
                    printf("%c", (ch & 0xFF));
                    buffer[buffer_index++] = (ch & 0xFF);
                    break;
                default:
                    break;
            }
            if (ch == RETURN) {
                buffer[buffer_index] = '\0';
                int watch;
                if (strcmp(buffer, "")) {
                    printf("\r\n");
                    if (strncmp(buffer, "recover", 7) == 0) {
                        watch = buffer[8] - '0' - 1;
                        printf("[recovering watch] %i\r\n", WATCH[watch]);
                        gpio_set_dir(WATCH[watch], GPIO_OUT);
                        gpio_put(WATCH[watch], 0);
                        sleep_ms(10000);
                        gpio_set_dir(WATCH[watch], GPIO_IN);
                        printf("[recover watch] complete.\r\n");
                    }
                    else {
                        printf("No cli handler for that command.\r\n");
                    }
                }
                else {
                    printf("No command.\r\n");
                }
                break;
            }
        }
    }
    return 0;
}