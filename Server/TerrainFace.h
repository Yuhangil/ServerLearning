#pragma once

#include "Server.h"
#include "Chunk.h"
#include "Noise.h"

class CTerrainFace
{
public:
	static int resolution;
	CChunk* chunks[CHUNKS_PER_TERRAINFACE][CHUNKS_PER_TERRAINFACE];

public:
	CTerrainFace(VECTOR localUp, CNoise* noise);
	~CTerrainFace();

public:
	void GetChunkData(int dst[][CHUNK_SIZE], VECTOR_INT chunkCoord);
	bool SetStructure(STRUCTURE_DATA structureData);
};

