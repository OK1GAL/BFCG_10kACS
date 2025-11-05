// cp pi_pico/GaliumV001/build/GaliumV001.uf2 /media/ok1gal/RPI-RP2
#include "header.h"

static int rotation;

uint8_t fast = 0;
uint64_t lastbtnpressmillis;

char buf[32];
int x = 0;
int y = 0;

uint16_t adcsample = 0;
float adcvoltage = 0;
float Vref = 3.208;
float current = 0;
float maxcurrent = 0;
float peak_callback_time = 0;

void gpio_callback(uint gpio, uint32_t events)
{
    if (gpio == PEAK_COMP)
    {
        adc_select_input(PEAK_ADC_CH);
        adcsample = adc_read();
        gpio_put(PEAK_RST, 1);
    }
}

void core1_entry()
{
    initcomms();
    gpio_put(GPO, 0);
    while (1)
    {
        tight_loop_contents();
        blink();
        busy_wait_ms(300);
        // linesendstring("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
        // linesendchar('a');
        gpio_put(GPO, 0);
        /* while (uart_is_readable(UART_ID))
        {

            gpio_put(GPO, 1);
            uint8_t ch = uart_getc(UART_ID);
            // Can we send it back?
            if (uart_is_writable(UART_ID))
            {
                // Change it slightly first!
                linesendchar(ch);
            }
        } */
    }
}

int main()
{
    stdio_init_all();
    // adc_init();
    // adc_gpio_init(26);
    // adc_select_input(0);
    // initADC_DMA_FFT();

    // initcomms();

    adc_init();

    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(26);
    adc_gpio_init(27);
    // Select ADC input 0 (GPIO26)
    adc_select_input(0);
    // Select ADC input 1 (GPIO27)
    adc_select_input(1);

    initGPIO();

    spi_init(LCD_SPI_PORT, 60000000);
    gpio_set_function(16, GPIO_FUNC_SPI);
    gpio_set_function(18, GPIO_FUNC_SPI);
    gpio_set_function(19, GPIO_FUNC_SPI);

    initI2C();

    LCDinit();
    setRotation(3);
    fillScreen(ST7735_BLACK);
    for (int i = 0; i < 3; i++)
    {
        gpio_put(UART_DIR_PIN, 1);
        sleep_ms(200);
        gpio_put(UART_DIR_PIN, 0);
        sleep_ms(200);
    }
    for (int i = 0; i < 3; i++)
    {
        gpio_put(GPO, 1);
        sleep_ms(100);
        gpio_put(GPO, 0);
        sleep_ms(100);
    }

    gpio_put(PEAK_RST, 1);
    sleep_ms(100);
    gpio_put(PEAK_RST, 0);

    // spectrum_redraw();

    multicore_launch_core1(core1_entry);

    sprintf(buf, "%d ", adc_read());
    drawText(1, 1, buf, ST7735_BLUE, ST7735_BLACK, 3);

    // si_init();
    // sleep_ms(10);
    // si_setphase(0, 0);

    MCP4716_set(512, 2, 1);

    gpio_put(PEAK_RST, 1);
    sleep_ms(100);
    gpio_put(PEAK_RST, 0);

    gpio_set_irq_enabled_with_callback(PEAK_COMP, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);

    while (true)
    {
        // adcsample = adc_read();
        sprintf(buf, "%d ", adcsample);
        drawText(1, 1, buf, ST7735_BLUE, ST7735_BLACK, 2);
        adcvoltage = (float)adcsample * (Vref / 4096);
        sprintf(buf, "%.4f V", adcvoltage);
        drawText(1, 17, buf, ST7735_BLUE, ST7735_BLACK, 2);
        current = (adcvoltage / 6.6) / 0.0005;
        sprintf(buf, "%.1f A", current);
        drawText(1, 34, buf, ST7735_BLUE, ST7735_BLACK, 2);
        if (current > maxcurrent)
        {
            maxcurrent = current;
        }
        sprintf(buf, "%.1f A", maxcurrent);
        drawText(1, 51, buf, ST7735_BLUE, ST7735_BLACK, 2);
        sleep_ms(1000);
        gpio_put(PEAK_RST, 0);
        blink();

        // sleep_ms(100);
        //  si_evaluate(0, 10100000UL);
        //  si_enable(0, true);
        //  blink();
        //  freq_redraw();
        //  spectrum_redraw();
        //  drawarray(0, 40, 160, 40, waterfall);
        //  sleep_ms(100);
        //  redraw_display();
        //  y = y + 1;
    }
}
