#include "header.h"

uint8_t lastblink;
void blink()
{

    switch (lastblink)
    {
    case 0:
        lastblink = 1;
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
        break;
    case 1:
        lastblink = 0;
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
        break;

    default:
        break;
    }
    // sleep_ms(100);
}

void initI2C()
{
    i2c_init(I2C_ID, 100000);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SCL_PIN);
    gpio_pull_up(I2C_SDA_PIN);
}

void initGPIO()
{

    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_init(GPO);
    gpio_set_dir(GPO, GPIO_OUT);
    gpio_init(PEAK_RST);
    gpio_set_dir(PEAK_RST, GPIO_OUT);
}