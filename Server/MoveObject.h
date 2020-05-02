#pragma once
#include "Object.h"
class CMoveObject :
	public CObject
{
public:
	int maxStamina;
	int stamina;
	VECTOR velocity;

public:
	CMoveObject(unsigned int _objectCountID, int _objectID, int _maxHealth, int _maxStamina, VECTOR _pos);
	~CMoveObject();
};

