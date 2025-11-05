#include "header.h"

// 1=Vdd, 2=VrefUnbuf, 3=VrefBuff
void MCP4716_set(uint16_t Vout, int Gain, int Vref)
{
    uint16_t set = Vout << 6;
    uint8_t set_upper = (uint8_t)(set >> 8);
    uint8_t set_l = (uint8_t)set;
    uint8_t set_lower = (uint8_t)set_l;

    uint8_t command;
    command = 0b01100000; // Command Bit

    if (Vref == 1)
    {
        command = command || 0b00000000; // Vdd
    }
    else if (Vref == 2)
    {
        command = command || 0b00010000; // VrefUnbuf
    }
    else if (Vref == 3)
    {
        command = command || 0b00011000; // VrefBuff
    }

    if (Gain == 1)
    {
        command = command || 0b00000000;
        ; // Gain=1
    }
    else
        command = command || 0b00000001; // Gain =2

    uint8_t dac_data[3] = {command, set_upper, set_lower};

    i2c_write_blocking(I2C_ID, dac_addr, dac_data, 3, 0);
};