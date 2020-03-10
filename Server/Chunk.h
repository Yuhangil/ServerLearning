#pragma once
#include "Server.h"

class CChunk
{
public:
	unsigned int m_iTerrainData[CHUNK_SIZE][CHUNK_SIZE];
	VECTOR_INT m_tCoord;

public:
	CChunk(VECTOR_INT m_tCoord);
	~CChunk();

public:
	bool SetStructureFromWorldPos(STRUCTURE_DATA tStructureData);
};

