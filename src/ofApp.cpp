#include "ofApp.h"

ofPoint randomPoint() {
	return ofPoint(ofRandom(0,1024),ofRandom(0,768),ofRandom(-2000,100));
}

//--------------------------------------------------------------
void ofApp::setup(){
	
	glEnable(GL_DEPTH_TEST);
	light.enable();
	model.loadModel("models/Nautilus explorer.3DS");
	model.enableTextures();
	for(int i = 0; i < 10; ++i) {
		path.push_back(randomPoint());
	}
	model.setPosition(path[0].x, path[0].y, path[0].z);
	current_point = 0;
	previous_time = 0;
}

//--------------------------------------------------------------
void ofApp::update(){
	float current_time = ofGetElapsedTimef();
	seek(path[(current_point+1)%10], current_time - previous_time);
	previous_time = current_time;
}

void ofApp::seek(ofPoint target, float time){
	ofVec3f force = (target - model.getPosition());
	velocity += force * time;
	if(velocity.length() > 100) {
		velocity = velocity.normalized() * 100;
	}
	ofVec3f pos = model.getPosition() += velocity * time;
	model.setPosition(pos.x,pos.y,pos.z);
	
	if((target - pos).length() < 50.0f) {
		++current_point %= 10;
	}	
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(50, 50, 50, 0);

	ofPath ofpath;
	ofpath.setFilled(false);
	ofpath.setStrokeWidth(2);
	ofpath.setStrokeColor(ofColor(180,40,40,80));
	ofpath.moveTo(path[current_point]);
	for(int i = 1; i < 12; ++i) {
		ofpath.lineTo(path[(current_point+i)%10]);
	}
	ofpath.draw();
	
	ofSetColor(ofColor(180,40,40));
	ofSphere(path[(current_point+1)%10], 5);
		
	ofSetColor(ofColor(255));
	model.drawFaces();
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
