#pragma once
#include "Server.h"

class CObject
{
public:
	unsigned int objectCountID;
	int objectID;
	int worldIdx;
public:
	int maxHealth;
	int health;
	VECTOR pos;
	int chunkX, chunkZ;

public:
	CObject(unsigned int _objectCountID, int _objectID, int _maxHealth, VECTOR _pos);
	~CObject();

public:
	virtual void Update();
};

