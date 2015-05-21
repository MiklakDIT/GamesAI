#pragma once
#include "ship.h"
class RedShip :
	public Ship
{
public:
	RedShip(int);
	~RedShip(void);
	void Reset() override;
	void findTarget() override;
	ofVec3f pursuit(shared_ptr<Ship> enemy);

};

