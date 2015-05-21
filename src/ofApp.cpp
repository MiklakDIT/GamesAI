#include "ofApp.h"

ofPoint randomPoint() {
	return ofPoint(ofRandom(0,1024),ofRandom(0,768),ofRandom(-2000,100));
}

//--------------------------------------------------------------
void ofApp::setup(){
	ffmpeg.setup();
	
	font.loadFont("Arcade.ttf", 60);
	for(int i = 0; i < 2; ++i) {
		for(int j = 0; j < 6; ++j) {
			createShip(j, i);
		}
	}

	scoreA = scoreB = 0;

	ofDisableArbTex();
	//glEnable(GL_DEPTH_TEST);
	ofEnableDepthTest();
	light.enable();

	finishedLoading = true;

	previous_time = 0;
	ofEnableSmoothing();
	current_camera = -1;

	//cam1.move(50,50,-50);
}

void ofApp::createShip(int color, int team) {
	shared_ptr<Ship> ship;
	switch(color) {
	case 0:	ship = make_shared<PurpleShip>(team); break;
	case 1:	ship = make_shared<BlueShip>(team); break;
	case 2:	ship = make_shared<CyanShip>(team); break;
	case 3:	ship = make_shared<GreenShip>(team); break;
	case 4:	ship = make_shared<YellowShip>(team); break;
	case 5:	ship = make_shared<RedShip>(team); break;
	}
	Ship::AttachShip(ship);
	teams[team].push_back(ship);
}
//--------------------------------------------------------------
void ofApp::update(){
	float current_time = ofGetElapsedTimef();

	float intensity;

	updateCamera();
	
	if(sound.getIsPlaying()) {
		int soundPos = sound.getPositionMS();
		window = soundPos / 100.f;
		interpolate = (soundPos % 100) / 100.f;
		intensity = interpol(bahn.intensity[window], bahn.intensity[window+1], interpolate);
		if(window != lastWindow) {
			lastWindow = window;
			shootShoot(intensity);
		}
	} else {
		intensity = 0;
	}
	for(auto ship : Ship::ships) {
		ship->update(current_time - previous_time, intensity);
	}
	collideShips();
	
	for(auto p : projectiles) {
		int teamDestroyed = p->update(current_time - previous_time);
		if(teamDestroyed == 0) {
			scoreB++;
		} else if (teamDestroyed == 1) {
			scoreA++;
		}
	}
	for(auto it = projectiles.begin(); it != projectiles.end(); (*it)->alive ? ++it : it = projectiles.erase(it))
		;
	previous_time = current_time;
}

void ofApp::collideShips() {
	for(auto s1 = Ship::ships.begin(); s1 != Ship::ships.end(); ++s1) {
		for(auto s2 = s1 + 1; s2 != Ship::ships.end(); ++s2) {
			if(((*s1)->position - (*s2)->position).length() < 100) {
				int score = (*s1)->team + (*s2)->team;
				scoreA += score;
				scoreB += 2 - score;
				(*s1)->Reset();
				(*s2)->Reset();
			}
		}
	}
}

void ofApp::updateCamera() {
	if(current_camera >= 0) {
		shared_ptr<Ship> ship = Ship::ships[current_camera];
		ofVec3f pos = ship->position - ship->look * 500;
		cam1.setPosition(pos);
		ofQuaternion rot = ship->fromTwoVectors(ship->look, ofVec3f(0,0,1));
		cam1.setOrientation(rot);
	}
}

void ofApp::shootShoot(float intensity) {
	for(int c = 0; c < 2; ++c) {
		for(shared_ptr<Note> note : bahn.notes[c][window]) {
			if(!note->harmonic) {
				projectiles.insert(teams[note->channel][note->fqClass]->shoot(note, intensity));
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw(){

	ofBackgroundGradient(ofColor(0,0,5), ofColor(5,5,25));
	
	cam1.begin();
	for(auto ship : Ship::ships) {
		ship->draw();
	}
	for(auto p : projectiles) {
		p->draw();
	}
	ofSetColor(255);
	for(auto star : stars) {
		ofSphere(star, 20);
	}
	cam1.end();
	
	ofSetColor(ofColor(255, 0, 0));
	font.drawString("A: " + to_string(scoreA), 10, 10);
	font.drawString("B: " + to_string(scoreB), ofGetWindowWidth() - 60, 10);
	ofDrawBitmapString("A: " + to_string(scoreA), 10, 10, -10);
	ofDrawBitmapString("B: " + to_string(scoreB), ofGetWindowWidth() - 60, 10, -10);
	//ofDrawBitmapString(to_string(current_camera), 10, 10, -10);
	//cam1.end();
}

void ofApp::nextCamera() {
	if(++current_camera == Ship::ships.size()) {
		current_camera = -1;
	}
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch(key) {
	case OF_KEY_F1: openNewFile(); break;
	case OF_KEY_F2: startGame(); break;
	case OF_KEY_F3: Ship::showTarget = !Ship::showTarget; break;
	//case OF_KEY_F4: nextCamera(); break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

void ofApp::openNewFile() {
	// only open a new file, if the previous one has been analysed
	if(finishedLoading) {
		string fname = ffmpeg.dialog();
		if(fname != "") {
			this->filename = fname;
			finishedLoading = false;

			// run analysis in a separate thread
			// otherwise the application will not be responding
			thread bahn_thread([this] { 
				ffmpeg.reset();
				bahn.reset();
				ffmpeg.run(filename);
				bahn.run(&ffmpeg);
				sound.loadSound(filename);
				bahn.progress = 1.f;
				finishedLoading = true;
			});
			bahn_thread.detach();
		}
	}
}

int ofApp::randomCoordinate() {
	return (rand() % 2 ? 1 : -1) * rand();
}

ofVec3f ofApp::randomStar() {
	ofVec3f star = ofVec3f(randomCoordinate(), randomCoordinate(), randomCoordinate());
	star.normalize();
	star *= 5000;
	return star;
}

void ofApp::startGame() {
	if(finishedLoading) {
		for(auto ship : Ship::ships) {
			ship->Reset();
		}
		stars.clear();

		for(int i = 0; i < 100; ++i) {
			stars.push_back(randomStar());
		}
		lastWindow = -1;
		scoreA = scoreB = 0;
		sound.play();
	}
}