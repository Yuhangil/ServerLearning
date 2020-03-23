#pragma once

#include "Server.h"
#include "TerrainFace.h"
#include "Noise.h"

class CWorld
{
private:
	static const int diceRoll[2][2][4];
private:
	std::vector<STRUCTURE_DATA> structures;
	CTerrainFace* terrainFace[6];
	CNoise* noise;

public:
	bool active;
	int id;

public:
	CWorld();
	~CWorld();

public:
	bool AddStructure(STRUCTURE_DATA tStructureData);
	CTerrainFace* GetTerrainFace(int idx);
	int GetTerrainFaceIdxFromWorldPos(VECTOR worldPos);
};

