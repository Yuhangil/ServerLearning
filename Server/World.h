#pragma once

#include "Server.h"
#include "Noise.h"
#include "Chunk.h"

class CWorld
{
private:
	static const int diceRoll[2][2][4];
private:
	std::vector<STRUCTURE_DATA> structures;
	CChunk* chunks[CHUNKS_PER_WORLD][CHUNKS_PER_WORLD];

public:
	bool active;
	int id;

public:
	CWorld();
	~CWorld();

public:
	CChunk* GetChunk(int x, int z);

public:
	bool AddStructure(STRUCTURE_DATA tStructureData);
	
private:
	void CreateWorld(int seed);
	float FalloffModify(float value);
};

