#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include "pico/stdlib.h"

void led_task()
{
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while (1)
    {
        gpio_put(LED_PIN, 1);
        vTaskDelay(100); // Ticks to suspend task and allow other tasks to run
        gpio_put(LED_PIN, 0);
        vTaskDelay(100);
    }
}

int main()
{
    stdio_init_all();

    // Words on task stack, task args, priority, NULL
    xTaskCreate(led_task, "LED Task", 256, NULL, 1, NULL);
    vTaskStartScheduler();

    while (1)
    {
    };
}