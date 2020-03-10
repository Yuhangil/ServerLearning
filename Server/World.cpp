#include "World.h"

CWorld::CWorld():
	m_bActive(true)
{
	puts("Create World");
}

CWorld::~CWorld()
{
	for (int i = 0; i < CHUNK_SIZE; i++)
		for (int j = 0; j < CHUNK_SIZE; j++)
			if (chunks[i][j] != NULL)
				delete chunks[i][j];
}

bool CWorld::AddStructure(STRUCTURE_DATA tStructureData)
{
	for (int z = 0; z < tStructureData.size.z; z++)
	{
		for (int x = 0; x < tStructureData.size.x; x++)
		{
			VECTOR_INT tCoord = GetChunkFromWorldPos(VECTOR_INT(tStructureData.pos.x + x, tStructureData.pos.z + z));

			if (ChunkExist(tCoord))
			{
				return chunks[tCoord.z][tCoord.x]->SetStructureFromWorldPos(tStructureData);
			}
			else
				return false;
		}
	}

	return true;
}

bool CWorld::ChunkExist(VECTOR_INT tCoord)
{
	if (tCoord.x < 0 || tCoord.z < 0 || tCoord.x >= WORLD_SIZE || tCoord.z >= WORLD_SIZE)
		return false;
	if (chunks[tCoord.z][tCoord.x] == NULL)
		return CreateChunk(tCoord);
	return true;
}

bool CWorld::CreateChunk(VECTOR_INT tCoord)
{
	CChunk* chunk = new CChunk(tCoord);
	if (chunk == NULL)
		return false;
	chunks[tCoord.z][tCoord.x] = chunk;
	return true;
}

VECTOR_INT CWorld::GetChunkFromWorldPos(VECTOR_INT tWorldPos)
{
	// (x + (chunkSize / 2) * sign(x) + ( x < 0 ? 1 : 0 )) / chunkSize
	// (z + (chunkSize / 2) * sign(z) + ( z < 0 ? 1 : 0 )) / chunkSize
	return VECTOR_INT(
		(tWorldPos.x + (CHUNK_SIZE / 2) * (tWorldPos.x > 0 ? 1 : tWorldPos.x < 0 ? -1 : 0) + (tWorldPos.x < 0? 1 : 0)) / CHUNK_SIZE,
		(tWorldPos.z + (CHUNK_SIZE / 2) * (tWorldPos.z > 0 ? 1 : tWorldPos.z < 0 ? -1 : 0) + (tWorldPos.z < 0 ? 1 : 0)) / CHUNK_SIZE
	);
}
