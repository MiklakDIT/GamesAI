#include "ship.h"


Ship::Ship(string path, string texpath)
{
	position = ofVec3f(0,0,0);
	basicLook = ofVec3f(0,0,1);
	look = basicLook;
	current_point = 0;
	loadModel(path);
	target = ofVec3f(rand() % 800, rand() % 800, -rand() % 800);

}


Ship::~Ship(void)
{
}

void Ship::loadModel(string path) {
	model.enableTextures();
	model.loadModel(path);
	model.calculateDimensions();
}

void Ship::drawVec(string label, ofVec3f vec, int& y) {
	y += 15;
	string s = label + '(' + to_string(vec.x) + ',' + to_string(vec.y) + ',' + to_string(vec.z) + ')';
	ofDrawBitmapString(s,5,y,0);
}
ofVec3f tgl;

void Ship::draw() {	
	tex.bind();
	model.drawFaces();
	tex.unbind();
	int y = 0;
	ofSetColor(ofColor(180,40,40));
	drawVec("look", look, y);
	drawVec("target", tgl, y);
	drawVec("velocity", velocity, y);
	drawVec("force", force, y);

}

void Ship::update(float timeDelta) {
	force *= 0;
	if(seek(target, timeDelta)) {
		target = ofVec3f(rand() % 800, rand() % 800, -rand() % 800);
	}
	move(timeDelta);
}

ofQuaternion Ship::fromTwoVectors(ofVec3f u, ofVec3f v) {
	ofVec3f cross = u.getCrossed(v);
	float dot = u.dot(v);
	ofQuaternion quat(cross.x, cross.y, cross.z, dot + 1.f);
	return quat;
}
void Ship::seekRotate(ofVec3f target, float time) {
	ofVec3f targetLook = (target - position).getNormalized();
	tgl = targetLook;
	float angle = look.angle(targetLook);
	angular_velocity = 50;
	float rotationAngle = time * angular_velocity;
	float t = rotationAngle / angle;
	if(isnan(angle) || isnan(t) || t > 0.99f){
		look = targetLook;
	} else {
		look = (1-t)*look + t*targetLook; 
		look.normalize();
	}
	if(isnan(look.x)) {
		rot = rot;
	}
	rot = fromTwoVectors(look, basicLook);
	float a, x, y, z;
	rot.getRotate(a, x, y, z);
	model.setRotation(0, a, x, y, z);
}

bool Ship::seek(ofPoint target, float time) {

	seekRotate(target, time);

	ofVec3f targetLook = (target - look).getNormalized();
	if(sees(target)) {
		force += look * 100;
	} else {
		force -= look * 50;
	}

	return (target - position).length() < 200.0f;
}

bool Ship::sees(ofPoint target) {
	return (target-position).dot(look) > 0;
}

void Ship::move(float time) {
	velocity += time * force;
	if(velocity.length() > 100) {
		velocity = velocity.getNormalized() * 100;
	}
	position += velocity * time;
	ofVec3f newPosition = position + velocity * time;

	model.setPosition(position.x,position.y,position.z);
	
	position = newPosition;


}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     