#include "MoveObject.h"

CMoveObject::CMoveObject(unsigned int _objectCountID, int _objectID, int _maxHealth, int _maxStamina,VECTOR _pos):
	CObject(_objectCountID, _objectID, _maxHealth, _pos),
	maxStamina(_maxStamina), stamina(_maxStamina)
{
}

CMoveObject::~CMoveObject()
{
}
