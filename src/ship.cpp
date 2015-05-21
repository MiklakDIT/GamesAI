#include "ship.h"
vector<shared_ptr<Ship>> Ship::ships;
bool Ship::showTarget = false;
void Ship::AttachShip(shared_ptr<Ship> ship) {
	ships.push_back(ship);
}

void Ship::DetachAll() {
	ships.clear();
}

void Ship::Reset() {}

void Ship::Init(int max_speed, int agility, int attack, int strength) {
	basicLook = ofVec3f(0,0,1);
	position = ofVec3f(RANDOM_XYZ, RANDOM_XYZ, RANDOM_XYZ);
	look = ofVec3f(0,0,1);
	force = ofVec3f(0);
	velocity = ofVec3f(0);
	hasTarget = false;
	outOfBounds = false;
	//target = ofVec3f(rand() % 20, rand() % 20, -rand() % 20) * 40;
	this->max_speed = max_speed;
	this->agility = agility;
	this->attack = attack;
	this->strength = strength;
}

Ship::~Ship(void)
{
}

void Ship::loadModel(string path, float scale, ofColor& color) {
	model.loadModel(path);
	model.enableTextures();
	model.enableColors();
	model.enableMaterials();
	model.calculateDimensions();
	model.setScale(scale, scale, scale);
	if(team == 0) {
		color.setBrightness(125);
	} else {
		color.setSaturation(125);
	}
	this->color = color;
}

void Ship::drawVec(string label, ofVec3f vec, int& y) {
	y += 15;
	string s = label + '(' + to_string(vec.x) + ',' + to_string(vec.y) + ',' + to_string(vec.z) + ')';
	ofDrawBitmapString(s,5,y,0);
}
ofVec3f tgl;

void Ship::draw() {	
	//tex.bind();
	ofSetColor(color);
	model.drawFaces();
	//tex.unbind();
	
	if(showTarget && hasTarget) {
		ofLine(position, target);
		ofBox(target, 10);
	}
}

void Ship::drawDebug() {
	ofSetColor(color);
	int y = 0;
	if(color == ofColor(255, 255, 0)) {
		drawVec("look", look, y);
		drawVec("target", tgl, y);
		drawVec("velocity", velocity, y);
		drawVec("force", force, y);
	}
}

bool Ship::hit(int attack) {
	strength -= attack;
	if(strength < 0) {
		Reset();
		return true;
	}
	return false;
}

void Ship::returnToArena(float timeDelta) {
	target = ofVec3f(0,0,0);
	outOfBounds = true;
	seek(target, timeDelta);
}

void Ship::returnToNormal(float timeDelta) {
	if(position.length() <= ARENA_SIZE * 0.6) {
		outOfBounds = false;
		hasTarget = false;
	} else {
		seek(target, timeDelta);
	}
}

void Ship::standardBehaviour(float timeDelta) {
	findTarget();
	if(hasTarget && seek(target, timeDelta)) {
		hasTarget = false;
		//target = ofVec3f(rand() % 800, rand() % 800, -rand() % 800);
	}
}

#define WARNING_DISTANCE 500
bool Ship::avoidCollision(float timeDelta) {
	for(auto ship : ships) {
		if(ship->position != this->position) {
			if(this->sees(ship->position, 25)) {
				ofVec3f toShip = ship->position - this->position;
				float distance = toShip.length();
				if(distance < WARNING_DISTANCE) {
					ofVec3f trajectory = position + look * distance;
					ofVec3f offset = trajectory - ship->position;
					target = ship->position + offset * 5;
					hasTarget = true;
					isAvoiding = true;
					seek(target, timeDelta);
					return false;
				}
			}
		}
	}
	isAvoiding = false;
	return true;
}

void Ship::update(float timeDelta, float intensity) {
	force *= 0;
	speed = max_speed * (0.5 + intensity * 2);
	if(avoidCollision(timeDelta)) {
		if(position.length() >= ARENA_SIZE) {
			returnToArena(timeDelta);
		} else if (outOfBounds) {
			returnToNormal(timeDelta);
		} else {
			standardBehaviour(timeDelta);		
		}
	}
		
	move(timeDelta);
}

shared_ptr<Projectile> Ship::shoot(shared_ptr<Note> note, float intensity) {
	shared_ptr<Projectile> p = make_shared<Projectile>();
	p->setAttack(attack);
	p->setPosition(position);
	p->setColor(color);
	p->setSize(10 + note->amplitude * 50);
	p->setSpeed(100 + intensity * 300);
	if(target_enemy && sees(target_enemy->position, 40)) {
		p->setLook((target_enemy->position - this->position).getNormalized());
	} else {
		p->setLook(look);
	}
	return p;
}

ofQuaternion Ship::fromTwoVectors(ofVec3f u, ofVec3f v) {
	ofVec3f cross = u.getCrossed(v);
	float dot = u.dot(v);
	ofQuaternion quat(cross.x, cross.y, cross.z, dot + 1.f);
	return quat;
}
void Ship::seekRotate(ofVec3f& target, float time) {
	ofVec3f targetLook = (target - position).getNormalized();
	tgl = targetLook;
	float angle = look.angle(targetLook);
	float rotationAngle = time * agility;
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
		force += look * speed;
	} else {
		force -= look * speed / 2;
	}

	return (target - position).length() < 200.0f;
}

bool Ship::sees(ofPoint target, float angle) {
	return look.angle(target-position) < angle;
}

void Ship::move(float time) {
	velocity += time * force;
	if(velocity.length() > speed) {
		velocity = velocity.getNormalized() * speed;
	}
	position += velocity * time;
	ofVec3f newPosition = position + velocity * time;

	model.setPosition(position.x,position.y,position.z);
	
	position = newPosition;
}                 

void Ship::findTarget() {

}