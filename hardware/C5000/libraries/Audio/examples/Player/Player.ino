/*
 * Wave player demo:
 * Searches for the .wav files on the SD Card and opens them. 
 * Reads data from the wave files present on the SD Card and sends
 * the data to codec, which can be listened on headphone.
 *
 * This program stops after playing all the wave files present
 * in root folder of SD card
 */

#include "SD.h"
#include "Audio.h"
#include "OLED.h"

#define CIRCULAR_BUFFER_SIZE (5u)
#define SINGLE_BUFFER_SIZE   (8 * I2S_DMA_BUF_LEN)

File rootDirHandle;
File fileHandle;

bool bufferRead[CIRCULAR_BUFFER_SIZE];
bool bufferSent[CIRCULAR_BUFFER_SIZE];
bool stopExample = false;

int  channelType;
long samplingRate;

unsigned short audioReadBufferL[I2S_DMA_BUF_LEN];
unsigned short audioReadBufferR[I2S_DMA_BUF_LEN];

/* Circular Buffer to read 8KB block of data at a time from the input wave
   file */
int waveFileData[CIRCULAR_BUFFER_SIZE][SINGLE_BUFFER_SIZE] = {{0}, {0}};
int readCircBufferIndex = 0;
int codecCircBufferIndex = 0;
int indivBufferIndex = 0;

// DMA Interrupt Service Routine
interrupt void dmaIsr(void)
{
    Uint16 ifrValue;

    ifrValue = DMA.readInterruptStatus();
    if ((ifrValue >> DMA_CHAN_ReadR) & 0x01)
    {
        /* If the buffer is read from the input wave file, send it to the Audio
           Out of the Codec */
        if (true == bufferRead[codecCircBufferIndex])
        {
            bufferCopy();
        }
    }

    /* Calling AudioC.isrDma() will copy the buffers to Audio Out of the Codec,
     * initiates next DMA transfers of Audio samples to and from the Codec
     */
    AudioC.isrDma();
}

/*
 * Opens next wave file present under Root Directory, reads the number of
 * channels of the wave file and finally reads the sampling rate of the file to
 * set the sampling rate of the Audio Codec
 */
int openWavFile(void)
{
    char fileName[13];
    int  index;

    while (1)
    {
        // Open Next Wave file present on the SD Card under root
        fileHandle = rootDirHandle.openNextFile();
        if (fileHandle)
        {
            if (!(fileHandle.isDirectory()))
            {
                fileHandle.getName(fileName);

                index = strlen(fileName);

                /* Check whether the File handle which we have obtained is of a
                   Audio wave file */
                if (((fileName[index - 3] == 'w') || (fileName[index - 3] == 'W')) &&
                    ((fileName[index - 2] == 'a') || (fileName[index - 2] == 'A')) &&
                    ((fileName[index - 1] == 'v') || (fileName[index - 1] == 'V')))
                {
                    disp.clear(1);
                    disp.setline(1);
                    disp.print(fileName);

                    Serial.print("\nFile Name: ");
                    Serial.println(fileName);

                    /* Number of channels of the current wave file will be present
                       in the wave header of the respective file, at an index of
                       22 in terms of bytes */
                    fileHandle.seek(22);

                    Serial.print("Number of Channels: ");
                    channelType = fileHandle.read();
                    Serial.print(channelType);
                    if (CHANNEL_MONO == channelType)
                    {
                        Serial.println(" (Mono)");
                    }
                    else
                    {
                        Serial.println(" (Stereo)");
                    }

                    /* Sampling rate present in the Wave file header starts from
                       24th index in terms of bytes */
                    fileHandle.seek(24);
                    samplingRate = (long)fileHandle.read();
                    samplingRate |= ((long)fileHandle.read() << 8);

                    Serial.print("Sampling Rate: ");
                    Serial.print(samplingRate);
                    Serial.println("Hz");

                    fileHandle.seek(44);
                    return (0);
                }
            }
        }
        else
        {
            disp.setline(1);
            disp.clear(1);
            disp.print("Stopped");
            return (1);
        }
    }
}

// Function to swap bytes of an input buffer of specified length
void swapBytes(int *input, int length)
{
    int index;
    int temp;

    for (index = 0; index < length; index++)
    {
        temp = (input[index] & 0xFF) << 8;
        input[index] = (input[index] & 0xFF00) >> 8;

        input[index] |= temp;
    }
}

