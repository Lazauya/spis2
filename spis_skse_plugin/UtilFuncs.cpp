#include "skse/ScaleformCallbacks.h"
#include "skse/ScaleformAPI.h"
#include "skse/ScaleformVM.h"
#include "skse/ScaleformMovie.h"
#include "skse/ScaleformTypes.h"
#include "skse/GameMenus.h"

#include "UtilFuncs.h"
//#include "ExtraMenuDefines.h"

#include <sstream>
#include <iomanip>

SInt32 CalcItemId(TESForm * form, BaseExtraList * extraList)
{
	if (!form || !extraList)
		return 0;

	const char * name = extraList->GetDisplayName(form);

	// No name in extra data? Use base form name
	if (!name)
	{
		TESFullName* pFullName = DYNAMIC_CAST(form, TESForm, TESFullName);
		if (pFullName)
			name = pFullName->name.data;
	}

	if (!name)
		return 0;

	return (SInt32)HashUtil::CRC32(name, form->formID & 0x00FFFFFF);
}

EquipData ResolveEquippedObject(Actor * actor, UInt32 weaponSlot, UInt32 slotMask)
{
	EquipData foundData;
	foundData.pForm = NULL;
	foundData.pExtraData = NULL;
	if (!actor)
		return foundData;

	MatchByEquipSlot matcher(actor, weaponSlot, slotMask);
	ExtraContainerChanges* pContainerChanges = static_cast<ExtraContainerChanges*>(actor->extraData.GetByType(kExtraData_ContainerChanges));
	if (pContainerChanges) {
		foundData = pContainerChanges->FindEquipped(matcher, weaponSlot == MatchByEquipSlot::kSlotID_Right || slotMask != 0, weaponSlot == MatchByEquipSlot::kSlotID_Left);
		return foundData;
	}

	return foundData;
}

namespace spis
{
	//come back here if it's giving trouble, might just need to use input string to distinguish between menus (not sure why)
	TESObjectREFR * GetCurrentAccessedContainer(UInt32 nth)
	{
		MenuManager * mm = MenuManager::GetSingleton();
		UIStringHolder * sh = UIStringHolder::GetSingleton();
		if (mm->IsMenuOpen(&sh->inventoryMenu))
		{
			InventoryMenu * im = static_cast<InventoryMenu*>(mm->GetMenu(&sh->inventoryMenu));
			StandardItemData * sd = nullptr;
			im->itemData->items.GetNthItem(nth, sd);
			TESObjectREFR * container = nullptr;
			LookupREFRByHandle(&sd->OrginRefHandle, &container);
			return container;
		}
		else if (mm->IsMenuOpen(&sh->containerMenu))
		{
			ContainerMenu * cm = static_cast<ContainerMenu*>(mm->GetMenu(&sh->containerMenu));
			StandardItemData * sd = nullptr;
			cm->itemData->items.GetNthItem(nth, sd);
			TESObjectREFR * container = nullptr;
			LookupREFRByHandle(&sd->OrginRefHandle, &container);
			return container;
		}
		else if (mm->IsMenuOpen(&sh->barterMenu))
		{
			BarterMenuExt * bm = static_cast<BarterMenuExt*>(mm->GetMenu(&sh->barterMenu));
			StandardItemData * sd = nullptr;
			bm->itemData->items.GetNthItem(nth, sd);
			TESObjectREFR * container = nullptr;
			LookupREFRByHandle(&sd->OrginRefHandle, &container);
			return container;
		}
		else if (mm->IsMenuOpen(&sh->giftMenu))
		{
			GiftMenu * gm = static_cast<GiftMenu*>(mm->GetMenu(&sh->giftMenu));
			StandardItemData * sd = nullptr;
			gm->itemData->items.GetNthItem(nth, sd);
			TESObjectREFR * container = nullptr;
			LookupREFRByHandle(&sd->OrginRefHandle, &container);
			return container;
		}
		else
		{
			_ERROR("No container found!");
			return nullptr;
		}
	}

	std::string GetValueFromFile(std::ifstream infile, std::string key, std::string path)
	{
		if (!infile.is_open())
		{
			infile.open(path.c_str());
		}
		
		if (infile.is_open())
		{
			std::string outstring;

			while (std::getline(infile, outstring))
			{
				UInt32 pos = outstring.find(key);
				if (pos != std::string::npos)
				{
					pos += key.size() + 2;
					return outstring.substr(pos, outstring.find("\"", pos) - pos);
				}
			}

			if (path != "NULL")
			{
				infile.close();
			}

			_ERROR("No key found in file %s", path.c_str());
			return ("NULL");
		}
		else
		{
			_ERROR("Couldn't find file!");
			return std::string("NULL");
		}
	}

	std::string int_to_hex(UInt32 i)
	{
		std::stringstream stream;
		stream << std::setfill('0') << std::setw(sizeof(UInt32) * 2) << std::hex << i;
		return stream.str();
	}

	PlayerCharacter::ObjDesc * CreateObjDesc(TESForm * type, UInt32 count, BaseExtraList * extraDataList, tList<BaseExtraList> * bel)
	{
		PlayerCharacter::ObjDesc * p = (PlayerCharacter::ObjDesc *)FormHeap_Allocate(sizeof(PlayerCharacter::ObjDesc));
		ASSERT(p);

		new (p)PlayerCharacter::ObjDesc;
		p->form = type;
		p->countDelta = count;
		p->extraData = tList<BaseExtraList>::Create();
		if (bel)
		{
			p->extraData = bel;
		}
		else if (extraDataList)
		{
			p->extraData->Push(extraDataList);
		}

		return p;
	}

	StandardItemData * CreateHookedStandardItemData(GFxMovieView ** view, PlayerCharacter::ObjDesc * od, UInt32 * src)
	{
		StandardItemData * sid = (StandardItemData *)FormHeap_Allocate(sizeof(StandardItemData));
		sid = sid->ctor_Hook(view, od, src);
		return sid;
	}
}