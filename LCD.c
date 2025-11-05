#include "header.h"
#include "font.h"


#define ST7735_NOP 0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID 0x04
#define ST7735_RDDST 0x09

#define ST7735_SLPIN 0x10
#define ST7735_SLPOUT 0x11
#define ST7735_PTLON 0x12
#define ST7735_NORON 0x13

#define ST7735_INVOFF 0x20
#define ST7735_INVON 0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON 0x29
#define ST7735_CASET 0x2A
#define ST7735_RASET 0x2B
#define ST7735_RAMWR 0x2C
#define ST7735_RAMRD 0x2E

#define ST7735_PTLAR 0x30
#define ST7735_COLMOD 0x3A
#define ST7735_MADCTL 0x36

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR 0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1 0xC0
#define ST7735_PWCTR2 0xC1
#define ST7735_PWCTR3 0xC2
#define ST7735_PWCTR4 0xC3
#define ST7735_PWCTR5 0xC4
#define ST7735_VMCTR1 0xC5

#define ST7735_RDID1 0xDA
#define ST7735_RDID2 0xDB
#define ST7735_RDID3 0xDC
#define ST7735_RDID4 0xDD

#define ST7735_PWCTR6 0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

#define ST7735_MADCTL_MY 0x80
#define ST7735_MADCTL_MX 0x40
#define ST7735_MADCTL_MV 0x20
#define ST7735_MADCTL_ML 0x10
#define ST7735_MADCTL_BGR 0x08

#define LCD_ASCII_OFFSET 0x20

#define __delay_ms(x) sleep_ms(x)

#define spiwrite(data) spi_write_blocking(LCD_SPI_PORT, &data, 1)

#define tft_cs_low()                   \
    asm volatile("nop \n nop \n nop"); \
    gpio_put(LCD_CS, 0);               \
    asm volatile("nop \n nop \n nop")
#define tft_cs_high()                  \
    asm volatile("nop \n nop \n nop"); \
    gpio_put(LCD_CS, 1);               \
    asm volatile("nop \n nop \n nop")

#define tft_dc_low()                   \
    asm volatile("nop \n nop \n nop"); \
    gpio_put(LCD_DC, 0);               \
    asm volatile("nop \n nop \n nop")
#define tft_dc_high()                  \
    asm volatile("nop \n nop \n nop"); \
    gpio_put(LCD_DC, 1);               \
    asm volatile("nop \n nop \n nop")

#define tft_rst_low()                  \
    asm volatile("nop \n nop \n nop"); \
    gpio_put(LCD_RES, 0);              \
    asm volatile("nop \n nop \n nop")
#define tft_rst_high()                 \
    asm volatile("nop \n nop \n nop"); \
    gpio_put(LCD_RES, 1);              \
    asm volatile("nop \n nop \n nop")

//**************************************************end of defines************************************************//

uint8_t _colstart = 26, _rowstart = 1, _tft_type, _rotation = 0, _xstart = 0, _ystart = 0;

uint8_t tft_width = 160, tft_height = 80;

// because of code compatibilty
#define _width         tft_width
#define _height        tft_height

bool _wrap = true;

uint16_t colorshift(uint16_t color)
{
  color = (color&0b0000000000011111) || ((color&0b0000011111000000)>>1) || (1);
  //return color;
}

void lcd_spi_init()
{

    gpio_init(LCD_CS);
    gpio_set_dir(LCD_CS, GPIO_OUT);
    gpio_put(LCD_CS, 1); // Chip select is active-low

    gpio_init(LCD_DC);
    gpio_set_dir(LCD_DC, GPIO_OUT);
    gpio_put(LCD_DC, 0); // Chip select is active-low

    gpio_init(LCD_RES);
    gpio_set_dir(LCD_RES, GPIO_OUT);
    gpio_put(LCD_RES, 0);
}

void write_command(uint8_t cmd_)
{
    tft_dc_low();
    tft_cs_low();
    spiwrite(cmd_);
    tft_cs_high();
}

// Write SPI data
void write_data(uint8_t data_)
{
    tft_dc_high();
    tft_cs_low();
    spiwrite(data_);
    tft_cs_high();
}

void TFT_ResetPIN()
{
    tft_rst_high();
    __delay_ms(10);
    tft_rst_low();
    __delay_ms(10);
    tft_rst_high();
    __delay_ms(10);
}

