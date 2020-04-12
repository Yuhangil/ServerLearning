#pragma once
#include "Server.h"

class CChunk
{
public:
	TERRAIN_INFO terrainData[CHUNK_SIZE][CHUNK_SIZE];
	VECTOR_INT coord;
	bool active;

public:
	CChunk(VECTOR_INT coord);
	~CChunk();

public:
	void Update();

public:
	bool SetStructureByWorldPos(VECTOR_INT worldPos, unsigned int structure_id);
	TERRAIN_INFO GetTerrainData(VECTOR_INT worldPos);

};

