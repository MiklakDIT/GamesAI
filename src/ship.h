#pragma once

#include "ofxAssimpModelLoader.h"
#include "ofTexture.h"
#include "ofxBullet.h"
#include "Projectile.h"
#include "note.h"

#define ARENA_SIZE 5000
#define RANDOM_XYZ ((rand()%50)-25)*ARENA_SIZE/50
class Ship
{
public:
	static vector<shared_ptr<Ship>> ships;
	static void AttachShip(shared_ptr<Ship> ship);
	static void DetachAll();
	void Init(int max_speed, int agility, int attack, int strength);
	virtual void Reset();
	~Ship(void);
	void loadModel(string path, float scale, ofColor& color);
	void draw();
	void drawVec(string label, ofVec3f vec, int& y);
	void drawDebug();
	void move(float time);
	void update(float time, float intensity);
	void standardBehaviour(float timeDelta);
	void returnToArena(float timeDelta);
	void returnToNormal(float timeDelta);
	bool avoidCollision(float timeDelta);
	bool seek(ofPoint target, float time);
	bool sees(ofPoint target, float angle = 90);
	virtual void findTarget();
	void seekRotate(ofVec3f& target, float time);
	shared_ptr<Projectile> shoot(shared_ptr<Note> note, float intensity);
	static bool showTarget;
	ofQuaternion fromTwoVectors(ofVec3f origin, ofVec3f target);
	bool hasTarget, outOfBounds;
	bool isAvoiding;
	shared_ptr<Ship> target_enemy;
	bool hit(int attack);
	
	int max_speed, speed, agility, attack, strength;
	ofColor color;
	int team;
	
	ofxAssimpModelLoader model;
	ofTexture tex;
	ofVec3f velocity, position, look, basicLook, force, target;
	ofQuaternion rot;
	float angular_velocity;

};

