#pragma once
#include "Server.h"

class CChunk
{
public:
	float terrainData[CHUNK_SIZE][CHUNK_SIZE];
	VECTOR_INT coord;

public:
	CChunk(VECTOR_INT coord);
	~CChunk();

public:
	bool SetStructureByWorldPos(STRUCTURE_DATA structureData);
};

