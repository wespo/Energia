/*
 * Wave recorder demo:
 *  Stores the audio samples received from audio module to a file
 *  on SD card in wave format.
 *
 * Default configurations:
 *  Sampling rate - 48KHz
 *  Channel Type  - Mono
 *  Duration      - 60 secs
 *
 * Note: Stereo mode configurations can be enabled by defining macro
 * 'ENABLE_STEREO_RECORD'
 *
 * Changing the default configurations:
 *  Sampling rate - Change the value of 'samplingRate' which
 *                  can be one of the macros SAMPLING_RATE_XX_KHZ
 *                  defined in Audio.h
 *  Channel Type  - Change the value of 'channelType' to CHANNEL_MONO
 *                  for mono recording
 *  Duration      - Change the value of macro 'RECORD_DURATION' which
 *                  defines the duration of record in secs
 *
 * CAUTION: DO NOT power off the DSP shield when record is in progress.
 * Recorded file will not be saved unless program ends smoothly.
 *
 * SD Card performance Requirements for proper record
 * ---------------------------------------------------
 * SD card used for record should be of class 4 or higher for proper 
 * recording in stereo mode. Recording in stereo mode can be done 
 * without data loss up to 32KHz sampling frequency with class 4 SD card.
 * It is not recommended to use sampling frequency more than 32KHz 
 * for stereo mode which causes data loss during record.
 *
 */

#include "SD.h"
#include "Audio.h"
#include "OLED.h"

/* Record duration in seconds.
   Modify this macro to change the duration of record */
#define RECORD_DURATION (60)

//#define ENABLE_STEREO_RECORD


// Variable to hold Record file size in terms of KiloBytes
long recFileSize = 0;

/* Counter to indicate the number of times to call File.write(), to write the
 * audio data to the recorded file.
 * File.write() will write (2*SINGLE_BUFFER_SIZE) bytes of data at once, 
 * hence the value (recordCounter * (2*SINGLE_BUFFER_SIZE)) will give the 
 * recorded file size.
 */
long recordCounter = 0;

File fileHandle;

// Variable to switch between the data buffers of the Audio library
unsigned short audioLibWriteBufIndex = 0;

volatile bool stopRecording = false;

// Data buffers to transfer the audio data to file write buffers
#pragma DATA_ALIGN(32)
int writeToFileL[I2S_DMA_BUF_LEN];
#pragma DATA_ALIGN(32)
int writeToFileR[I2S_DMA_BUF_LEN];

// Variables to store different buffer indexes used by the program
int codecIndex = 0;
int writeBufIndex = 0;
int writeToFileIndex = 0;

volatile long fileCounter = 0;

char waveFileHeader[44] = {0};

#ifdef ENABLE_STEREO_RECORD
/* Record channel type.*/
int channelType = CHANNEL_STEREO;
long samplingRate = SAMPLING_RATE_32_KHZ;

/* Size of the Block of data. File.write() writes 'SINGLE_BUFFER_SIZE'
   number of words at a time to the recorded file */
#define SINGLE_BUFFER_SIZE  (2048)

// Circular Buffer Size
#define NO_OF_WRITE_BUFFERS (10)

#else
long samplingRate = SAMPLING_RATE_48_KHZ;
int channelType = CHANNEL_MONO;

/* Size of the Block of data. File.write() writes 'SINGLE_BUFFER_SIZE'
   number of words at a time to the recorded file */
#define SINGLE_BUFFER_SIZE  (4096)

// Circular Buffer Size
#define NO_OF_WRITE_BUFFERS (5)

#endif

// Buffers to store the data to be written to file
#pragma DATA_ALIGN(32)
int writeBuf[NO_OF_WRITE_BUFFERS][SINGLE_BUFFER_SIZE] = {0};
volatile bool buffAvailable[NO_OF_WRITE_BUFFERS] = {false};

