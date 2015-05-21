#pragma once
#include "ofMain.h"

class Projectile
{
public:
	Projectile();
	Projectile(ofVec3f& look, ofVec3f& position, int speed, int attack, int size, ofColor& color);
	void draw();
	int update(float timeDelta);
	ofVec3f look, position;
	int attack, speed, size;
	ofColor color;
	bool alive;
	void setLook(ofVec3f& look);
	void setPosition(ofVec3f& position);
	void setSpeed(int speed);
	void setAttack(int attack);
	void setSize(int size);
	void setColor(ofColor& color);
};

