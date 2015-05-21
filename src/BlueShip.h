#pragma once
#include "ship.h"
class BlueShip :
	public Ship
{
public:
	BlueShip(int);
	~BlueShip(void);
	void Reset() override;
	void findTarget() override;

};
