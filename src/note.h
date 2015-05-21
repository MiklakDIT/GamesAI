#pragma once

#include <memory>

enum NoteType {
	NOTE_TYPE_SHORT = 1,
	NOTE_TYPE_LONG = 2,
	NOTE_TYPE_SLOPE = 4,
	NOTE_TYPE_START = 8,
	NOTE_TYPE_LONG_START = 10,
	NOTE_TYPE_SLOPE_START = 12,
	NOTE_TYPE_MID = 16,
	NOTE_TYPE_LONG_MID = 18,
	NOTE_TYPE_SLOPE_MID = 20,
	NOTE_TYPE_END = 32,
	NOTE_TYPE_LONG_END = 34,
	NOTE_TYPE_SLOPE_END = 36,
	NOTE_TYPE_ALL = 63
};

class Note {
public:
	Note(int,int,int,float);
	bool isType(NoteType);
	int getFrequencyClass();
	void initFrequencyClass();
	void setHarmonic(bool h);

	int channel, frequency, time, timeBegin, length;
	float amplitude;
	NoteType type;
	bool harmonic, alive;
	std::shared_ptr<Note> next;
	int fqClass;

	static int minFq, maxFq; // minimum and maximum frequency
	static float currentTime;
};