// Initializes OLED, Audio and SD modules
void setup()
{
    int  retStatus;
    int  index;
    Serial.begin(9600);
    Serial.print("\n Audio Player Demo!\n");

    pinMode(LED0, OUTPUT);

    //Initialize OLED module for file name display	
    disp.oledInit();
    disp.clear();
    disp.setOrientation(1);
    disp.setline(0);
    disp.print("Audio Player");

    for (index = 0; index < CIRCULAR_BUFFER_SIZE; index++)
    {
        bufferSent[index] = true;
        bufferRead[index] = false;
    }

    retStatus = SD.begin(); // initialize SD Card
    if (TRUE == retStatus)
    {
        rootDirHandle = SD.open("/");
        if (rootDirHandle)
        {
            /* Open the 1st wave file that exists under Root directory of the
               SD Card */
            retStatus = openWavFile();
            if (0 != retStatus)
            {
                // Wave file not present, hence stop the recording
                stopExample = true;
            }
        }
        else
        {
            // Error in opening Root directory, hence stop the recording
            stopExample = true;
        }
    }
    else
    {
        // Error in initializing the SD module, hence stop the recording
        stopExample = true;
    }

    retStatus = AudioC.Audio();
    if (retStatus != 0)
    {
        // Error in initializing the Audio module, hence stop the recording
        stopExample = true;
    }
    else
    {
    	// Configure codec sampling rate based on wave file sampling rate
        AudioC.setSamplingRate(samplingRate);
        // Configure interrupts and start audio transfer
        AudioC.attachIntr(dmaIsr);
    }
}

/*
 * This API does a simple copy of Audio samples to the Audio Out buffers of the
 * Codec, from the buffers which hold the audio data read from the wave file
 */
void bufferCopy()
{
    int index;
    int index2;

    if (CHANNEL_MONO == channelType) // Mono Channel
    {
        // Left channel and right channel data will be same

        // Copying Left Channel Audio sample Data
        copyShortBuf(&waveFileData[codecCircBufferIndex][indivBufferIndex],
                     AudioC.audioInLeft[AudioC.activeInBuf],
                     I2S_DMA_BUF_LEN);

        // Copying Right Channel Audio sample Data*/
        copyShortBuf(AudioC.audioInLeft[AudioC.activeInBuf],
                     AudioC.audioInRight[AudioC.activeInBuf],
                     I2S_DMA_BUF_LEN);

        indivBufferIndex += I2S_DMA_BUF_LEN;
    }
    else // Stereo Channel
    {
        index2 = indivBufferIndex;
        
        // Copying Left and right samples alternatively
        for (index = 0; index < I2S_DMA_BUF_LEN; index++)
        {
            AudioC.audioInLeft[AudioC.activeInBuf][index]  = waveFileData[codecCircBufferIndex][index2++];
            AudioC.audioInRight[AudioC.activeInBuf][index] = waveFileData[codecCircBufferIndex][index2++];
        }
        
        indivBufferIndex += (2 * I2S_DMA_BUF_LEN);
    }

    // Update buffer indexes	
    if (SINGLE_BUFFER_SIZE == indivBufferIndex)
    {
        indivBufferIndex = 0;
        bufferSent[codecCircBufferIndex] = true;
        bufferRead[codecCircBufferIndex] = false;

        codecCircBufferIndex++;

        if (CIRCULAR_BUFFER_SIZE == codecCircBufferIndex)
        {
            codecCircBufferIndex = 0;
        }
    }
}

/*
 * This API reads audio data from the wave file and stores it into a temporary
 * buffer. DMA ISR copies the data from this temporary buffer to the Output
 * buffers of the Audio library
 *
 * When the API completes reading the entire data from a wave file, it tries to
 * open the next existing wave file under Root directory. If it finds the next
 * wave file, then it plays that file. When it doesn't find any more wave files,
 * it just stops reading data.
 */
void loop()
{
    int bytesRead;
    int status;

    if ((stopExample == false) && (bufferSent[readCircBufferIndex] == true))
    {
        bytesRead = fileHandle.read(&waveFileData[readCircBufferIndex][0],
                                    SINGLE_BUFFER_SIZE);
        if (0 == bytesRead)
        {
            // Open next wave file, if present on the SD card 
            status = openWavFile();
            if (0 != status)
            {
                /* No more Wave files present under Root directory, hence
                  stop the recording */
                stopExample = true;
            }
            else
            {
                /* Set the sampling rate of the Codec in accordance with the
                 * sampling rate mentioned in the respective wave file header
                 */
                AudioC.setSamplingRate(samplingRate);
                
                // Wait for short time to get the codec clock stabilized
                delaySeconds(1);
            }
            return;
        }

        /* Audio data in the wave file will be in Little Endian format, hence
           swap the bytes to obtain the actual data */
        swapBytes(waveFileData[readCircBufferIndex], SINGLE_BUFFER_SIZE);
        bufferSent[readCircBufferIndex] = false;
        bufferRead[readCircBufferIndex] = true;

        readCircBufferIndex++;

        if (CIRCULAR_BUFFER_SIZE == readCircBufferIndex)
        {
            readCircBufferIndex = 0;
        }
    }
}
