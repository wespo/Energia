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

/*
 * Opens next wave file present under Root Directory, reads the number of
 * channels of the wave file and finally reads the sampling rate of the file to
 * set the sampling rate of the Audio Codec
 */

void WAVClass::emptyBuffer()
{
	for(int i = 0; i < CIRCULAR_BUFFER_SIZE; i++)
	{
		for(int j = 0; j < SINGLE_BUFFER_SIZE; j++)
		{
			waveFileData[i][j] = 0;
		}
	}
}

void WAVClass::init()
{
	emptyBuffer();
	readCircBufferIndex = 0;
	codecCircBufferIndex = 0;
	indivBufferIndex = 0;

	int  retStatus;
    int  index;

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
}


int WAVClass::openWavFile(void)
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
void WAVClass::swapBytes(int *input, int length)
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

int WAVClass::readBuffer()
{
	int bytesRead;
	int status;

	if ((stopped == false) && (bufferSent[readCircBufferIndex] == true))
	{
		bytesRead = fileHandle.read(&waveFileData[readCircBufferIndex][0],SINGLE_BUFFER_SIZE);
		if (0 == bytesRead)
		{
			emptyBuffer();
			return 0;
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
	return bytesRead;
}

void WAVClass::copyWavBuffer(int* leftSamples, int* rightSamples)
{
	int index;
    int index2;
	if (true == bufferRead[codecCircBufferIndex])
	{
		if (CHANNEL_MONO == channelType) // Mono Channel
		{
			// Left channel and right channel data will be same

			// Copying Left Channel Audio sample Data
			copyShortBuf(&waveFileData[codecCircBufferIndex][indivBufferIndex],leftSamples,I2S_DMA_BUF_LEN);

			// Copying Right Channel Audio sample Data*/
			copyShortBuf(leftSamples,rightSamples,I2S_DMA_BUF_LEN);

			indivBufferIndex += I2S_DMA_BUF_LEN;
		}
		else // Stereo Channel
		{
			index2 = indivBufferIndex;

			// Copying Left and right samples alternatively
			for (index = 0; index < I2S_DMA_BUF_LEN; index++)
			{
				leftSamples[index] = waveFileData[codecCircBufferIndex][index2++];
				rightSamples[index] = waveFileData[codecCircBufferIndex][index2++];
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
}