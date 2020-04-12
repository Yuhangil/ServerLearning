#pragma once

#include "Server.h"
#include "Noise.h"
#include "Chunk.h"
#include "Object.h"

class CWorld
{
private:
	std::vector<STRUCTURE_DATA> structures;
	CChunk* chunks[CHUNKS_PER_WORLD][CHUNKS_PER_WORLD];
	std::vector<CObject*> objects;

public:
	bool active;
	int id;

public:
	CWorld();
	~CWorld();

public:
	void Update();

public:
	CChunk* GetChunkByCoord(int chunkX, int chunkZ);
	CChunk* GetChunkByCoord(VECTOR_INT coord);
	CChunk* GetChunkByWorldPos(VECTOR_INT pos);
	bool AddStructure(STRUCTURE_DATA tStructureData);
	
private:
	void CreateWorld(int seed);
	float FalloffModify(float value);
};

