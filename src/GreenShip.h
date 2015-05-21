#pragma once
#include "ship.h"
class GreenShip :
	public Ship
{
public:
	GreenShip(int);
	~GreenShip(void);
	void Reset() override;
	void findTarget() override;
};

