#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_PORT i2c0

// Address of accelerometer
static int addr = 0x28;

// Initialise accelerometer
void accel_init(void)
{
    sleep_ms(1000);

    // most devices have a chipID registered in address 00
    uint8_t reg = 0x00;
    uint8_t chipID[1];

    // device address, register to store data, size of data, controller controls i2c bus
    // true will keep control of the i2c bus which is then released after the read function
    // To read data we need to first write the register that we want to read from
    i2c_write_blocking(I2C_PORT, addr, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, addr, chipID, 1, false);

    // Check to see if device is connected properly
    // The chipID can be found in the datasheet
    if (chipID[0] != 0xA0)
    {
        while (1)
        {
            printf("Chip ID Not correct - Check connection");
            sleep_ms(5000);
        }
    }

    // Use internal oscillator
    uint8_t data[2];
    data[0] = 0x3F;
    data[1] = 0x40;

    i2c_write_blocking(I2C_PORT, addr, data, 2, false);
}

int main(void)
{
    stdio_init_all;

    // Initialise I2C
    // Takes the speed of communication in Hertz
    // I2C on pico has max speed of 1MHz but other devices can support up to 5MHz
    i2c_init(I2C_PORT, 400000);

    // Configure GPIO pins of SCL and SDA lines
    gpio_set_function(4, GPIO_FUNC_I2C);
    gpio_set_function(5, GPIO_FUNC_I2C);

    // Enable pull-up resistors on the ports
    // GPIO pins not the board pin numbers
    gpio_pull_up(4);
    gpio_pull_up(5);

    accel_init();

    // Read accelleration registers
    uint8_t accel[6];
    int16_t accelX, accelY, accelZ;
    float f_accelX, f_accelY, f_accelZ;
    uint8_t val = 0x08;

    while (1)
    {
        // Read the acceleration registers
        i2c_write_blocking(I2C_PORT, addr, &val, 1, true);
        i2c_read_blocking(I2C_PORT, addr, accel, 6, false);

        // Combine 2 registers into 1 acceleration value
        accelX = ((accel[1] << 8) | accel[0]);
        accelY = ((accel[3] << 8) | accel[2]);
        accelZ = ((accel[5] << 8) | accel[4]);

        // Correct order of magnitude
        f_accelX = accelX / 100.00;
        f_accelY = accelY / 100.00;
        f_accelZ = accelZ / 100.00;

        printf("X: %6.2f   Y: %6.2f   Z: %6.2f\n", f_accelX, f_accelY, f_accelZ);
        sleep_ms(300);
    }
}