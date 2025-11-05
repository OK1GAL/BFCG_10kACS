#include "header.h"

// --- Configuration ---
#define CAPTURE_CHANNEL 0        // ADC0 (GPIO 26) - assuming your current sensor is here
#define CAPTURE_DEPTH 8192       // Total size of the buffer (N_total)
#define PRE_TRIGGER_SAMPLES 4096 // Samples before the trigger to keep (N_pre)
#define POST_TRIGGER_SAMPLES (CAPTURE_DEPTH - PRE_TRIGGER_SAMPLES)
// --- Globals ---
uint16_t capture_buf[CAPTURE_DEPTH];
uint dma_current_capture_channel;

void setup_capture()
{
    adc_init();
    adc_gpio_init(26 + CAPTURE_CHANNEL); // Initialize the GPIO pin for ADC input
    adc_select_input(CAPTURE_CHANNEL);
    adc_set_clkdiv(0);
    adc_fifo_setup(
        true,  // Write each completed conversion to the sample FIFO
        true,  // Enable DMA data request (DREQ)
        1,     // DREQ (and IRQ) asserted when at least 1 sample present
        false, // We won't see the ERR bit because of 8 bit reads; disable.
        false  // Shift each sample to 8 bits when pushing to FIFO
    );
    adc_run(false);

    // 1. Configure the DMA channel
    dma_current_capture_channel = dma_claim_unused_channel(true);
    dma_channel_config c = dma_channel_get_default_config(dma_current_capture_channel);

    channel_config_set_transfer_data_size(&c, DMA_SIZE_16);
    channel_config_set_read_increment(&c, false); // Reading from fixed ADC_RESULT register
    channel_config_set_write_increment(&c, true); // Writing to different locations in capture_buf

    // KEY: Configure the DMA write address to wrap around at the buffer end
    channel_config_set_ring(&c, true, 15);
    channel_config_set_dreq(&c, DREQ_ADC); // Triggered by ADC conversion

    // 2. Initial DMA Configuration and Start

    dma_channel_configure(
        dma_current_capture_channel,
        &c,
        capture_buf,   // Destination (write to buffer)
        &adc_hw->fifo, // Source (read from ADC result register)
        CAPTURE_DEPTH, // Number of transfers (total buffer size)
        true           // Start immediately
    );
    busy_wait_ms(100);
    // 3. Start ADC continuous conversion (free-running mode)
    adc_run(true);

    printf("DMA addr: %u\n\r", (dma_hw->ch[dma_current_capture_channel].write_addr));
    busy_wait_ms(100);
    printf("DMA addr: %u\n\r", (dma_hw->ch[dma_current_capture_channel].write_addr));
    busy_wait_ms(100);
    printf("DMA addr: %u\n\r", (dma_hw->ch[dma_current_capture_channel].write_addr));
    busy_wait_ms(100);
    printf("DMA addr: %u\n\r", (dma_hw->ch[dma_current_capture_channel].write_addr));

    gpio_set_irq_enabled_with_callback(PEAK_COMP, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
    gpio_put(PEAK_RST, 0);
}

void print_captured_data()
{
    // 1. Get the current DMA write address (where it *will* write next)
    // We read the hardware register directly.
    uint32_t current_dma_address = dma_hw->ch[dma_current_capture_channel].write_addr;

    if (current_dma_address == 0)
    {
        printf("!!! ERROR: DMA write address is 0. Capture failed or DMA is not running. !!!\n");
        return;
    }

    // Calculate the index relative to the start of the capture_buf array.
    // The DMA address is a byte address, so divide by the size of the data type (uint16_t = 2 bytes)
    // to get the array index.
    uint32_t next_write_idx = (current_dma_address - (uint32_t)capture_buf) / sizeof(uint16_t);

    // The trigger event happened PRE_TRIGGER_SAMPLES before the capture stopped.
    // Since the DMA stopped after POST_TRIGGER_SAMPLES were recorded, the stop address
    // (next_write_idx) is the end of the post-trigger capture.

    // Calculate the logical start index of the captured event.
    // The relevant data starts (POST_TRIGGER_SAMPLES) samples BEFORE the stop address,
    // wrapping around the buffer.
    uint32_t start_idx = (next_write_idx + POST_TRIGGER_SAMPLES) % CAPTURE_DEPTH;

    // Note: The actual trigger point occurred PRE_TRIGGER_SAMPLES samples *after* start_idx.

    printf("\n--- Capture Complete! ---\n");
    printf("DMA stop address (next write): %lu\n", current_dma_address);
    printf("Logical stop index: %lu\n", next_write_idx);
    printf("Effective start index for readout: %lu\n", start_idx);

    // The data loop prints the circular buffer correctly, starting from the oldest
    // relevant sample (start_idx) and proceeding chronologically.

    for (int i = 0; i < CAPTURE_DEPTH; i++)
    {
        uint32_t read_idx = (start_idx + i) % CAPTURE_DEPTH;
        uint16_t value = capture_buf[read_idx];

        if (i == PRE_TRIGGER_SAMPLES)
        {
            // This marks the approximate trigger point
            printf("--- TRIGGER POINT ---\n");
        }

        printf("Address:  %u Sample %d: %d\n", read_idx, i, value);
    }
}

uint8_t capture_state = 0;
#define STATE_RUNNING 0
#define STATE_POST_WAIT 1
#define STATE_READY_TO_PRINT 2

uint32_t dma_address_at_trigger = 0;

void handle_capture_trigger()
{
    if (capture_state == STATE_RUNNING)
    {
        gpio_set_irq_enabled(PEAK_COMP, GPIO_IRQ_EDGE_RISE, false);
        gpio_put(PEAK_RST, 1); // Reset the peak comparator immediately
        // DO NOT stop ADC/DMA here. Let them run for POST_TRIGGER_SAMPLES.
        dma_address_at_trigger = dma_hw->ch[dma_current_capture_channel].write_addr;
        capture_state = STATE_POST_WAIT;
    }
}

void handle_current_captures()
{
    static uint32_t post_trigger_counter = 0;

    if (capture_state == STATE_POST_WAIT)
    {
        long samples_from_trigger = dma_hw->ch[dma_current_capture_channel].write_addr - dma_address_at_trigger;

        if (samples_from_trigger < POST_TRIGGER_SAMPLES)
        {
            /* code */
        }
        

        if (post_trigger_counter < POST_TRIGGER_SAMPLES)
        {
            post_trigger_counter++;
        }
        else
        {
            // 2. Post-trigger samples captured! Stop the process.
            adc_run(false);
            busy_wait_ms(10);
            dma_channel_abort(dma_current_capture_channel);

            printf("Post-trigger capture complete. Stopping DMA/ADC.\n");
            capture_state = STATE_READY_TO_PRINT;
            post_trigger_counter = 0; // Reset counter for next capture
        }
    }
    else if (capture_state == STATE_READY_TO_PRINT)
    {
        // 3. Print the data
        print_captured_data();

        // 4. Restart the entire process
        dma_channel_start(dma_current_capture_channel); // Restart DMA
        adc_select_input(CAPTURE_CHANNEL);
        adc_run(true);
        gpio_put(PEAK_RST, 0);
        capture_state = STATE_RUNNING;
        gpio_set_irq_enabled(PEAK_COMP, GPIO_IRQ_EDGE_RISE, true);
        printf("Capture and printout finished.\n");
    }
}