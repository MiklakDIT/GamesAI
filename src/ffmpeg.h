#pragma once

#include <iostream>
#include <fstream>
#include <Windows.h>
#include "ofxFftBasic.h"
#include <cstring>
#include <string>

extern "C"
{
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
//	#include <libavutil/avutil.h>
};

class FFmpeg {
	public:
		~FFmpeg();
		FFmpeg();
		void setup();
		int run(string);
		void reset();
		int getWindowSize();
		bool isStereo();
		string dialog();
		int getWindowCount();
		int getChannelsCount();
		int getFqBinCount();
		vector<float*> * getAmplitudes();
		float* getMaxAmplitude();
	
	private:
		long count; // number of samples
		float** converted_data; // PCM data converted into floats
		float** samples; // window used for performing FFT
		int current_index; // used for buildings windows for FFT
		int windowsize; // size of a window needed by FFT
		int channels; // number of channels
		float* maxAmplitude; // maximum amplitudes for bass, vocal and treble frequency range
		int bass, treble; // thresholds for bass and treble frequencies
		vector<float*> *amplitudes; // fully decoded FFT data

		int error(string message);
		void processFrame(ofxFftBasic*, const AVCodecContext*, const AVFrame*);
		void (FFmpeg::*convertData)(const AVFrame*);
		void convertS16(const AVFrame*);
		void convertS16P(const AVFrame*);
		void convertS32(const AVFrame*);
		void convertS32P(const AVFrame*);
		void doFfts(ofxFftBasic*);
		void doFft(ofxFftBasic*, int);

		/* 
		* Functions below are not finished
		* they were going to be used for caching files
		*/
		void saveCachedFile(char*);
		bool fileExists(const char*);
		bool tryCachedFile(char*);
		string getCacheFileName(char*);
};