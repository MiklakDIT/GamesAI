#include "bahn.h"

Bahn::Bahn() {
	notes = nullptr;
	shortNotes = nullptr;
	longNotes = nullptr;
	progress = 0;
}

Bahn::~Bahn() {
	reset();
}

void Bahn::run(FFmpeg* ffmpeg) {
	amplitudes = ffmpeg->getAmplitudes();
	channels = ffmpeg->getChannelsCount();
	fqBins = ffmpeg->getFqBinCount();
	length = ffmpeg->getWindowCount();
	bass = fqBins / 50;
	treble = fqBins / 20;
	progress = 0.5f;
	ofLogNotice("Detecting notes");
	doNotes(ffmpeg);
	progress = 0.67f;
	ofLogNotice("Detecting harmonics");
	doHarmonics();
	progress = 0.75f;
	ofLogNotice("Detecting peaks");
	doPeaks();
	progress = 0.79f;
	ofLogNotice("Detecting slopes");
	doSlopes();
	progress = 0.83f;
	ofLogNotice("Detecting long harmonics");
	doLongHarmonics();
	progress = 0.87f;
	//ofLogNotice("Softening edges");
	//doSoftenDrones();
	progress = 0.91f;

	Note::minFq = minFq;
	Note::maxFq = maxFq;

	ofLogNotice("Aggregating notes");
	doAggregate();
	progress = 0.93f;

	ofLogNotice("Finalising notes");
	doFinalise();
	progress = 0.95f;
}

void Bahn::doNotes(FFmpeg* ffmpeg) {
	float* max = ffmpeg->getMaxAmplitude();
	float* thresholds = new float[3];
	maxAmplitude = MAX(max[2],MAX(max[0],max[1]));
	thresholds[0] = min(0.25f * max[0], 0.1f * maxAmplitude);
	thresholds[1] = min(0.25f * max[1], 0.1f * maxAmplitude);
	thresholds[2] = min(0.25f * max[2], 0.1f * maxAmplitude);
	int max_index = 0;
	maxFq = 0;
	minFq = fqBins;

	notes = new vector<shared_ptr<Note>> *[channels];
	longNotes = new vector<shared_ptr<Note>> *[channels];
	shortNotes = new vector<shared_ptr<Note>> *[channels];
	
	for(int c = 0; c < channels; ++c) {
		notes[c] = new vector<shared_ptr<Note>>[length];
		longNotes[c] = new vector<shared_ptr<Note>>[length];
		shortNotes[c] = new vector<shared_ptr<Note>>[length];
	}
	
	int note_duration, note_begin;
	vector<shared_ptr<Note>> current_note;
	for(int c = 0; c < channels; ++c) {
		for(int f = 0; f < fqBins; ++f) {
			// find the threshold (it can be different for bass, vocal, and treble notes)
			float threshold = thresholds[getMaxIndex(f)];
			note_duration = 0;
			current_note.clear();
			for(int t = 0; t < length; ++t) {
				if(amplitudes[c][t][f] > threshold) {
					// note started or continued
					// its duration needs to be measured before adding it
					note_duration++;
					auto note = make_shared<Note>(c, t, f, amplitudes[c][t][f] / maxAmplitude);
					current_note.push_back(note);
					if(f < minFq) minFq = f;
					else if(f > maxFq) maxFq = f;
				} else if(note_duration > 0) {
					// note ended
					// check if it is long
					if(note_duration > 9) {
						// long note detected
						for(int i = 0; i < note_duration; ++i) {
							if(i == 0) {
								current_note[i]->type = NOTE_TYPE_LONG_START;
								current_note[i]->next = current_note[i+1];
							} else if (i < note_duration - 1) {
								current_note[i]->type = NOTE_TYPE_LONG_MID;
								current_note[i]->next = current_note[i+1];
							} else {
								current_note[i]->type = NOTE_TYPE_LONG_END;
							}
							current_note[i]->timeBegin = current_note[0]->time;
							current_note[i]->length = note_duration;
							notes[c][current_note[i]->time].push_back(current_note[i]);
							longNotes[c][current_note[i]->time].push_back(current_note[i]);
						}
					} else {
						// the note is short 
						// only take the loudest peak and discard the rest
						int max = 0;
						for(int i = 1; i < note_duration; ++i) {
							if(current_note[i]->amplitude > current_note[max]->amplitude) {
								max = i;
							}
						}
						current_note[max]->type = NOTE_TYPE_SHORT;
						notes[c][current_note[max]->time].push_back(current_note[max]);
						shortNotes[c][current_note[max]->time].push_back(current_note[max]);
					}
					note_duration = 0;
					current_note.clear();
				}
			}
		}
	}
	delete [] thresholds;
}

