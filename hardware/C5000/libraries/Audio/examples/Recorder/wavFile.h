#ifndef _WAVFILE_H_
#define _WAVFILE_H_

void swapBytes(int *buffer, int length);

void updateWavFileHeader(char *waveFileHeader, int single_buffer_size, long recordCounter, long samplingRate, int noOfChannels);

#endif //_WAVFILE_H_
