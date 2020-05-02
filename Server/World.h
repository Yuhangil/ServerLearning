#pragma once

#include "Server.h"
#include "Noise.h"
#include "Chunk.h"
#include "Object.h"

class CWorld
{
private:
	static unsigned int gObjectCounter;

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
	void CreateWorld(int seed);

	static void MakeObjectBuffer(char* buffer, int objectCountID, int objectID, VECTOR pos);

public:
	CChunk* GetChunkByCoord(int chunkX, int chunkZ);
	CChunk* GetChunkByCoord(VECTOR_INT coord);
	CChunk* GetChunkByWorldPos(VECTOR_INT pos);
	bool AddStructure(STRUCTURE_DATA tStructureData);
	std::vector<CObject*> GetObjects() const
	{
		return objects;
	}

private:
	void CreateLandScape(int seed);
	void AddObject(int objectID, VECTOR pos);
	float FalloffModify(float value);
};

