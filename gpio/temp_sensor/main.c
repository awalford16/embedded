#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#define SEGMENT_COUNT 8
#define DIGIT_COUNT 4

uint LED_SEGMENTS[SEGMENT_COUNT] = {0, 1, 2, 3, 4, 5, 6, 7};
uint LED_DIGITS[DIGIT_COUNT] = {8, 9, 10, 11};

uint DIGIT_SEGMENTS[DIGIT_COUNT][SEGMENT_COUNT] = {
    {1, 1, 1, 1, 1, 1}, // Digit 0
    {0, 1, 1},          // Digit 1
    {1, 1, 0, 1, 1, 0, 1},
    {1, 1, 1, 1, 0, 0, 1},
};

void show_digit(uint digit)
{
    // Enable segments for chosen digit
    for (int s = 0; s < SEGMENT_COUNT; s++)
    {
        gpio_put(LED_SEGMENTS[s], DIGIT_SEGMENTS[digit][s]);
    }

    // Enable chosen digit
    gpio_put(LED_DIGITS[digit], 0);
}

void clear_digits()
{
    // Disable all digits
    for (int d = 0; d < DIGIT_COUNT; d++)
    {
        gpio_put(LED_DIGITS[d], 1);
    }
}

int main(void)
{
    stdio_init_all();

    // Setup ADC converter
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4); // Select 5th input channel of sensor

    // Initialise GPIO pins
    for (int s = 0; s < SEGMENT_COUNT; s++)
    {
        gpio_init(LED_SEGMENTS[s]);
        gpio_set_dir(LED_SEGMENTS[s], GPIO_OUT);
    }

    for (int d = 0; d < DIGIT_COUNT; d++)
    {
        gpio_init(LED_DIGITS[d]);
        gpio_set_dir(LED_DIGITS[d], GPIO_OUT);
    }

    while (1)
    {
        uint16_t raw = adc_read();
        const float conversion = 3.3f / (1 << 12);
        float voltage = raw * conversion;

        // Equeation given in datasheet
        float temp = 27 - (voltage - 0.706) / 0.001712;

        printf("Temperature: %f C\n", temp);

        for (int i = 0; i < 4; i++)
        {
            show_digit(i);
            sleep_ms(1000);
        }

        sleep_ms(1000);
        clear_digits();
    }
}