void Bcmd()
{
    write_command(ST7735_SWRESET);
    __delay_ms(50);
    write_command(ST7735_SLPOUT);
    __delay_ms(250);
    __delay_ms(250);
    write_command(ST7735_COLMOD);
    write_data(0x05);
    __delay_ms(10);
    write_command(ST7735_FRMCTR1);
    write_data(0x00);
    write_data(0x06);
    write_data(0x03);
    __delay_ms(10);
    write_command(ST7735_MADCTL);
    write_data(0x08);
    write_command(ST7735_DISSET5);
    write_data(0x15);
    write_data(0x02);
    write_command(ST7735_INVCTR);
    write_data(0x00);
    write_command(ST7735_PWCTR1);
    write_data(0x02);
    write_data(0x70);
    __delay_ms(10);
    write_command(ST7735_PWCTR2);
    write_data(0x05);
    write_command(ST7735_PWCTR3);
    write_data(0x01);
    write_data(0x02);
    write_command(ST7735_VMCTR1);
    write_data(0x3C);
    write_data(0x38);
    __delay_ms(10);
    write_command(ST7735_PWCTR6);
    write_data(0x11);
    write_data(0x15);
    write_command(ST7735_GMCTRP1);
    write_data(0x09);
    write_data(0x16);
    write_data(0x09);
    write_data(0x20);
    write_data(0x21);
    write_data(0x1B);
    write_data(0x13);
    write_data(0x19);
    write_data(0x17);
    write_data(0x15);
    write_data(0x1E);
    write_data(0x2B);
    write_data(0x04);
    write_data(0x05);
    write_data(0x02);
    write_data(0x0E);
    write_command(ST7735_GMCTRN1);
    write_data(0x0B);
    write_data(0x14);
    write_data(0x08);
    write_data(0x1E);
    write_data(0x22);
    write_data(0x1D);
    write_data(0x18);
    write_data(0x1E);
    write_data(0x1B);
    write_data(0x1A);
    write_data(0x24);
    write_data(0x2B);
    write_data(0x06);
    write_data(0x06);
    write_data(0x02);
    write_data(0x0F);
    __delay_ms(10);
    write_command(ST7735_CASET);
    write_data(0x00);
    write_data(0x02);
    write_data(0x08);
    write_data(0x81);
    write_command(ST7735_RASET);
    write_data(0x00);
    write_data(0x01);
    write_data(0x08);
    write_data(0xA0);
    write_command(ST7735_NORON);
    __delay_ms(10);
    write_command(ST7735_DISPON);
    __delay_ms(250);
    __delay_ms(250);
}

void Rcmd1()
{
    write_command(ST7735_SWRESET);
    __delay_ms(150);
    write_command(ST7735_SLPOUT);
    __delay_ms(250);
    __delay_ms(250);
    write_command(ST7735_FRMCTR1);
    write_data(0x01);
    write_data(0x2C);
    write_data(0x2D);
    write_command(ST7735_FRMCTR2);
    write_data(0x01);
    write_data(0x2C);
    write_data(0x2D);
    write_command(ST7735_FRMCTR3);
    write_data(0x01);
    write_data(0x2C);
    write_data(0x2D);
    write_data(0x01);
    write_data(0x2C);
    write_data(0x2D);
    write_command(ST7735_INVCTR);
    write_data(0x07);
    write_command(ST7735_PWCTR1);
    write_data(0xA2);
    write_data(0x02);
    write_data(0x84);
    write_command(ST7735_PWCTR2);
    write_data(0xC5);
    write_command(ST7735_PWCTR3);
    write_data(0x0A);
    write_data(0x00);
    write_command(ST7735_PWCTR4);
    write_data(0x8A);
    write_data(0x2A);
    write_command(ST7735_PWCTR5);
    write_data(0x8A);
    write_data(0xEE);
    write_command(ST7735_VMCTR1);
    write_data(0x0E);
    write_command(ST7735_INVOFF);
    write_command(ST7735_MADCTL);
    write_data(0xC8);
    write_command(ST7735_COLMOD);
    write_data(0x05);
}
void Rcmd2green()
{
    write_command(ST7735_CASET);
    write_data(0x00);
    write_data(0x02);
    write_data(0x00);
    write_data(0x7F + 0x02);
    write_command(ST7735_RASET);
    write_data(0x00);
    write_data(0x01);
    write_data(0x00);
    write_data(0x9F + 0x01);
}
void Rcmd2red()
{
    write_command(ST7735_CASET);
    write_data(0x00);
    write_data(0x00);
    write_data(0x00);
    write_data(0x7F);
    write_command(ST7735_RASET);
    write_data(0x00);
    write_data(0x00);
    write_data(0x00);
    write_data(0x9F);
}

