#include "Chunk.h"

CChunk::CChunk(VECTOR_INT _coord):
	coord(_coord)
{
	memset(terrainData, 0, sizeof(terrainData));
}

CChunk::~CChunk()
{
}

bool CChunk::SetStructureByWorldPos(STRUCTURE_DATA structureData)
{
	VECTOR_INT tPosInChunk = VECTOR_INT(
		structureData.pos.x % CHUNK_SIZE,
		structureData.pos.z % CHUNK_SIZE
	);
	if (tPosInChunk.x < 0 || tPosInChunk.z < 0 || tPosInChunk.x >= CHUNK_SIZE || tPosInChunk.z >= CHUNK_SIZE)
		return false;
	terrainData[tPosInChunk.z][tPosInChunk.x] = structureData.structure_id;
}
