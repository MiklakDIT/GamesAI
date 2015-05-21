#include "RedShip.h"


RedShip::RedShip(int team)
{
	this->team = team;
	Reset();
	loadModel("models/Mak/Mak fighter.obj", 0.5, ofColor(255, 0, 0));
}


RedShip::~RedShip(void)
{
}

void RedShip::Reset() {
	Init(200, 40, 15, 500);
	cout << "Respawn: Red #" << team << endl;
}

void RedShip::findTarget() {
	if(hasTarget && target_enemy && !target_enemy->sees(this->position)) {
		// target enemy still in view
		target = target_enemy->position;
		return;
	}

	// lost the target enemy
	// find new target enemy
	for(auto enemy : ships) {
		if(enemy->team != this->team) {
			if(!enemy->sees(this->position)) {
				target = enemy->position;
				target_enemy = enemy;
				hasTarget = true;
				return;
			}
		}
	}
	
	if(hasTarget) {
		target = target_enemy->position;
	}
}
