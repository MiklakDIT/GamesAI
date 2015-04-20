#include "ofApp.h"

ofPoint randomPoint() {
	return ofPoint(ofRandom(0,1024),ofRandom(0,768),ofRandom(-2000,100));
}

//--------------------------------------------------------------
void ofApp::setup(){
	ofDisableArbTex();
	glEnable(GL_DEPTH_TEST);
	light.enable();
	for(int i = 0; i < 10; ++i) {
		ship.path.push_back(randomPoint());
	}
	ship.loadModel("models/Nautilus explorer.3DS");
	ship.current_point = 0;
	previous_time = 0;
}

//--------------------------------------------------------------
void ofApp::update(){
	float current_time = ofGetElapsedTimef();
	ship.update(current_time - previous_time);
	previous_time = current_time;
}



//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(50, 50, 50, 0);

	ofPath ofpath;
	ofpath.setFilled(false);
	ofpath.setStrokeWidth(2);
	ofpath.setStrokeColor(ofColor(180,40,40,80));
	ofpath.moveTo(ship.path[ship.current_point]);
	for(int i = 1; i < 12; ++i) {
		ofpath.lineTo(ship.path[(ship.current_point+i)%10]);
	}
	ofpath.draw();
	
	ofSetColor(ofColor(180,40,40));
	ofSphere(ship.path[(ship.current_point+1)%10], 5);
		
	ofSetColor(ofColor(255));
	ship.draw();
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
