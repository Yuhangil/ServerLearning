#include "World.h"

// [ X, Z ][ direction ][ rollValue ]
// rollValue : 
//		diceRoll[][][3]자리에 diceRoll[][][0]
//		diceRoll[][][0]자리에 diceRoll[][][1]
//		diceRoll[][][1]자리에 diceRoll[][][2]
//		diceRoll[][][2]자리에 diceRoll[][][3]
const int CWorld::diceRoll[2][2][4] = {
	{{2, 5, 1, 0}, {1, 5, 2, 0}},
	{{3, 5, 4, 0}, {4, 5, 3, 0}}
};

CWorld::CWorld():
	active(true)
{
	noise = new CNoise();

	VECTOR direction[6] = {
		{0, 1, 0}, {1, 0, 0}, {-1, 0, 0},
		{0, 0, -1}, {0, 0, 1}, {0, -1, 0}
	};
	// 위, 동, 서, 남, 북, 아래

	for (int i = 0; i < 6; i++)
	{
		terrainFace[i] = new CTerrainFace(direction[i], noise);
	}
}

CWorld::~CWorld()
{
	delete noise;
	for (int i = 0; i < 6; i++)
		delete terrainFace[i];
}

bool CWorld::AddStructure(STRUCTURE_DATA tStructureData)
{
	for (int z = 0; z < tStructureData.size.z; z++)
	{
		for (int x = 0; x < tStructureData.size.x; x++)
		{
			
		}
	}

	return true;
}

CTerrainFace* CWorld::GetTerrainFace(int idx)
{
	return terrainFace[idx];
}


int CWorld::GetTerrainFaceIdxFromWorldPos(VECTOR worldPos) // worldPos : 게임 내에서 논리적으로 Y축을 추가해 x, z축을 지면축으로
{
	if (worldPos.x < 0) worldPos.x += CTerrainFace::resolution * 4;
	if (worldPos.z < 0) worldPos.z += CTerrainFace::resolution * 4;
	// 정육면체를 몇 번 굴렸는지에 대한 값
	int rollX = (worldPos.x / CTerrainFace::resolution);
	int rollZ = (worldPos.z / CTerrainFace::resolution);

	rollX = rollX % 4;
	rollZ = rollZ % 4;
	
	// 정육면체를 굴려서 worldPos에 해당하는 면이 윗면에 오도록
	int dice[6] = { 0, 1, 2, 3, 4, 5 };
	int dir = ((0 < worldPos.x) - (0 > worldPos.x) + 1) / 2;
	for (int i = 0; i < abs(rollX); i++)
	{
		int t = dice[diceRoll[0][dir][3]];
		dice[diceRoll[0][dir][3]] = dice[diceRoll[0][dir][0]];
		dice[diceRoll[0][dir][0]] = dice[diceRoll[0][dir][1]];
		dice[diceRoll[0][dir][1]] = dice[diceRoll[0][dir][2]];
		dice[diceRoll[0][dir][2]] = t;
	}
	dir = ((0 < worldPos.z) - (0 > worldPos.z) + 1) / 2;
	for (int i = 0; i < abs(rollZ); i++)
	{
		int t = dice[diceRoll[1][dir][3]];
		dice[diceRoll[1][dir][3]] = dice[diceRoll[1][dir][0]];
		dice[diceRoll[1][dir][0]] = dice[diceRoll[1][dir][1]];
		dice[diceRoll[1][dir][1]] = dice[diceRoll[1][dir][2]];
		dice[diceRoll[1][dir][2]] = t;
	}

	return dice[0];
}

