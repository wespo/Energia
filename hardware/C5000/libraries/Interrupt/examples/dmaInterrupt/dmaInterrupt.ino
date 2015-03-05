/*
  Interrupt Example for DMA
  Configures all the DMA channels for data transfer, in interrupt mode.
  DMA is configured with proper source and destination address and data length.
  Data in the source buffer is copied into the destination buffer, using DMA.
  The read and write buffer contents should match with each other.
*/

#define DMA_BUFFER_SIZE    (512u)

/* Variable to indicate that the DMA transfer has completed for the respective
   DMA channel */
volatile int interrupt_occurred = 0;

/* Variables to store Source and Destination buffer addresses used during DMA
   transfers */
unsigned short *dmaSRCBuff = NULL;
unsigned short *dmaDESTBuff = NULL;

/*
 * Simple DMA ISR that reads the interrupt status and clears it, finally informs
 * the setup() API that DMA transfer has completed for the current DMA channel
 */
interrupt void dma_isr(void)
{
    unsigned int ifrValue;

    ifrValue = DMA.getInterruptStatus();
    DMA.clearInterruptStatus(ifrValue);

    interrupt_occurred = 1;
}

void setup()
{
    DMAConfig      dmaConfig;
    int            status;
    unsigned short chanNumber;
    unsigned short index;
    volatile int   delay;
    Serial.begin(9600);
    Serial.println("\r\nDMA INTERRUPT MODE DEMO!");

    /* DMA.dmaInit() creates DMA data buffers and initializes the config buffer
     * The DMA.dmaInit() API will initialize the config structure passed to it
     * with the default config values
     */
    status = DMA.dmaInit(&dmaConfig, DMA_BUFFER_SIZE);
    if (status != 0)
    {
        return;
    }

    dmaSRCBuff  = dmaConfig.srcAddr;
    dmaDESTBuff = dmaConfig.destAddr;

    dmaConfig.enableDmaInt = 1; /* Enable DMA interrupt */

    initInterrupt(0x0000);
    attachInterrupt(INTERRUPT_DMA, (INTERRUPT_IsrPtr)dma_isr, 0);

    /* Enabling Interrupt */
    enableInterrupt(INTERRUPT_DMA);

    /* Initialize DMA */
    DMA.init();

    /* Test all the DMA channels by opening each of the channels, configuring
     * them to transfer data from 'dmaSRCBuff' and 'dmaDESTBuff' buffers.
     * Finally, once the DMA transfer completes compare the source and
     * destination buffer contents
     */
    for (chanNumber = 0; chanNumber < DMA_CHANNEL_MAX; chanNumber++)
    {
        /* Set value to indicate that the DMA transfer has not yet completed for
           the current DMA channel */
        interrupt_occurred = 0;

        /* Initialize the DMA Source and Destination buffers */
        for (index = 0; index < DMA_BUFFER_SIZE; index++)
        {
            dmaSRCBuff[index]  = index;
            dmaDESTBuff[index] = 0x0000;
        }

        /* Opens the respective DMA channel */
        status = DMA.openChannel(chanNumber);
        if (status != 0)
        {
            break;
        }

        /* Configure the respective DMA channel */
        status = DMA.configChannel(chanNumber, dmaConfig);
        if (status != 0)
        {
            break;
        }

        /* Start DMA transfer for the respective DMA channel */
        status = DMA.start(chanNumber);
        if (status != 0)
        {
            break;
        }

        /* Wait for the respective DMA transfer to complete */
        while (1)
        {
            for (delay = 0; delay < 1000; delay++) ;

            /* The value of the variable 'interrupt_occurred' will be set in
               the DMA ISR, which will be triggered once the current DMA
               transfer completes */
            if (1 == interrupt_occurred)
            {
                break;
            }
        }

        /* Close the respective DMA channel */
        status = DMA.closeChannel(chanNumber);
        if (status != 0)
        {
            break;
        }

        for (index = 0; index < DMA_BUFFER_SIZE; index++)
        {
            if (dmaSRCBuff[index] != dmaDESTBuff[index])
            {
                Serial.print("Buffer mismatched at position ");
                Serial.print((long)index);
                Serial.print(", for channel ");
                Serial.print((long)chanNumber);
                break;
            }
        }

        /* When index will be equal to DMA_BUFFER_SIZE, then both the source
           and destination buffers would have matched */
        if (index == DMA_BUFFER_SIZE)
        {
            Serial.print("DMA Transfer for Channel No: ");
            Serial.print((long)chanNumber);
            Serial.println(" Successful");
            Serial.println("Source and Destination Buffers Match!!");
            Serial.println(" ");
        }
    }

    if (chanNumber == 16) /* DMA transfer test completed for all channels */
    {
        Serial.println("\r\nDMA INTERRUPT MODE TEST PASSED!!");
        xfOn();
    }
    else
    {
        Serial.println("\r\n DMA INTERRUPT MODE DEMO FAILED!!");
        xfOff();
    }

    /* De-allocate the buffers, which were allocated by the DMA module by
       calling DMA.dmaInit() API */
    DMA.dmaDeInit(&dmaConfig);
}

void loop()
{
}
