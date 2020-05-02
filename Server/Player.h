#pragma once

#include "Server.h"
#include "MoveObject.h"

class CPlayer :
	public CMoveObject
{
public:
	char playername[31];
	int client_id;
	ITEM_DATA items[INV_ROW * INV_COLUMN];
	ITEM_DATA handL;
	ITEM_DATA handR;

public :
	CPlayer(unsigned int objectCountID, int _client_id, int _maxHealth, int _maxStamina, VECTOR pos);
	~CPlayer();

public:
	void ModifyItemData(int slotID, int id, int amount);
	void SwapItemData(int srcSlotID, int dstSlotID);
	ITEM_DATA GetItemData(int slotID) const;

public:
	virtual void Update();

};

