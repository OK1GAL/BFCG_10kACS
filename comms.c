#include "header.h"
//jebata
// 0xFF/0xFF/ADDR/CMD/NumOfData/Data/Checksum/0xFF/0xFF//

uint8_t RXbuf[1024];
uint16_t RX_byte_num = 0;
uint8_t last_byte = 0;
uint8_t in_packet = 0;
uint8_t packet_rdy;


void parse_command()
{
    
}

// RX interrupt handler
void on_uart_rx()
{
    while (uart_is_readable(UART_ID))
    {
        uint8_t ch = uart_getc(UART_ID);
        RXbuf[RX_byte_num] = ch;
        RX_byte_num++;
        if(in_packet==0 && (last_byte == ch == 0xFF))
        {
            in_packet = 1;
            packet_rdy = 0;
        }
        if(in_packet==1 && (last_byte == ch == 0xFF))
        {
            in_packet = 0;
            packet_rdy = 1;
            parse_command();
        }
    }
}

void initcomms()
{
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // uart_set_fifo_enabled(UART_ID, false);

    irq_set_exclusive_handler(UART0_IRQ, on_uart_rx);
    irq_set_enabled(UART0_IRQ, true);

    uart_set_irq_enables(UART_ID, true, false);

    gpio_init(UART_DIR_PIN);
    gpio_set_dir(UART_DIR_PIN, GPIO_OUT);
}

void linesendchar(char data)
{
    gpio_put(UART_DIR_PIN, 1);
    uart_putc(UART_ID, data);
    uart_tx_wait_blocking(UART_ID);
    gpio_put(UART_DIR_PIN, 0);
}
void linesendstring(char *data)
{
    gpio_put(UART_DIR_PIN, 1);
    // uart_putc(UART_ID,data);
    uart_puts(UART_ID, data);
    uart_tx_wait_blocking(UART_ID);
    gpio_put(UART_DIR_PIN, 0);
}
