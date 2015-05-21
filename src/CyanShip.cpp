#include "CyanShip.h"


CyanShip::CyanShip(int team)
{
	this->team = team;
	Reset();
	loadModel("models/MS/MS Bomber.3DS", 0.5, ofColor(0, 255, 255));
}


CyanShip::~CyanShip(void)
{
}

void CyanShip::Reset() {
	Init(100, 20, 7, 1500);
	cout << "Respawn: Cyan #" << team << endl;
}


void CyanShip::findTarget() {
	float closestAngle = 180;
	for(auto enemy : ships) {
		if(enemy->team != this->team) {
			ofVec3f toEnemy = enemy->position - this->position;
			float angle = look.angle(toEnemy);
			if(angle < closestAngle) {
				closestAngle = angle;
				target_enemy = enemy;
			}
		}
	}

	if(hasTarget) {
		return;
	}

	target = ofVec3f(RANDOM_XYZ, RANDOM_XYZ, RANDOM_XYZ);
	hasTarget = true;

}