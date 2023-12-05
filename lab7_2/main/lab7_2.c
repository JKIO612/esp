#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_timer.h"

#include "driver/adc.h"
#include "esp_system.h"

#define BUTTON_GPIO 0


#define ADC_CHANNEL ADC_CHANNEL_0
#define THRESHOLD 60

void decode_morse_code(const char* morse_code) {
    // Morse code to English translation table
    const char* morse_code_table[] = {
        ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", "-.-", ".-..", "--",
        "-.", "---", ".--.", "--.-", ".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--..",
        "-----", ".----", "..---", "...--", "....-", ".....", "-....", "--...", "---..", "----."
    };

    const char* alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char* numbers = "0123456789";
    int length = strlen(morse_code);

    char message[length + 1];
    message[0] = '\0';

    for (int i = 0; i < length; i++) {
        if (morse_code[i] == '.') {
            strcat(message, ".");
        } else if (morse_code[i] == '-') {
            strcat(message, "-");
        } else if (morse_code[i] == ' ') {
            int index = -1;
            for (int j = 0; j < 36; j++) {
                if (strcmp(message, morse_code_table[j]) == 0) {
                    index = j;
                    break;
                }
            }
            if (index >= 0) {
                if (index < 26) {
                    printf("%c", alphabet[index]);
                } else {
                    printf("%c", numbers[index - 26]);
                }
            }
            message[0] = '\0';
        }
    }

    printf("\n");
}


void app_main(void) {
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN_DB_11);
    int high = 0;
    int low = 0;
    char morse_code[7] = "";

    while (1) {
        int val = adc1_get_raw(ADC_CHANNEL);
        if (val > THRESHOLD) {
            high += 1;
            low = 0;
        } else {
            if (high >= 5) {
                strcat(morse_code, "-");
                }
            else if (high >= 2) {
                strcat(morse_code, ".");
            }
            low += 1;
            high = 0;
            }
            if (low >= 5) {
            strcat(morse_code, " ");
            printf("Morse code is: %s", morse_code);
            decode_morse_code(morse_code);
                morse_code[0] = '\0';
            low = 0;
            }
            vTaskDelay(pdMS_TO_TICKS(50));
        }
}