// Forms wave file header
void updateWavFileHeader(long samplingRate, int noOfChannels)
{
    long audioFileSize;
    long byteRate;

    /* When the 'recordCounter' value is calculated, it might be in fractions,
       hence set the file size in terms of 'recordCounter' value */
    audioFileSize = recordCounter * (2*SINGLE_BUFFER_SIZE); // Size of PCM samples 
    audioFileSize += 44;                      // Size of WAV header

    // Update Header with the Chunk Id: "RIFF", in Big Endian Mode
    waveFileHeader[0] = 'R';
    waveFileHeader[1] = 'I';
    waveFileHeader[2] = 'F';
    waveFileHeader[3] = 'F';

    /* Update Header with the Chunk Size, this size is equal to total file size
       minus 8 bytes. This is the size of the rest of the chunk following this
       number. It should be written in Little Endian mode */
    waveFileHeader[4] = (audioFileSize - 8) & 0xFF;
    waveFileHeader[5] = (audioFileSize >> 8) & 0xFF;
    waveFileHeader[6] = (audioFileSize >> 16) & 0xFF;
    waveFileHeader[7] = (audioFileSize >> 24) & 0xFF;

    // Update Header with the File format: "WAVE", in Big Endian Mode
    waveFileHeader[8]  = 'W';
    waveFileHeader[9]  = 'A';
    waveFileHeader[10] = 'V';
    waveFileHeader[11] = 'E';

    // Update Header with the Sub Chunk Id: "fmt ", in Big Endian Mode
    waveFileHeader[12] = 'f';
    waveFileHeader[13] = 'm';
    waveFileHeader[14] = 't';
    waveFileHeader[15] = ' ';

    /* Update Header with the Sub Chunk1 Size, this size is 16 for PCM data.
       It should be written in Little Endian mode */
    waveFileHeader[16] = 16;
    waveFileHeader[17] = 0;
    waveFileHeader[18] = 0;
    waveFileHeader[19] = 0;

    /* Update Header with the Audio Format. For PCM format, its value should
       be 1. It should be written in Little Endian mode */
    waveFileHeader[20] = 1;
    waveFileHeader[21] = 0;

    /* Update Header with the Number of channels present in the Audio file.
       Mono = 1, Stereo = 2, it should be written in Little Endian mode */
    waveFileHeader[22] = noOfChannels;
    waveFileHeader[23] = 0;


    /* Update Header with the Sampling rate of the Audio file. It should be
       written in Little Endian mode */
    waveFileHeader[24] = (samplingRate & 0xFF);
    waveFileHeader[25] = (samplingRate >> 8) & 0xFF;
    waveFileHeader[26] = (samplingRate >> 16) & 0xFF;
    waveFileHeader[27] = (samplingRate >> 24) & 0xFF;

    /* Update Header with the Byte Rate of the Audio File. Its value is equal
       to, ByteRate = SampleRate * NumChannels * BitsPerSample / 8.
       It should be written in Little Endian mode */
    byteRate = samplingRate * noOfChannels * 16 / 8;
    waveFileHeader[28] = (byteRate & 0xFF);
    waveFileHeader[29] = (byteRate >> 8) & 0xFF;
    waveFileHeader[30] = (byteRate >> 16) & 0xFF;
    waveFileHeader[31] = (byteRate >> 24) & 0xFF;

    /* Update Header with the Block Align. Its value is,
       BlockAlign = NumChannels * BitsPerSample / 8. In Little Endian Mode */
    waveFileHeader[32] = noOfChannels * 16 / 8;
    waveFileHeader[33] = 0;

    /* Update Header with the Bits Per Sample, 8 bits = 8, 16 bits = 16.
       In Little Endian Mode */
    waveFileHeader[34] = 16;
    waveFileHeader[35] = 0;

    // Update Header with Sub Chunk2 ID: "data". Its in Big Endian Mode
    waveFileHeader[36] = 'd';
    waveFileHeader[37] = 'a';
    waveFileHeader[38] = 't';
    waveFileHeader[39] = 'a';

    /* Update Header with the Sub Chunk2 Size, this size is equal to total
       file size minus 24 bytes. Its in Little Endian Mode */
    waveFileHeader[40] = (audioFileSize - 24) & 0xFF;  /* LSB of Sub Chunk Size */
    waveFileHeader[41] = ((audioFileSize - 24) >> 8) & 0xFF;
    waveFileHeader[42] = ((audioFileSize - 24) >> 16) & 0xFF;
    waveFileHeader[43] = ((audioFileSize - 24) >> 24) & 0xFF; /* MSB of Sub Chunk Size */
}

/* Copies the audio Rx data into Tx buffers and copies the audio data
   to file write buffers */
