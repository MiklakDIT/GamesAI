#pragma once 

#include <vector>
#include "note.h"
#include <iostream>
#include "ffmpeg.h"

class Bahn : public ofThread {
	public:
		Bahn();
		~Bahn();
		void reset();
		void run(FFmpeg*);
		float getAmplitude(int,int,int);

		vector<float*> *amplitudes; // FFT data
		vector<shared_ptr<Note>> **notes; // list of notes
		vector<shared_ptr<Note>> **shortNotes; // list of short notes: for time efficiency 
		vector<shared_ptr<Note>> **longNotes; // list of long notes: for time efficiency 
		int channels; // number of channels
		int fqBins; // size of the frequency domain
		int length; // size of the time domain
		int bass, treble; // thresholds for bass, vocal and treble frequency ranges
		int minFq, maxFq; // the frequencies of the lowest and highest notes
		float colorCount[6]; // the sum of amplitudes for each colour class
		float* intensity; // intensity of the song
		float* beats; // used for beat detection
		float progress; // used to tell the user about the status of the analysis
		float maxAmplitude; // amplitude of the loudest note in the song
	private:
		void doPeaks();
		void doSlopes();
		void undoSlope(shared_ptr<Note>);
		void doNotes(FFmpeg*);
		void doHarmonics();
		void doSoftenDrones();
		void doAggregate();
		void doFinalise();
		void doLongHarmonics();
		void doSyncLongHarmonic(shared_ptr<Note>, shared_ptr<Note>);
		void doAsyncLongHarmonic(shared_ptr<Note>, shared_ptr<Note>);
		int findNextNoteInSlope(shared_ptr<Note>);
		bool compareAmp(Note*, Note*);
		inline int getMaxIndex(float);
};