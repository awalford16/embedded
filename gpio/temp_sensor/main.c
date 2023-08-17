#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#define SEGMENT_COUNT 8
#define DIGIT_COUNT 4

uint LED_SEGMENTS[SEGMENT_COUNT] = {0, 1, 2, 3, 4, 5, 6, 7};
uint LED_DIGITS[DIGIT_COUNT] = {8, 9, 10, 11};

// Display upsidedown so DP appears as degrees
uint DISPLAY_VALUES[12][SEGMENT_COUNT] = {
    {1, 1, 1, 1, 1, 1},    // Digit 0
    {0, 0, 0, 0, 1, 1, 0}, // Digit 1
    {1, 1, 0, 1, 1, 0, 1},
    {1, 0, 0, 1, 1, 1, 1},
    {0, 0, 1, 0, 1, 1, 1},    // 4
    {1, 0, 1, 1, 0, 1, 1},    // 5
    {1, 1, 1, 1, 0, 1, 1, 0}, // 6
    {0, 0, 0, 1, 1, 1},       // 7
    {1, 1, 1, 1, 1, 1, 1, 0}, // 8
    {1, 0, 1, 1, 1, 1, 1},    // 9
    {1, 1, 1, 1, 0, 0, 0, 1}, // .C
    {}};

void show_digit(uint digit)
{
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

void display_value(uint8_t digit, uint8_t value)
{
    show_digit(digit);

    // Enable segments for chosen digit
    for (int s = 0; s < SEGMENT_COUNT; s++)
    {
        gpio_put(LED_SEGMENTS[s], DISPLAY_VALUES[value][s]);
    }

    sleep_ms(5);
    clear_digits();
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

    int check_temp = 100;
    int temp = 0;
    while (1)
    {
        // Update temperature every 100
        if (check_temp == 100)
        {
            uint16_t raw = adc_read();
            const float conversion = 3.3f / (1 << 12);
            float voltage = raw * conversion;

            // Equeation given in datasheet
            temp = 27 - (voltage - 0.706) / 0.001712;
            printf("Temperature: %d C\n", temp);

            check_temp = 0;
        }

        int num1 = temp / 10;
        int num2 = temp % 10;

        display_value(0, 10); // .C
        display_value(1, num2);
        display_value(2, num1);
        display_value(3, 12); // Blank

        check_temp++;
    }

    return 0;
}