void Rcmd3()
{
    write_command(ST7735_GMCTRP1);
    write_data(0x02);
    write_data(0x1C);
    write_data(0x07);
    write_data(0x12);
    write_data(0x37);
    write_data(0x32);
    write_data(0x29);
    write_data(0x2D);
    write_data(0x29);
    write_data(0x25);
    write_data(0x2B);
    write_data(0x39);
    write_data(0x00);
    write_data(0x01);
    write_data(0x03);
    write_data(0x10);
    write_command(ST7735_GMCTRN1);
    write_data(0x03);
    write_data(0x1D);
    write_data(0x07);
    write_data(0x06);
    write_data(0x2E);
    write_data(0x2C);
    write_data(0x29);
    write_data(0x2D);
    write_data(0x2E);
    write_data(0x2E);
    write_data(0x37);
    write_data(0x3F);
    write_data(0x00);
    write_data(0x00);
    write_data(0x02);
    write_data(0x10);
    write_command(ST7735_NORON);
    __delay_ms(10);
    write_command(ST7735_DISPON);
    __delay_ms(100);
}

/*
  SPI displays set an address window rectangle for blitting pixels
  Parameter1:  x0 Top left corner x coordinate
  Parameter2:  y0 Top left corner y coordinate
  Parameter3:  x1 Lower right corner x coordinate
  Parameter4:  y1 Lower right corner y coordinate
 https://en.wikipedia.org/wiki/Bit_blit
*/

//
// uint16_t rgbtocolor(uint8_t R, uint8_t G, uint8_t B)
// {
//   uint16_t color = (B&0b00011111)||((G&0b00111111)<<5)||((R&0b00011111)<<);
//   return 
// }

int16_t Color565(int16_t r, int16_t g, int16_t b){           
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}


void setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
    write_command(ST7735_CASET);
    write_data(0);
    write_data(x0 + _xstart);
    write_data(0);
    write_data(x1 + _xstart);
    write_command(ST7735_RASET);
    write_data(0);
    write_data(y0 + _ystart);
    write_data(0);
    write_data(y1 + _ystart);
    write_command(ST7735_RAMWR); // Write to RAM
}

void fillRectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color){
  uint8_t hi, lo;
  if((x >= _width) || (y >= _height))
    return;
  if((x + w - 1) >= _width)  
    w = _width  - x;
  if((y + h - 1) >= _height) 
    h = _height - y;
  setAddrWindow(x, y, x+w-1, y+h-1);
  hi = color >> 8; lo = color;
  tft_dc_high();
  tft_cs_low();
  for(y=h; y>0; y--) {
    for(x = w; x > 0; x--) {
      spiwrite(hi);
      spiwrite(lo);
    }
  }
  tft_cs_high() ;
}

void fillScreen(uint16_t color) {
  fillRectangle(0, 0, _width, _height, color);
}


void drawFastHLine(uint8_t x, uint8_t y, uint8_t w, uint16_t color){
  uint8_t hi, lo;
  if((x >= _width) || (y >= _height))
    return;
  if((x + w - 1) >= _width)
    w = _width - x;
  hi = color >> 8; lo = color;
  setAddrWindow(x, y, x + w - 1, y);
  tft_dc_high();
  tft_cs_low();
  while (w--) {
    spiwrite(hi);
    spiwrite(lo);
  }
  tft_cs_high() ;
}

void drawFastVLine(uint8_t x, uint8_t y, uint8_t h, uint16_t color){
  uint8_t hi, lo;
  if((x >= _width) || (y >= _height))
    return;
  if((y + h - 1) >= _height)
    h = _height - y;
  hi = color >> 8; lo = color;
  setAddrWindow(x, y, x, y + h - 1);
  tft_dc_high();
  tft_cs_low();
  while (h--) {
    spiwrite(hi);
    spiwrite(lo);
  }
  tft_cs_high() ;
}

