#include "note.h"

float Note::currentTime = 0.f;
int Note::minFq = 0;
int Note::maxFq = 0;

Note::Note(int c, int t, int f, float a) {
	channel = c;
	time = t;
	frequency = f;
	amplitude = a;
	harmonic = false;
	length = 0;
	next = nullptr;
}

void Note::setHarmonic(bool h) {
	harmonic = h;
	if(next) {
		next->setHarmonic(h);
	}
}

bool Note::isType(NoteType t) {
	return type & t;
}

int Note::getFrequencyClass() {
	return fqClass;	
}

void Note::initFrequencyClass() {
	float bottom, top, current, relativeFq, offset;
	// The minimum possible value of frequency variable is 0
	// log(0) is undefined, hence the values are incremented by 1
	bottom = log(minFq+1);
	top = log(maxFq+1);
	current = log(frequency+1);
	relativeFq =  6.f * (current - bottom) / (top - bottom);
	this->fqClass = relativeFq;
}
