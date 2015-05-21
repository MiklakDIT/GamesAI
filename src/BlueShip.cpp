#include "BlueShip.h"


BlueShip::BlueShip(int team)
{
	this->team = team;
	Reset();
	loadModel("models/Nautilus/Nautilus explorer.3DS", 1, ofColor(0, 0, 255));
}


BlueShip::~BlueShip(void)
{
}

void BlueShip::Reset() {
	Init(50, 10, 10, 1500);
	cout << "Respawn: Blue #" << team << endl;
}

void BlueShip::findTarget() {
	if(hasTarget && sees(target_enemy->position)) {
		// target enemy still in view
		target = target_enemy->position;
		return;
	}

	// lost the target enemy
	// find new target enemy
	float closestAngle = 180;
	for(auto enemy : ships) {
		if(enemy->team != this->team) {
			ofVec3f toEnemy = enemy->position - this->position;
			float angle = look.angle(toEnemy);
			if(angle < closestAngle) {
				closestAngle = angle;
				target = enemy->position;
				target_enemy = enemy;
				hasTarget = true;
			}
		}
	}
}