void drawarray(int x, int y, int width, int height , uint16_t array[])
{
  setAddrWindow(x,y,x+width-1,y+height-1);
  tft_dc_high();
  tft_cs_low();
  uint8_t hi, lo;
  for(int i = 0;i<height;i++)
  {
    for(int j = 0;j<width;j++)
    {
      hi = array[i*width+j] >> 8; lo = array[i*width+j];
      spiwrite(hi);
      spiwrite(lo);
    }
  }
  tft_cs_high();
}

void fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color) {
  int16_t i;
  for (i = x; i < x + w; i++) {
    drawFastVLine(i, y, h, color);
  }
}

void setRotation(uint8_t m) {
  // m can be 0-3
  uint8_t madctl = 0;

  _rotation = m % 4;

  switch (_rotation) {
  case 0:
    madctl = ST7735_MADCTL_MX | ST7735_MADCTL_MY | ST7735_MADCTL_BGR;
    _xstart = _colstart;
    _ystart = _rowstart;
    break;
  case 1:
    madctl = ST7735_MADCTL_MY | ST7735_MADCTL_MV | ST7735_MADCTL_BGR;
    _ystart = _colstart;
    _xstart = _rowstart;
    break;
  case 2:
    madctl = ST7735_MADCTL_BGR;
    _xstart = _colstart;
    _ystart = _rowstart;
    break;
  case 3:
    madctl = ST7735_MADCTL_MX | ST7735_MADCTL_MV | ST7735_MADCTL_BGR;
    _ystart = _colstart;
    _xstart = _rowstart;
    break;
  }
  write_command(ST7735_MADCTL);
  write_data(madctl);
}

void invertDisplay(bool i) {
  if(i)
    write_command(ST7735_INVON);
  else
    write_command(ST7735_INVOFF);
}

void LCDinit()
{

    lcd_spi_init();

    TFT_ResetPIN();

    tft_dc_low();
    Rcmd1();
    Rcmd2red();
    Rcmd3();
    write_command(ST7735_MADCTL);
    write_data(0xC8);

    invertDisplay(1);
}

void drawPixel(uint8_t x, uint8_t y, uint16_t color){
  if((x >= _width) || (y >= _height)) 
    return;
  setAddrWindow(x,y,x+1,y+1);
  write_data(color >> 8);
  write_data(color & 0xFF);
}


void drawChar(uint8_t x, uint8_t y, uint8_t c, uint16_t color, uint16_t bg,  uint8_t size){
  int8_t i, j;
  if((x >= _width) || (y >= _height))
    return;
  if(size < 1) size = 1;
  if((c < ' ') || (c > '~'))
    c = '?';
  for(i=0; i<5; i++ ) {
    uint8_t line;
    line = FontBasic[(c - LCD_ASCII_OFFSET)*5 + i];
    for(j=0; j<7; j++, line >>= 1) {
      if(line & 0x01) {
        if(size == 1) drawPixel(x+i, y+j, color);
        else
        {
            fillRect(x+(i*size), y+(j*size), size, size, color);
        }
      }
      else if(bg != color) {
           if(size == 1) drawPixel(x+i, y+j, bg);
           else          fillRect(x+i*size, y+j*size, size, size, bg);
        }
    }
  }
}

void setTextWrap(bool w){
  _wrap = w;
}

// Draw text character array to screen
void drawText(uint8_t x, uint8_t y, const char *_text, uint16_t color, uint16_t bg, uint8_t size) {
  uint8_t cursor_x, cursor_y;
  uint16_t textsize, i;
  cursor_x = x, cursor_y = y;
  textsize = strlen(_text);
  for(i = 0; i < textsize; i++){
    if(_wrap && ((cursor_x + size * 5) > _width)) {
      cursor_x = 0;
      cursor_y = cursor_y + size * 7 + 3 ;
      if(cursor_y > _height) cursor_y = _height;
      if(_text[i] == LCD_ASCII_OFFSET) {
        continue;
      }
    }
    drawChar(cursor_x, cursor_y, _text[i], color, bg, size);
    cursor_x = cursor_x + size * 6;
    if(cursor_x > _width) {
      cursor_x = _width;
    }
  }
}