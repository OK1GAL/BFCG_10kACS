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

//************FFT**************//

#define ADC_PIN 26
#define ADC_CHAN 0
typedef signed int fix15;
#define multfix15(a, b) ((fix15)((((signed long long)(a)) * ((signed long long)(b))) >> 15))
#define float2fix15(a) ((fix15)((a)*32768.0)) // 2^15
#define fix2float15(a) ((float)(a) / 32768.0)
#define absfix15(a) abs(a)
#define int2fix15(a) ((fix15)(a << 15))
#define fix2int15(a) ((int)(a >> 15))
#define char2fix15(a) (fix15)(((fix15)(a)) << 15)

// Number of samples
#define NUM_SAMPLES 1024
// Number of samples per FFT, minus 1
#define NUM_SAMPLES_M_1 1023
// Length of short (16 bits) minus log2 number of samples (10)
#define SHIFT_AMOUNT 6
// Log2 number of samples
#define LOG2_NUM_SAMPLES 10

// Max and min macros
#define max(a, b) ((a > b) ? a : b)
#define min(a, b) ((a < b) ? a : b)

extern uint16_t fspectrumsamples[160];
extern uint16_t waterfall[160 * 40];
extern uint16_t S_meter;

void FFTfix(fix15 fr[], fix15 fi[]);
void initADC_DMA_FFT();
void commitFFT();
//*****************************//

//*****************************//

//****function prototypes******//

//*****************************//

#endif