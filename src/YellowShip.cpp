#include "YellowShip.h"


YellowShip::YellowShip(int team)
{
	this->team = team;
	Reset();
	loadModel("models/MS/MS Superiority Fighter.3DS", 0.5, ofColor(255, 255, 0));

	//basicLook = ofVec3f(1,0,0);
}

YellowShip::~YellowShip(void)
{
	
}

void YellowShip::Reset() {
	Init(150, 50, 5, 1000);
	cout << "Respawn: Yellow #" << team << endl;
}

ofVec3f YellowShip::pursuit(shared_ptr<Ship> enemy) {
	if(enemy->velocity.length() == 0) {
		// enemy is stationary
		return enemy->position;
	} else if (enemy->sees(this->position)) {
		// enemies facing each other
		return enemy->position;
	} else {
		float distanceToEnemy = (enemy->position - position).length();
		float t = distanceToEnemy / speed;
		ofVec3f offset = enemy->look * t * enemy->speed;
		return enemy->position + offset;
	}
}

void YellowShip::findTarget() {
	if(hasTarget) {
		target = pursuit(target_enemy);
		return;
	} 
	
	shared_ptr<Ship> enemy;
	do {
		enemy = ships[rand() % ships.size()];
	} while (enemy->team == this->team);

	target = pursuit(enemy);
	target_enemy = enemy;
	hasTarget = true;
}
