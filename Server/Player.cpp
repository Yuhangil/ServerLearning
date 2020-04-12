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
	items[slotID].id = id;
	items[slotID].amount = amount;
}

void CPlayer::SwapItemData(int srcSlotID, int dstSlotID)
{
	ITEM_DATA tmp = items[srcSlotID];
	if (dstSlotID >= INV_ROW * INV_COLUMN)
	{
		dstSlotID -= INV_ROW * INV_COLUMN;
		if (dstSlotID == 0)
		{
			items[srcSlotID] = handL;
			handL = tmp;
		}
		else
		{
			items[srcSlotID] = handR;
			handR = tmp;
		}
	}
	else
	{
		items[srcSlotID] = items[dstSlotID];
		items[dstSlotID] = tmp;
	}
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
