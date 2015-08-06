#include "wavFile.h"


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

// Forms wave file header
void updateWavFileHeader(char *waveFileHeader, int single_buffer_size, long recordCounter, long samplingRate, int noOfChannels)
{
    long audioFileSize;
    long byteRate;

    /* When the 'recordCounter' value is calculated, it might be in fractions,
       hence set the file size in terms of 'recordCounter' value */
    audioFileSize = recordCounter * (2*single_buffer_size); // Size of PCM samples 
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

