#include "ofxAssimpModelLoader.h"
#include "ofTexture.h"

#pragma once
class Ship
{
public:
	Ship(void);
	~Ship(void);
	void loadModel(string path);
	void draw();
	void drawVec(string label, ofVec3f vec, int& y);
	void move(float time);
	void update(float time);
	void seek(ofPoint target, float time);
	void seekRotate(ofVec3f target, float time);
	ofQuaternion fromTwoVectors(ofVec3f origin, ofVec3f target);
	vector<ofPoint> path;
	int current_point;

	ofxAssimpModelLoader model;
	ofTexture tex;
	ofVec3f velocity, position, look, basicLook, force;
	ofQuaternion rot;
	float angular_velocity;

};

