#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "ofTexture.h"
#include "ofLight.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void seek(ofPoint, float);

		ofxAssimpModelLoader model;
		ofTexture tex;
		ofLight light;
		vector<ofPoint> path;
		int current_point;
		ofVec3f velocity;
		double previous_time;

};