inline int Bahn::getMaxIndex(float f) {
	return f < bass ? 0 : (f < treble ? 1 : 2);
}

void Bahn::doHarmonics() {
	float remainder, ratio;
	int difference;
	for(int c = 0; c < channels; ++c) {
		for(int t = 0; t < length; ++t) {
			for(auto& note : shortNotes[c][t]) {
				for(auto& note2 : shortNotes[c][t]) {
					if(note2->frequency > note->frequency && 
						!note->harmonic && !note2->harmonic) {

						ratio = (2.f * (float)note2->frequency) / (float)note->frequency;
						remainder = fmod(ratio,1);
						difference = note2->frequency - note->frequency;
						if(remainder > 0.9f || remainder < 0.1f || difference <= 3) {
							float amp1 = difference <= 3 ? note->amplitude : note->amplitude * 1.5f;

							if(note2->amplitude > amp1) {
								note->harmonic = true;
							} else {
								note2->harmonic = true;
							}
						}
					}
				}
			}
		}
	}
}

bool compare(shared_ptr<Note> n1, shared_ptr<Note> n2) {
	if(n1->channel == n2->channel)
		return n1->amplitude > n2->amplitude;
	return n1->channel < n2->channel;
}

void Bahn::doPeaks() {
	for(int c = 0; c < channels; ++c) {
		for(int t = 0; t < length; ++t) {
			sort(shortNotes[c][t].begin(), shortNotes[c][t].end(), compare);
			int vocal_count = 0, bass_count = 0, treble_count = 0;
			for(auto& note : shortNotes[c][t]) {
				if(!note->harmonic && note->type == NOTE_TYPE_SHORT) {
					if(note->amplitude < 0.1f) {
						note->harmonic = true;
					} else if (note->frequency <= bass) {
						if(++bass_count > 1) {
							note->harmonic = true;
						}
					} else if (note->frequency <= treble) {
						if(++vocal_count > 2) {
							note->harmonic = true;
						}
					} else if(++treble_count > 1) {
						note->harmonic = true;
					}
				}
			}
		}
	}
}

int Bahn::findNextNoteInSlope(shared_ptr<Note> note) {
	++note->length;

	for(auto note2 : shortNotes[note->channel][note->time + 1]) {
		if(!note2->harmonic && note2->type == NOTE_TYPE_SHORT) {
			float fq_ratio = (float)note2->frequency / (float)note->frequency;
			if(fq_ratio > 0.9f && fq_ratio < 1.1f) {
				note->type = note->type == NOTE_TYPE_SHORT ? NOTE_TYPE_SLOPE_START : NOTE_TYPE_SLOPE_MID;
				note2->type = NOTE_TYPE_SLOPE_END;
				note->next = note2;
				note2->length = note->length;
				note->length = findNextNoteInSlope(note2); 
				return note->length;
			}
		}
	}

	return note->length;
}

void Bahn::undoSlope(shared_ptr<Note> first_note) {
	for(auto note = first_note; note; note = note->next) {
		note->type = NOTE_TYPE_SHORT;
	}
}

void Bahn::doSlopes() {
	for(int c = 0; c < channels; ++c) {
		for(int t = 0; t < length-1; ++t) {
			for(auto& note : shortNotes[c][t]) {
				// short notes can also contain glides, so the type needs to be checked
				if(!note->harmonic && note->type == NOTE_TYPE_SHORT) {
					// if the glide is too short, revert the changes
					if(findNextNoteInSlope(note) < 5) {
						undoSlope(note);
					}
				}
			}
		}
	}
}

// used for smoothing the amplitudes in long notes (so called drones)
// not used any more
void Bahn::doSoftenDrones() {
	for(int c = 0; c < channels; ++c) {
		for(int t = 0; t < length; ++t) {
			for(auto note = notes[c][t].begin(); note != notes[c][t].end(); ++note) {
				if((*note)->type & NOTE_TYPE_START) {
					if((*note)->length >= 5) {
						float * soft_amps = new float[(*note)->length];
						for(int x = 0; x < (*note)->length; ++x) {
							soft_amps[x] = 0.f;
						}
						int i = 0;
						for(auto nod = *note; nod; nod = nod->next) {
							if(i >= 2)
								soft_amps[i-2] += nod->amplitude;
							if(i >= 1)
								soft_amps[i-1] += nod->amplitude;
							if(i < (*note)->length - 2)
								soft_amps[i+2] += nod->amplitude;
							if(i < (*note)->length - 1)
								soft_amps[i+1] += nod->amplitude;
							soft_amps[i] += nod->amplitude;
							++i;
						}
						i = 0;
						for(auto nod = *note; nod; nod = nod->next) {
							if(i == 0 || i == (*note)->length - 1)
								nod->amplitude = soft_amps[i] / 3.f;
							else if(i == 1 || i == (*note)->length - 2)
								nod->amplitude = soft_amps[i] / 4.f;
							else
								nod->amplitude = soft_amps[i] / 5.f;
							++i;
						}
						delete [] soft_amps;
					} else {
						for(auto nod = *note; nod; nod = nod->next) {
							nod->type = NOTE_TYPE_SHORT;
						}
					}
				}
			}
		}
	}
}

