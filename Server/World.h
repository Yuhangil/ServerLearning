#pragma once

#include "Server.h"
#include "Chunk.h"

class CWorld
{
private:
	vector<STRUCTURE_DATA> structures;
	CChunk* chunks[WORLD_SIZE][WORLD_SIZE];

public:
	bool m_bActive;
	int m_iID;

public:
	CWorld();
	~CWorld();

public:
	bool AddStructure(STRUCTURE_DATA tStructureData);
	bool ChunkExist(VECTOR_INT tCoord);

private:
	bool CreateChunk(VECTOR_INT tCoord);
	VECTOR_INT GetChunkFromWorldPos(VECTOR_INT tWorldPos);
};

