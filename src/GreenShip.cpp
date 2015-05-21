#include "GreenShip.h"


GreenShip::GreenShip(int team)
{
	this->team = team;
	Reset();
	loadModel("models/MS/MS Heavy Fighter.3DS", 0.5, ofColor(0, 255, 0));
}


GreenShip::~GreenShip(void)
{
}

void GreenShip::Reset() {
	Init(100, 30, 7, 1100);
	cout << "Respawn: Green #" << team << endl;
}

void GreenShip::findTarget() {
	float closest = ARENA_SIZE;
	for(auto ally : ships) {
		if(ally->team == this->team) {
			if(ally->hasTarget && !ally->outOfBounds) {
				float distance = (ally->position - this->position).length();
				if(distance < closest && distance > 0) {
					closest = distance;
					target = ally->target;
					target_enemy = ally->target_enemy;
					hasTarget = true;
				}
			}
		}
	}
}
