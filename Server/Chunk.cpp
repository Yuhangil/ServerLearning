#include "Chunk.h"

CChunk::CChunk(VECTOR_INT _coord):
	coord(_coord), active()
{
	memset(terrainData, 0, sizeof(terrainData));
}

CChunk::~CChunk()
{
}

void CChunk::Update()
{
}

bool CChunk::SetStructureByWorldPos(VECTOR_INT worldPos, unsigned int structure_id)
{
	VECTOR_INT posInChunk = VECTOR_INT(
		worldPos.x % CHUNK_SIZE,
		worldPos.z % CHUNK_SIZE
	);
	if (posInChunk.x < 0 || posInChunk.z < 0)
		return false;
	terrainData[posInChunk.z][posInChunk.x].structure = structure_id;
}

TERRAIN_INFO CChunk::GetTerrainData(VECTOR_INT worldPos)
{
	if (worldPos.x < 0 || worldPos.z < 0)
		return TERRAIN_INFO();
	return terrainData[worldPos.z % CHUNK_SIZE][worldPos.x % CHUNK_SIZE];
}
