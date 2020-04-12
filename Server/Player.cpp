#include "Player.h"

CPlayer::CPlayer(int _client_id):
	CObject(),
	client_id(_client_id)
{
	memset(items, 0, sizeof(items));
	handL = handR = { 0, };
}

CPlayer::~CPlayer()
{
}

void CPlayer::ModifyItemData(int slotID, int id, int amount)
{
	if (slotID >= INV_ROW * INV_COLUMN)
	{
		if (slotID - INV_ROW * INV_COLUMN == 0)
		{
			handL.id = id;
			handL.amount = amount;
		}
		else
		{
			handR.id = id;
			handR.amount = amount;
		}
	}
	else
	{
		items[slotID].id = id;
		items[slotID].amount = amount;
	}
}

void CPlayer::SwapItemData(int srcSlotID, int dstSlotID)
{
	ITEM_DATA src = GetItemData(srcSlotID);
	ITEM_DATA dst = GetItemData(dstSlotID);
	ModifyItemData(srcSlotID, dst.id, dst.amount);
	ModifyItemData(dstSlotID, src.id, src.amount);
}

ITEM_DATA CPlayer::GetItemData(int slotID) const
{
	if (slotID < INV_ROW * INV_COLUMN)
		return items[slotID];
	
	if (slotID - INV_ROW * INV_COLUMN == 0)
		return handL;
	
	return handR;
}

void CPlayer::Update()
{
}
