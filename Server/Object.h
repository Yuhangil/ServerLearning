#pragma once
#include "Server.h"

class CObject
{
public:
	int chunkX, chunkZ;
	VECTOR pos;
	VECTOR velocity;

public:
	CObject();
	~CObject();

public:
	virtual void Update();
};

