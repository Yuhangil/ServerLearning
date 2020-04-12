#include "World.h"

CWorld::CWorld():
	active(true)
{
	CreateWorld(0);
}

CWorld::~CWorld()
{
	for (int i = 0; i < CHUNKS_PER_WORLD; i++)
		for (int j = 0; j < CHUNKS_PER_WORLD; j++)
			delete chunks[i][j];
}

void CWorld::Update()
{

	for (auto obj = objects.begin(); obj != objects.end(); obj++)
	{
		if (chunks[(*obj)->chunkX][(*obj)->chunkZ]->active)
			(*obj)->Update();
	}
}

CChunk* CWorld::GetChunkByCoord(int chunkX, int chunkZ)
{
	if (chunkX < 0 || chunkZ < 0 || chunkX >= CHUNKS_PER_WORLD || chunkZ >= CHUNKS_PER_WORLD)
		return nullptr;
	return chunks[chunkZ][chunkX];
}

CChunk* CWorld::GetChunkByCoord(VECTOR_INT coord)
{
	return GetChunkByCoord(coord.x,coord.z);
}

CChunk* CWorld::GetChunkByWorldPos(VECTOR_INT pos)
{
	int x = pos.x / CHUNK_SIZE;
	int z = pos.z / CHUNK_SIZE;
	return GetChunkByCoord(x, z);
}

bool CWorld::AddStructure(STRUCTURE_DATA stData)
{
	for (int z = 0; z < stData.size.z; z++)
	{
		for (int x = 0; x < stData.size.x; x++)
		{
			VECTOR_INT next = VECTOR_INT(stData.pos.x + x, stData.pos.z + z);
			if (GetChunkByWorldPos(next)->GetTerrainData(next).structure != 0)
				return false;
		}
	}
	for (int z = 0; z < stData.size.z; z++)
	{
		for (int x = 0; x < stData.size.x; x++)
		{
			VECTOR_INT next = VECTOR_INT(stData.pos.x + x, stData.pos.z + z);
			GetChunkByWorldPos(next)->SetStructureByWorldPos(next, stData.structure_id);
		}
	}


	return true;
}

void CWorld::CreateWorld(int seed)
{
	CNoise* noise = new CNoise(seed);

	for (int i = 0; i < CHUNKS_PER_WORLD; i++)
		for (int j = 0; j < CHUNKS_PER_WORLD; j++)
			chunks[i][j] = new CChunk(VECTOR_INT(j, i));

	//Noise Setting
	float roughness = 2.34;
	float persistence = 0.54;
	float scale = 75;

	//Noise Info
	int size = CHUNKS_PER_WORLD * CHUNK_SIZE;
	float noiseMaxValue = 0;
	float noiseMinValue = 1e9;

	float halfSize = size / 2.f;

	for (int z = 0; z < size; z++)
	{
		for (int x = 0; x < size; x++)
		{
			float noiseValue = 0;
			float frequency = 1;
			float amplitude = 1;

			for (int i = 0; i < 3; i++)
			{
				float _x = (x - halfSize) / scale * frequency;
				float _z = (z - halfSize) / scale * frequency;

				float v = noise->Evaluate(VECTOR(_x, _z, 0));
				noiseValue += (v + 1) * .5f * amplitude;
				frequency *= roughness;
				amplitude *= persistence;
			}

			if (noiseMaxValue < noiseValue)
				noiseMaxValue = noiseValue;
			if (noiseMinValue > noiseValue)
				noiseMinValue = noiseValue;

			chunks[z / CHUNK_SIZE][x / CHUNK_SIZE]->terrainData[z % CHUNK_SIZE][x % CHUNK_SIZE].noise = noiseValue;
		}
	}

	for (int z = 0; z < size; z++)
	{
		for (int x = 0; x < size; x++)
		{
			float v = chunks[z / CHUNK_SIZE][x / CHUNK_SIZE]->terrainData[z % CHUNK_SIZE][x % CHUNK_SIZE].noise;
			v = v / (noiseMaxValue - noiseMinValue);

			float falloffValue = max(abs(z / (float)size * 2 - 1), abs(x / (float)size * 2 - 1));
			falloffValue = FalloffModify(falloffValue);
			v -= falloffValue;
			if (v < 0)
				v = 0;

			chunks[z / CHUNK_SIZE][x / CHUNK_SIZE]->terrainData[z % CHUNK_SIZE][x % CHUNK_SIZE].noise = v;
		}
	}


	delete noise;
}

float CWorld::FalloffModify(float value)
{
	float a = 3.f;
	float b = 2.2f;
	return pow(value, a) / (pow(value, a) + pow(b - b* value, a));
}
