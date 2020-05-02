#include "Object.h"

CObject::CObject(unsigned int _objectCountID, int _objectID, int _maxHealth, VECTOR _pos) :
	objectCountID(_objectCountID), objectID(_objectID), 
	pos(_pos),
	maxHealth(_maxHealth), health(_maxHealth), worldIdx(0)
{
	chunkX = _pos.x / CHUNK_SIZE;
	chunkZ = _pos.z / CHUNK_SIZE;

}

CObject::~CObject()
{
}

void CObject::Update()
{
}
