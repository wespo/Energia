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

/** @file WavFile.h
 *
 *  @brief Wave File API header file.
 *
 */

#ifndef DSP_WAV_FILE_H
#define DSP_WAV_FILE_H

#include "Dsplib.h"
#include "SD.h"
#include "Audio.h"
#include "core.h"

/** Buffer length */
#define I2S_DMA_BUF_LEN        (512)  // Must be power of 2
#define CHANNEL_MONO   (1)  /**< Macro to indicate the Channel type as Mono   */
#define CHANNEL_STEREO (2)  /**< Macro to indicate the Channel type as Stereo */

#define CIRCULAR_BUFFER_SIZE (5u)
#define SINGLE_BUFFER_SIZE   (8 * I2S_DMA_BUF_LEN)
#define MAX_FILENAME_LEN 48

//Defines for record
/* Record duration in seconds.
   Modify this macro to change the duration of record */
#define NO_OF_WRITE_BUFFERS 5
//#define ENABLE_STEREO_RECORD


class WAVRead {
   public:
    void init(unsigned int bufferlen = SINGLE_BUFFER_SIZE);
    int openWavFile(); //opens next wave file
	int openWavFile(char* fileNam); //opens next wave file
	int readBuffer();
	void copyWavBuffer(int* leftSamples, int* rightSamples, int length = I2S_DMA_BUF_LEN);
	long samplingRate;
	int  channelType;
	bool stopped;
	char fileName[MAX_FILENAME_LEN];
   private:
   	void swapBytes(int *input, int length);
   	void emptyBuffer();
	File rootDirHandle;
	File fileHandle;

	unsigned int bufferSize;

	bool isInitialized;
	bool bufferRead[CIRCULAR_BUFFER_SIZE];
	bool bufferSent[CIRCULAR_BUFFER_SIZE];
	

	

	/* Circular Buffer to read 8KB block of data at a time from the input wave
	   file */
	int readCircBufferIndex;
	int codecCircBufferIndex;
	int indivBufferIndex;
};

class WAVWrite {
	public:
		WAVWrite(void);
		void updateWavFileHeader();
		void captureAudio(int* audioStream, int audioBufSize);
		void captureAudio(int* audioStreamL, int* audioStreamR, int audioBufSize);
		bool storeAudio();
		void begin(long sr, int channels, int duration);
	private:
		void swapBytes(int *buffer, int length);
		// Variable to hold Record file size in terms of KiloBytes
		long recFileSize;

		/* Counter to indicate the number of times to call File.write(), to write the
		 * audio data to the recorded file.
		 * File.write() will write (2*SINGLE_BUFFER_SIZE) bytes of data at once, 
		 * hence the value (recordCounter * (2*SINGLE_BUFFER_SIZE)) will give the 
		 * recorded file size.
		 */
		long recordCounter;
		long sampleRate;
		int numChannels;
		File fileHandle;
		int bufferSize;
		int writeBuffers;
		// Variable to switch between the data buffers of the Audio library
		unsigned short audioLibWriteBufIndex;

		volatile bool stopRecording;

		// Variables to store different buffer indexes used by the program
		int codecIndex;
		int writeBufIndex;
		int writeToFileIndex;

		volatile long fileCounter;

		char waveFileHeader[44];
		volatile bool buffAvailable[2*NO_OF_WRITE_BUFFERS];
};


#endif   
/* DSP_WAV_FILE_H */
