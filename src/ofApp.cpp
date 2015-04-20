#include "ofApp.h"

ofPoint randomPoint() {
	return ofPoint(ofRandom(0,1024),ofRandom(0,768),ofRandom(-2000,100));
}

//--------------------------------------------------------------
void ofApp::setup(){
	ofDisableArbTex();
	glEnable(GL_DEPTH_TEST);
	light.enable();
	shared_ptr<Ship> ship = make_shared<Ship>("models/Nautilus/Nautilus explorer.obj", "models/Nautilus/texture.jpg");
	ships.push_back(ship);
	/*ship = make_shared<Ship>("models/Tagawa/Tagawa.obj");
	ships.push_back(ship);
	ship = make_shared<Ship>("models/Seed Ship/Seed ship.obj");
	ships.push_back(ship);
	ship = make_shared<Ship>("models/Mak/Mak fighter.obj");
	ships.push_back(ship);
	ship = make_shared<Ship>("models/MS/MS Heavy Fighter.3DS");
	ships.push_back(ship);
	ship = make_shared<Ship>("models/MS/MS Bomber.3DS");
	ships.push_back(ship);*/
	previous_time = 0;
}

//--------------------------------------------------------------
void ofApp::update(){
	float current_time = ofGetElapsedTimef();
	for(auto ship : ships) {
		ship->update(current_time - previous_time);
	}
	previous_time = current_time;
}



//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(50, 50, 50, 0);
	light.draw();
	/*ofPath ofpath;
	ofpath.setFilled(false);
	ofpath.setStrokeWidth(2);
	ofpath.setStrokeColor(ofColor(180,40,40,80));
	ofpath.moveTo(ships[0]->path[ships[0]->current_point]);
	for(int i = 1; i < 12; ++i) {
		ofpath.lineTo(ships[0]->path[(ships[0]->current_point+i)%10]);
	}
	ofpath.draw();
	
	ofSetColor(ofColor(180,40,40));
	ofSphere(ships[0]->path[(ships[0]->current_point+1)%10], 5);
		*/
	ofSetColor(ofColor(255));
	for(auto ship : ships) {
		ship->draw();
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