void Bahn::doSyncLongHarmonic(shared_ptr<Note> note1, shared_ptr<Note> note2) {
	float length_ratio = (float)(note2->length) / (float)(note2->length);
	if(length_ratio >= 0.8f && length_ratio <= 1.2f) {
		int ampl1 = 0, ampl2 = 0;
		for(auto note = note1; note; note = note->next) {
			ampl1 += note->amplitude;
		}
		for(auto note = note2; note; note = note->next) {
			ampl2 += note->amplitude;
		}
		if(ampl1 > ampl2) {
			note2->setHarmonic(true);
		} else {
			note1->setHarmonic(true);
		}
	}
}

void Bahn::doAsyncLongHarmonic(shared_ptr<Note> starting_note, shared_ptr<Note> lasting_note) {
	float length_ratio = (float)(lasting_note->length + lasting_note->timeBegin - lasting_note->time) / (float)(starting_note->length);
	if(length_ratio >= 0.8f && length_ratio <= 1.2f) {
		starting_note->setHarmonic(true);
	}
}

void Bahn::doLongHarmonics() {
	for(int c = 0; c < channels; ++c) {
		for(int t = 0; t < length; ++t) {
			for(auto note = longNotes[c][t].begin(); note != longNotes[c][t].end(); ++note) {
				for(auto note2 = note + 1; note2 != longNotes[c][t].end(); ++note2) {
					if(!(*note)->harmonic && !(*note2)->harmonic) {
						if((*note2)->type == NOTE_TYPE_LONG_START && (*note)->type == NOTE_TYPE_LONG_START) {
							doSyncLongHarmonic(*note, *note2); 							
						} else if ((*note2)->type == NOTE_TYPE_LONG_START) {
							doAsyncLongHarmonic(*note2, *note);
						} else if ((*note)->type == NOTE_TYPE_LONG_START) {
							doAsyncLongHarmonic(*note, *note2);
						}
					}
				}
			}
		}
	}
}

void Bahn::doAggregate() {
	float bottom = log(minFq+1);
	float top = log(maxFq);
	float width = top - bottom;
	for(int i = 0; i < 6; ++i) {
		colorCount[i] = .0f;
	}
	intensity = new float[length+1];
	beats = new float[length+1];
	for(int t = length - 1; t >= 0; --t) {
		beats[t] = intensity[t] = .0f;
		for(int c = 0; c < channels; ++c) {
			for(auto& note : notes[c][t]) {
				if(!note->harmonic) {
					// aggregate amplitudes for every frequency class
					float log_val = log(note->frequency);
					int fqClass = log_val >= top ? 5 : 6 * (log_val - bottom) / (width);
					colorCount[fqClass] += note->amplitude; 

					// aggregate amplitudes for beat detection
					int multiplier = 1;
					if(note->isType(NOTE_TYPE_SHORT)) {
						// drum beats will be short, hence they should more
						multiplier = 4;
					} else if(note->isType(NOTE_TYPE_START)) {
						multiplier = 2;
					}	

					if(note->frequency < bass) {
						beats[t] += 2.f * multiplier * note->amplitude;
						multiplier *= 1.5f;
					}

					// aggregate amplitudes for intensity measurement
					// the value of 0.0028888f is experimental
					for(int i = min(t + 10, length); i >= t; --i) {
						intensity[i] += multiplier * 0.0028888f;
					}
				}
			}
		}
	}
	
	// clamp intensity
	for(int t = length - 1; t >= 0; --t) {
		if(intensity[t] > 1.f) {
			intensity[t] = 1.f;
		}
	}
	
	// dummy value that avoids array bound checks
	intensity[length] = 0.f;


}

//initialise values for the notes
void Bahn::doFinalise() {
	for(int c = 0; c < channels; ++c) {
		for(int t = 0; t < length; ++t) {
			for(auto& note : notes[c][t]) {
				note->initFrequencyClass();
			}
		}
	}
}

void Bahn::reset() {
	if(notes) {
		for(int i = 0; i < channels; i++) {
			delete [] notes[i];
			delete [] shortNotes[i];
			delete [] longNotes[i];
		}

		delete [] notes;
		delete [] shortNotes;
		delete [] longNotes;
		delete [] intensity;
		delete [] beats;
	}
	progress = 0.0f;
}
