#include "Projectile.h"
#include "ship.h"

Projectile::Projectile(ofVec3f& look, ofVec3f& position, int speed, int attack, int size, ofColor& color) {
	this->look = look;
	this->position = position;
	this->speed = speed;
	this->attack = attack;
	this->size = size;
	this->color = color;
	this->alive = true;
}

Projectile::Projectile() {
	this->alive = true;
}


void Projectile::setLook(ofVec3f& look) {
	this->look = look;
}
void Projectile::setPosition(ofVec3f& position) {
	this->position = position;
}
void Projectile::setSpeed(int speed) {
	this->speed = speed;
}
void Projectile::setAttack(int attack) {
	this->attack = attack;
}
void Projectile::setSize(int size) {
	this->size = size;
}
void Projectile::setColor(ofColor& color) {
	this->color = color;
}

void Projectile::draw() {
	ofSetColor(color);
	ofSphere(position, size);
}

#define HIT_RANGE 100
int Projectile::update(float timeDelta) {
	position += look * speed;
	if(position.length() > 10000) {
		alive = false;
		return -1;
	}
	
	//position += look * speed;

	for(auto ship : Ship::ships) {
		if((ship->position - position).length() < HIT_RANGE) {
			if(ship->color != this->color) {
				alive = false;
				if(ship->hit(attack)) {
					return ship->team;
				}
			}
		}
	}
	
	return -1;
};