void processAudio()
{

    if ((false == stopRecording) && (buffAvailable[codecIndex] == false))
    {
        if (CHANNEL_STEREO == channelType) // Recording in Stereo mode
        {
            for (int index = 0; index < I2S_DMA_BUF_LEN; index++)
            {
                writeBuf[codecIndex][writeBufIndex++] = AudioC.inputLeft[index];
                writeBuf[codecIndex][writeBufIndex++] = AudioC.inputRight[index];
            }

            /* Audio data of the Wave file should be in little endian form, hence
               swap the bytes before writing it to the file */
            swapBytes(&writeBuf[codecIndex][writeBufIndex - (2 * I2S_DMA_BUF_LEN)],(2 * I2S_DMA_BUF_LEN));
        }
        else /* Recording in Mono mode */
        {
            for (int index = 0; index < I2S_DMA_BUF_LEN; index++)
            {
                writeBuf[codecIndex][writeBufIndex++] = AudioC.inputLeft[index];
            }

            /* Audio data of the Wave file should be in little endian form, hence
               swap the bytes before writing it to the file */
            swapBytes(&writeBuf[codecIndex][writeBufIndex - I2S_DMA_BUF_LEN],I2S_DMA_BUF_LEN);
        }

        if (SINGLE_BUFFER_SIZE == writeBufIndex)
        {
            writeBufIndex = 0;
            buffAvailable[codecIndex] = true;

            if ((NO_OF_WRITE_BUFFERS - 1) == codecIndex)
            {
                codecIndex = 0;
            }
            else
            {
                codecIndex++;
            }
        }
    }
}

void setup()
{
    Bool retStatus;
    int  status;

    pinMode(LED0, OUTPUT);

    //Initialize OLED module for file name display
    disp.oledInit();
    disp.clear();
    disp.setOrientation(1);
    disp.setline(0);
    disp.print("Wave Recorder");
    disp.setline(1);
    disp.print("Running");

    retStatus = SD.begin();
    if (TRUE == retStatus)
    {
        fileHandle = SD.open("Record.wav", FILE_WRITE);
        if (fileHandle)
        {
            // 'recFileSize' will hold the file size in terms of bytes
            recFileSize  = RECORD_DURATION;
            recFileSize *= samplingRate;
            recFileSize *= 2; // File Size in bytes

            if (CHANNEL_STEREO == channelType)
            {
                // Double the file size in case of stereo channel
                recFileSize *= 2;
            }

            /* File.write() will write (2*SINGLE_BUFFER_SIZE) bytes of data at once, 
             * hence the value (recordCounter * (2*SINGLE_BUFFER_SIZE)) will give the 
             * recorded file size.
             */
            recordCounter = recFileSize / (2*SINGLE_BUFFER_SIZE);

            /* recordCounter value might contain fractional part, which will not
             * be taken into account because of conversion from float to long.
             * In such case, increase the recordCounter value by 1 so that
             * recorded file duration will not be less than RECORD_DURATION
             */
            if (recFileSize % recordCounter)
            {
                recordCounter++;
            }

            updateWavFileHeader(samplingRate, channelType);
            fileHandle.write(waveFileHeader, 44);
            fileHandle.flush();
        }
        else
        {
            stopRecording = true;
        }
    }
    else
    {
        stopRecording = true;
    }

    status = AudioC.Audio(TRUE);
    if (status != 0)
    {
        stopRecording = true;
    }

    AudioC.setSamplingRate(samplingRate);
}

void swapBytes(int *buffer, int length)
{
    int index;
    int temp;

    for (index = 0; index < length; index++)
    {
        temp = (buffer[index] & 0xFF) << 8;
        buffer[index] = (buffer[index] & 0xFF00) >> 8;

        buffer[index] |= temp;
    }
}

void loop()
{
    while (false == stopRecording)
    {
        if (true == buffAvailable[writeToFileIndex])
        {
            // Data is ready to be written to SD card
            fileHandle.write(&writeBuf[writeToFileIndex][0], SINGLE_BUFFER_SIZE);

            fileCounter++;
            buffAvailable[writeToFileIndex] = false;

            if ((NO_OF_WRITE_BUFFERS - 1) == writeToFileIndex)
            {
                writeToFileIndex = 0;
            }
            else
            {
                writeToFileIndex++;
            }
        }

        if (fileCounter == recordCounter)
        {
            stopRecording = true;

            fileHandle.close();
            digitalWrite(LED0, 0);

	    disp.setline(1);
	    disp.clear(1);
	    disp.print("Stopped");
        }
    }
}
