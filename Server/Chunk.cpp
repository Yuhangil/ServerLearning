#include "Chunk.h"

CChunk::CChunk(VECTOR_INT tCoord):
	m_tCoord(tCoord)
{
	memset(m_iTerrainData, 0, sizeof(m_iTerrainData));
}

CChunk::~CChunk()
{
}

bool CChunk::SetStructureFromWorldPos(STRUCTURE_DATA tStructureData)
{
	VECTOR_INT tPosInChunk = VECTOR_INT(
		tStructureData.pos.x - m_tCoord.x * CHUNK_SIZE + CHUNK_SIZE / 2,
		tStructureData.pos.z - m_tCoord.z * CHUNK_SIZE + CHUNK_SIZE / 2
	);
	if (tPosInChunk.x < 0 || tPosInChunk.z < 0 || tPosInChunk.x >= CHUNK_SIZE || tPosInChunk.z >= CHUNK_SIZE)
		return false;
	m_iTerrainData[tPosInChunk.z][tPosInChunk.x] = tStructureData.structure_id;
}
