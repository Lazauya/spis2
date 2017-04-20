/*######################################
All code is from horsenit's junk_serialization
some variables clarified by me
########################################*/

#ifndef _EXTRAMENUDEFINES_H
#define _EXTRAMENUDEFINES_H

#include "skse/GameAPI.h"
#include "skse/GameReferences.h"
#include "skse/ScaleformCallbacks.h"
#include "skse/GameMenus.h"
#include "skse/Hooks_Scaleform.h"
#include "skse/ScaleformExtendedData.h"

class StandardItemData
{
public:
	virtual ~StandardItemData();

	virtual const char *	GetName(void);
	virtual UInt32			GetCount(void);
	virtual UInt32			GetEquipState(void);
	virtual UInt32			GetFilterFlag(void);
	virtual UInt32			GetFavorite(void);
	virtual bool			GetEnabled(void);

	//	void						** _vtbl;	// 00
	PlayerCharacter::ObjDesc	* objDesc;	// 04
	UInt32						OrginRefHandle;	// 08 
	UInt32						unk0C;		// 0C
	GFxValue					fxValue;	// 10

	MEMBER_FN_PREFIX(StandardItemData);
	DEFINE_MEMBER_FN(ctor_data, StandardItemData *, 0x00842140, GFxMovieView ** movieView, PlayerCharacter::ObjDesc * objDesc, UInt32 * src);

	enum { kCtorHookAddress = 0x008433B0 + 0x0049 };

	StandardItemData * ctor_Hook(GFxMovieView ** movieView, PlayerCharacter::ObjDesc * objDesc, UInt32 * src);
};

// 976580 - constructor?
class ItemMenuData
{
public:
	GFxMovieView * view; // 00
	UInt32 unk04;
	GFxValue listObject; // 08 - List object (gets selectedIndex property, etc)
	GFxValue itemList; // 18 - GFxValue array of items
	tArray<StandardItemData*> items; // 28
	UInt8 updatePending; // 34 - flag checked in GetSelectedItem, returns NULL if set
	UInt8 unk31[3];

	// grabs StandardItem that corresponds to listObject's selectedIndex
	StandardItemData* GetSelectedItem() {
		return CALL_MEMBER_FN(this, GetSelectedItem)();
	}

	MEMBER_FN_PREFIX(ItemMenuData);
	// returns null if updatePending above is set
	DEFINE_MEMBER_FN(GetSelectedItem, StandardItemData*, 0x00841D90);
	// sets updatePending above, then calls 0x00897F90 which adds an InventoryUpdate message to the queue
	//DEFINE_MEMBER_FN(ScheduleUpdate, void, 0x00841E70, TESForm* source /*not sure*/);
	//DEFINE_MEMBER_FN(InvalidateListData, void, 0x00841D30);
};

class BarterMenuExt : public IMenu
{
public:
	ItemMenuData * itemData; // 1C
	UInt32 unk20[(0x4C - 0x20) >> 2]; // 20
	tArray<TESForm*> changed; // 4C list of data that changed? tArray<TESForm*> ?
	UInt32 playerGold; // 58
	UInt32 vendorGold; // 5C
	UInt32 sortFlag; // 60

	MEMBER_FN_PREFIX(BarterMenuExt);
	DEFINE_MEMBER_FN(UpdateInventory, void, 0x844450);
};

class InventoryMenu : public IMenu
{
public:
	UInt32 unk1C;
	UInt32 unk20;
	UInt32 unk24;
	UInt32 unk28;
	UInt32 unk2C;
	ItemMenuData * itemData; // 30
	UInt32 unk34;
	UInt32 unk38;
	UInt32 unk3C;
	UInt32 unk40;
	UInt32 unkFlag; // 44 -- non-zero 86AA50 , zero 86A9F0
	UInt32 unk48;
	UInt32 sortFlag; // 4C switch on

	MEMBER_FN_PREFIX(InventoryMenu);
	DEFINE_MEMBER_FN(UpdateInventory, void, 0x86B980);
};

class ContainerMenu : public IMenu
{
public:
	UInt32 unk1C;
	UInt32 unk20;
	UInt32 unk24;
	UInt32 unk28;
	UInt32 unk2C;
	ItemMenuData * itemData; // 30
	UInt32 unk34;
	UInt32 unk38;
	UInt32 unk3C;
	UInt32 unk40;
	UInt32 unkFlag; // 44 -- non-zero 84A710 , zero 84B020
	UInt32 unk48[(0x6C - 0x48) >> 2];
	UInt32 sortFlag; // 6C

	MEMBER_FN_PREFIX(ContainerMenu);
	DEFINE_MEMBER_FN(UpdateInventory, void, 0x84B720);
};

// could be all wrong, haven't double checked
class GiftMenu : public IMenu
{
public:
	UInt32 unk1C;
	UInt32 unk20;
	UInt32 unk24;
	UInt32 unk28;
	UInt32 unk2C;
	ItemMenuData * itemData; // 30
	UInt32 unk34;
	UInt32 unk38;
	UInt32 unk3C;
	UInt32 unkFlag; // 40
	UInt32 unk44;
	UInt32 sortFlag; // 48

	MEMBER_FN_PREFIX(GiftMenu);
	DEFINE_MEMBER_FN(UpdateInventory, void, 0x85DDA0);
};

#endif