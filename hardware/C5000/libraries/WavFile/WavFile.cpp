//////////////////////////////////////////////////////////////////////////////
// *
// * Copyright (C) 2012 Texas Instruments Incorporated - http://www.ti.com/
// * Copyright (C) 2011 Spectrum Digital, Incorporated
// *
// *
// *  Redistribution and use in source and binary forms, with or without
// *  modification, are permitted provided that the following conditions
// *  are met:
// *
// *    Redistributions of source code must retain the above copyright
// *    notice, this list of conditions and the following disclaimer.
// *
// *    Redistributions in binary form must reproduce the above copyright
// *    notice, this list of conditions and the following disclaimer in the
// *    documentation and/or other materials provided with the
// *    distribution.
// *
// *    Neither the name of Texas Instruments Incorporated nor the names of
// *    its contributors may be used to endorse or promote products derived
// *    from this software without specific prior written permission.
// *
// *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// *
//////////////////////////////////////////////////////////////////////////////

/** @file WavFile.cpp
 *
 *  @brief Wave File API implementation
 *
 */

#include "WavFile.h"

#pragma DATA_ALIGN(32)
int waveFileData[CIRCULAR_BUFFER_SIZE][SINGLE_BUFFER_SIZE] = {0};
/*
 * Opens next wave file present under Root Directory, reads the number of
 * channels of the wave file and finally reads the sampling rate of the file to
 * set the sampling rate of the Audio Codec
 */

void WAVRead::emptyBuffer()
{
	for(int i = 0; i < CIRCULAR_BUFFER_SIZE; i++)
	{
		for(int j = 0; j < bufferSize; j++)
		{
			waveFileData[i][j] = 0;
		}
	}
}

void WAVRead::init(unsigned int bufferLen)
{
	emptyBuffer();
	readCircBufferIndex = 0;
	codecCircBufferIndex = 0;
	indivBufferIndex = 0;

	int  retStatus;
    int  index;

    bufferSize = bufferLen;
    if(bufferSize > SINGLE_BUFFER_SIZE)
    {
        bufferSize = SINGLE_BUFFER_SIZE;
    }

    // waveFileData = new int*[CIRCULAR_BUFFER_SIZE];

    // for(int i = 0; i < CIRCULAR_BUFFER_SIZE; i++)
    // {
    //     waveFileData[i] = new int[bufferSize]();
    // }


    for(int i = 0; i < MAX_FILENAME_LEN; i++)
    {
    	fileName[i] = 0;
    }
    stopped = false;

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
                stopped = true;
            }
        }
        else
        {
            // Error in opening Root directory, hence stop the recording
            stopped = true;
        }
    }
    else
    {
        // Error in initializing the SD module, hence stop the recording
        stopped = true;
    }
    isInitialized = true;
}


int WAVRead::openWavFile(void)
{
    int  index;
    char localName[MAX_FILENAME_LEN];
    while (1)
    {
        // Open Next Wave file present on the SD Card under root
        fileHandle = rootDirHandle.openNextFile();
        if (fileHandle)
        {
            if (!(fileHandle.isDirectory()))
            {
                fileHandle.getName(localName);

                index = strlen(localName);

                /* Check whether the File handle which we have obtained is of a
                   Audio wave file */
                if (((localName[index - 3] == 'w') || (localName[index - 3] == 'W')) &&
                    ((localName[index - 2] == 'a') || (localName[index - 2] == 'A')) &&
                    ((localName[index - 1] == 'v') || (localName[index - 1] == 'V')))
                {
                    // disp.clear(1);
                    // disp.setline(1);
                    // disp.print(localName);
                    delay(50);

                    // Serial.print("\nFile Name: ");
                    // Serial.println(localName);

                    /* Number of channels of the current wave file will be present
                       in the wave header of the respective file, at an index of
                       22 in terms of bytes */
                    fileHandle.seek(22);

                    //Serial.print("Number of Channels: ");
                    channelType = fileHandle.read();
                    // Serial.print(channelType);
                    // if (CHANNEL_MONO == channelType)
                    // {
                    //     Serial.println(" (Mono)");
                    // }
                    // else
                    // {
                    //     Serial.println(" (Stereo)");
                    // }

                    /* Sampling rate present in the Wave file header starts from
                       24th index in terms of bytes */
                    fileHandle.seek(24);
                    samplingRate = (long)fileHandle.read();
                    samplingRate |= ((long)fileHandle.read() << 8);

                    // Serial.print("Sampling Rate: ");
                    // Serial.print(samplingRate);
                    // Serial.println("Hz");
                    strcpy(fileName,localName);
                    fileHandle.seek(44);
                    return (0);
                }
            }
        }
        else
        {
            // disp.setline(1);
            // disp.clear(1);
            // disp.print("Stopped");
            stopped = true;
            emptyBuffer();
            return (1);
        }
    }
}

