#include "PurpleShip.h"


PurpleShip::PurpleShip(int team)
{
	this->team = team;
	Reset();
	loadModel("models/Tagawa/Tagawa.obj", 1, ofColor(255, 0, 255));
}


PurpleShip::~PurpleShip(void)
{
}

void PurpleShip::Reset() {
	Init(25, 70, 15, 2000);
	cout << "Respawn: Purple #" << team << endl;
}

void PurpleShip::findTarget() {
	if(hasTarget && sees(target_enemy->position)) {
		// target enemy still in view
		target = target_enemy->position;
		return;
	}

	// lost the target enemy
	// find new target enemy
	float closest = 5000;
	for(auto enemy : ships) {
		if(enemy->team != this->team) {
			float distance = (enemy->position - this->position).length();
			if(distance < closest) {
				closest = distance;
				target = enemy->position;
				target_enemy = enemy;
				hasTarget = true;
			}
		}
	}
}