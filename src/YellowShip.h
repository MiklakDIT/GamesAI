#pragma once
#include "ship.h"
class YellowShip :
	public Ship
{
public:
	YellowShip(int);
	~YellowShip(void);
	void findTarget() override;
	void Reset() override;
	ofVec3f pursuit(shared_ptr<Ship> enemy);
};