// Function to swap bytes of an input buffer of specified length
void WAVRead::swapBytes(int *input, int length)
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

int WAVRead::readBuffer()
{
    if(!isInitialized)
    {
        return 0;
    }
	int bytesRead;
	int status;

	if ((stopped == false) && (bufferSent[readCircBufferIndex] == true))
	{
		bytesRead = fileHandle.read(&waveFileData[readCircBufferIndex][0],bufferSize);
		if (0 == bytesRead)
		{
			emptyBuffer();
			return 0;
		}

		/* Audio data in the wave file will be in Little Endian format, hence
		   swap the bytes to obtain the actual data */
		swapBytes(waveFileData[readCircBufferIndex], bufferSize);
		bufferSent[readCircBufferIndex] = false;
		bufferRead[readCircBufferIndex] = true;

		readCircBufferIndex++;

		if (CIRCULAR_BUFFER_SIZE == readCircBufferIndex)
		{
			readCircBufferIndex = 0;
		}
	}
	return bytesRead;
}

void WAVRead::copyWavBuffer(int* leftSamples, int* rightSamples, int length)
{
    if(!isInitialized)
    {
        return;
    }

	int index;
    int index2;
	if (true == bufferRead[codecCircBufferIndex])
	{
		if (CHANNEL_MONO == channelType) // Mono Channel
		{
			// Left channel and right channel data will be same

			// Copying Left Channel Audio sample Data
			copyShortBuf(&waveFileData[codecCircBufferIndex][indivBufferIndex],leftSamples,length);

			// Copying Right Channel Audio sample Data*/
			copyShortBuf(leftSamples,rightSamples,length);

			indivBufferIndex += length;
		}
		else // Stereo Channel
		{
			index2 = indivBufferIndex;

			// Copying Left and right samples alternatively
			for (index = 0; index < length; index++)
			{
				leftSamples[index] = waveFileData[codecCircBufferIndex][index2++];
				rightSamples[index] = waveFileData[codecCircBufferIndex][index2++];
			}

			indivBufferIndex += (2 * length);
		}

			// Update buffer indexes	
		if (bufferSize == indivBufferIndex)
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
}

WAVWrite::WAVWrite(void)
{
    recFileSize = 0;
    recordCounter = 0;
    audioLibWriteBufIndex = 0;
    stopRecording = true;
    codecIndex = 0;
    writeBufIndex = 0;
    writeToFileIndex = 0;

    fileCounter = 0;

    for(int i = 0; i < 44; i++)
    {
        waveFileHeader[i] = 0;
    }
    for(int i = 0; i < 2*NO_OF_WRITE_BUFFERS; i++)
    {
        buffAvailable[i] = false;
    }
}

void WAVWrite::swapBytes(int *buffer, int length)
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
void WAVWrite::updateWavFileHeader()
{
    long audioFileSize;
    long byteRate;

    /* When the 'recordCounter' value is calculated, it might be in fractions,
       hence set the file size in terms of 'recordCounter' value */
    audioFileSize = recordCounter * (2*bufferSize); // Size of PCM samples 
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
    waveFileHeader[22] = numChannels;
    waveFileHeader[23] = 0;


    /* Update Header with the Sampling rate of the Audio file. It should be
       written in Little Endian mode */
    waveFileHeader[24] = (sampleRate & 0xFF);
    waveFileHeader[25] = (sampleRate >> 8) & 0xFF;
    waveFileHeader[26] = (sampleRate >> 16) & 0xFF;
    waveFileHeader[27] = (sampleRate >> 24) & 0xFF;

    /* Update Header with the Byte Rate of the Audio File. Its value is equal
       to, ByteRate = SampleRate * NumChannels * BitsPerSample / 8.
       It should be written in Little Endian mode */
    byteRate = sampleRate * numChannels * 16 / 8;
    waveFileHeader[28] = (byteRate & 0xFF);
    waveFileHeader[29] = (byteRate >> 8) & 0xFF;
    waveFileHeader[30] = (byteRate >> 16) & 0xFF;
    waveFileHeader[31] = (byteRate >> 24) & 0xFF;

    /* Update Header with the Block Align. Its value is,
       BlockAlign = NumChannels * BitsPerSample / 8. In Little Endian Mode */
    waveFileHeader[32] = numChannels * 16 / 8;
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

void WAVWrite::captureAudio(int* audioStream, int audioBufSize)
{
    if ((false == stopRecording) && (buffAvailable[codecIndex] == false))
    {
        for (int index = 0; index < audioBufSize; index++)
        {
            waveFileData[codecIndex][writeBufIndex++] = audioStream[index];
        }

        /* Audio data of the Wave file should be in little endian form, hence
           swap the bytes before writing it to the file */
        swapBytes(&waveFileData[codecIndex][writeBufIndex - audioBufSize],audioBufSize);

        if (bufferSize == writeBufIndex)
        {
            writeBufIndex = 0;
            buffAvailable[codecIndex] = true;

            if ((writeBuffers - 1) == codecIndex)
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

void WAVWrite::captureAudio(int* audioStreamL, int* audioStreamR, int audioBufSize)
{
    if ((false == stopRecording) && (buffAvailable[codecIndex] == false))
    {
        for (int index = 0; index < audioBufSize; index++)
        {
            waveFileData[0][codecIndex*bufferSize + writeBufIndex++] = audioStreamL[index];
            waveFileData[0][codecIndex*bufferSize + writeBufIndex++] = audioStreamR[index];
        }

        /* Audio data of the Wave file should be in little endian form, hence
           swap the bytes before writing it to the file */
        swapBytes(&waveFileData[codecIndex][writeBufIndex - (2 * audioBufSize)],(2 * audioBufSize));

        if (bufferSize == writeBufIndex)
        {
            writeBufIndex = 0;
            buffAvailable[codecIndex] = true;

            if ((writeBuffers - 1) == codecIndex)
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

void WAVWrite::begin(long sr, int channels, int duration)
{
    
    bool retStatus = SD.begin();
    int  status;
    stopRecording = true;
    if (TRUE == retStatus)
    {
        fileHandle = SD.open("Record.wav", FILE_WRITE);
        if (fileHandle)
        {
            // 'recFileSize' will hold the file size in terms of bytes
            sampleRate = sr;
            recFileSize  = duration;
            recFileSize *= sr;
            recFileSize *= 2; // File Size in bytes
            bufferSize = (3-channels) * SINGLE_BUFFER_SIZE/2;
            writeBuffers = channels * NO_OF_WRITE_BUFFERS;
            numChannels = channels;
            if (CHANNEL_STEREO == channels)
            {
                // Double the file size in case of stereo channel
                recFileSize *= 2;
                /* File.write() will write (2*SINGLE_BUFFER_SIZE) bytes of data at once, 
                 * hence the value (recordCounter * (2*SINGLE_BUFFER_SIZE)) will give the 
                 * recorded file size.
                 */
            }
            
            recordCounter = recFileSize / (2*bufferSize);
            Serial.println(recordCounter);
            /* recordCounter value might contain fractional part, which will not
             * be taken into account because of conversion from float to long.
             * In such case, increase the recordCounter value by 1 so that
             * recorded file duration will not be less than RECORD_DURATION
             */
            if (recFileSize % recordCounter)
            {
                recordCounter++;
            }

            updateWavFileHeader();
            fileHandle.write(waveFileHeader, 44);
            fileHandle.flush();
            stopRecording = false;
        }
        else
        {
            stopRecording = true;
            fileHandle.close();
            xfOn();
        }
    }
    else
    {
        stopRecording = true;
        fileHandle.close();
            xfOn();
    }

    status = AudioC.Audio(TRUE);
    if (status != 0)
    {
        stopRecording = true;
        fileHandle.close();
        xfOn();
    }

    AudioC.setSamplingRate(sampleRate);
}
volatile bool xf = false;
bool WAVWrite::storeAudio()
{
    if(false == stopRecording)
    {
        // if(xf)
        // {
        //     xfOn();
        //     xf = false;
        // }
        // else
        // {
        //     xfOff();
        //     xf = true;
        // }
        if (true == buffAvailable[writeToFileIndex])
        {
            // Data is ready to be written to SD card
            fileHandle.write(&waveFileData[writeToFileIndex][0], bufferSize);
            fileCounter++;
            buffAvailable[writeToFileIndex] = false;

            if ((writeBuffers - 1) == writeToFileIndex)
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
            return FALSE;
        }
        return TRUE;
    }
}