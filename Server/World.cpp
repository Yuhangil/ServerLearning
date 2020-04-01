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
	CreateWorld(0);
}

CWorld::~CWorld()
{
	for (int i = 0; i < CHUNKS_PER_WORLD; i++)
		for (int j = 0; j < CHUNKS_PER_WORLD; j++)
			delete chunks[i][j];
}

CChunk* CWorld::GetChunk(int x, int z)
{
	if (x < 0 || z < 0 || x >= CHUNKS_PER_WORLD || z >= CHUNKS_PER_WORLD)
		return nullptr;
	return chunks[z][x];
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

			chunks[z / CHUNK_SIZE][x / CHUNK_SIZE]->terrainData[z % CHUNK_SIZE][x % CHUNK_SIZE] = noiseValue;
		}
	}

	for (int z = 0; z < size; z++)
	{
		for (int x = 0; x < size; x++)
		{
			float v = chunks[z / CHUNK_SIZE][x / CHUNK_SIZE]->terrainData[z % CHUNK_SIZE][x % CHUNK_SIZE];
			v = v / (noiseMaxValue - noiseMinValue);

			float falloffValue = max(abs(z / (float)size * 2 - 1), abs(x / (float)size * 2 - 1));
			falloffValue = FalloffModify(falloffValue);
			v -= falloffValue;
			if (v < 0)
				v = 0;

			chunks[z / CHUNK_SIZE][x / CHUNK_SIZE]->terrainData[z % CHUNK_SIZE][x % CHUNK_SIZE] = v;
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
