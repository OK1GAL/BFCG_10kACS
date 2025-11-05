#ifndef LIB_INIT
#define LIB_INIT

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "hardware/sync.h"
//#include "hardware/pio.h"
//#include "hardware/irq.h"
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "hardware/i2c.h"
#include "hardware/regs/dma.h"

//************GPIO*************//

extern float Vref;

void blink();

//************GPIO*************//
#define PEAK_RST 7
#define CURRENT_COMP 24
#define PEAK_COMP 23
#define PEAK_ADC_CH 1
#define CURRENT_ADC_CH 0
#define GPO 3
#define GPI 8

void initGPIO();
void gpio_callback(uint gpio, uint32_t events);

//******CURRENT SENSING********//
void setup_capture();
void handle_capture_trigger();
void handle_current_captures();

//************COMMS************//
#define UART_ID uart0
#define BAUD_RATE 9600
#define UART_TX_PIN 0
#define UART_RX_PIN 1
#define UART_DIR_PIN 2

void initcomms();
void linesendchar(char data);
void linesendstring(char *data);

//************I2C**************//
#define I2C_ID i2c0
#define I2C_SCL_PIN 5
#define I2C_SDA_PIN 4

void initI2C();

//************DAC**************//
#define dac_addr 0x60

void MCP4716_set(uint16_t Vout,int Gain, int Vref);

//************EEPROM***********//


//************LCD**************//
#define LCD_SPI_PORT spi0
#define LCD_RES 20
#define LCD_DC 21
#define LCD_CS 17

#define ST7735_BLACK 0x0000
#define ST7735_BLUE 0x001F
#define ST7735_RED 0xF800
#define ST7735_GREEN 0x07E0
#define ST7735_CYAN 0x07FF
#define ST7735_MAGENTA 0xF81F
#define ST7735_YELLOW 0xFFE0
#define ST7735_WHITE 0xFFFF

void LCDinit();
void fillRectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color);
void fillScreen(uint16_t color);
void setRotation(uint8_t m);
void drawText(uint8_t x, uint8_t y, const char *_text, uint16_t color, uint16_t bg, uint8_t size);
void drawarray(int x, int y, int width, int height, uint16_t array[]);
int16_t Color565(int16_t r, int16_t g, int16_t b);
//****function prototypes******//

//*****************************//

#endif