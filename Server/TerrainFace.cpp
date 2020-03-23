#include "TerrainFace.h"

int CTerrainFace::resolution = CHUNKS_PER_TERRAINFACE * CHUNK_SIZE;

CTerrainFace::CTerrainFace(VECTOR tLocalUp, CNoise* noise)
{
	for (int i = 0; i < CHUNKS_PER_TERRAINFACE; i++)
	{
		for (int j = 0; j < CHUNKS_PER_TERRAINFACE; j++)
		{
			chunks[i][j] = new CChunk(VECTOR_INT(i, j));
		}
	}


	// 정육면체의 한 면으로 2D X, Y축으로 구성
	VECTOR tAxisA = VECTOR(tLocalUp.y, tLocalUp.z, tLocalUp.x);
	VECTOR tAxisB = VECTOR::Cross(tLocalUp, tAxisA);
	for (int y = 0; y < resolution; y++)
	{
		for (int x = 0; x < resolution; x++)
		{
			float percentX = (float)x / (resolution - 1);
			float percentY = (float)y / (resolution - 1);
			VECTOR pointOnUnitCube = tLocalUp + tAxisA * ((percentX - .5f) * 2) + tAxisB * ((percentY - .5f) * 2);
			VECTOR pointOnUnitSphere = pointOnUnitCube / pointOnUnitCube.Magnitude();
			chunks[y / CHUNK_SIZE][x / CHUNK_SIZE]->terrainData[y % CHUNK_SIZE][x % CHUNK_SIZE] = (int)(noise->GetValue(pointOnUnitSphere) * 10);
		}
	}
}

CTerrainFace::~CTerrainFace()
{
	for (int i = 0; i < CHUNKS_PER_TERRAINFACE; i++)
	{
		for (int j = 0; j < CHUNKS_PER_TERRAINFACE; j++)
		{
			delete chunks[i][j];
		}
	}
}
