#pragma once
#include "ship.h"
class CyanShip :
	public Ship
{
public:
	CyanShip(int);
	~CyanShip(void);
	void Reset() override;
	void findTarget() override;

};

