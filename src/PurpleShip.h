#pragma once
#include "ship.h"
class PurpleShip :
	public Ship
{
public:
	PurpleShip(int);
	~PurpleShip(void);
	void Reset() override;
	void findTarget() override;
};

