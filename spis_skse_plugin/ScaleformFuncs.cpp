#include "ScaleformFuncs.h"
#include "skse/GameExtraData.h"
#include "skse/GameAPI.h"
#include "skse/PapyrusForm.h"
#include "skse/GameData.h"
#include "WeaponArmorMasks.h"
#include <algorithm>
#include <vector>
#include <string>

namespace spis
{
	//for unstacking
	bool sidSortFunction(StandardItemData* _sid, StandardItemData * __sid)
	{
		std::string _name = _sid->GetName();
		std::string __name = _sid->GetName();

		for (UInt32 i = 0; i < (_name.size() > __name.size() ? __name.size() : _name.size()); i++)
		{
			if (_name.at(i) > __name.at(i))
			{
				return true;
			}
			else if (_name.at(i) < __name.at(i))
			{
				return false;
			}
		}

		return _name.size() > __name.size() ? true : false;
	}

	tArray<StandardItemData*> GetCompleteContainerContents(GFxMovieView ** view, bool sorted)
	{
		//container contents
		TESObjectREFR * container = GetCurrentAccessedContainer(0);
		TESObjectREFR * player = static_cast<TESObjectREFR*>(*g_thePlayer);

		UInt32 cngrh = container->CreateRefHandle();

		std::vector<StandardItemData*> sidvec;

		TESContainer * cont = DYNAMIC_CAST(container->baseForm, TESForm, TESContainer);

		ExtraContainerChanges * ecc = static_cast<ExtraContainerChanges*>(container->extraData.GetByType(kExtraData_ContainerChanges));

		for (UInt32 i = 0; i < cont->numEntries; i++)
		{
			ExtraContainerChanges::EntryData * curEntry = ecc->data->FindItemEntry(cont->entries[i]->form);
			UInt32 count = SInt32(cont->entries[i]->count) + (curEntry ? curEntry->countDelta : 0) > 0 ? SInt32(cont->entries[i]->count) + (curEntry ? curEntry->countDelta : 0) : 0;

			if (cont->entries[i]->form->IsArmor() || cont->entries[i]->form->IsWeapon())
			{
				if (count)
				{
					UInt32 paired = curEntry ? curEntry->extendDataList->Count() : 0;
					UInt32 unpaired = count - paired;

					for (UInt32 j = 0; j < paired; j++)
					{
						PlayerCharacter::ObjDesc * od = CreateObjDesc(cont->entries[i]->form, 1, curEntry->extendDataList->GetNthItem(j));
						StandardItemData * sid = CreateHookedStandardItemData(view, od, &cngrh);
						RegisterNumber(&sid->fxValue, "nth", double(j)); //add this for later
						sidvec.push_back(sid);
					}

					for (UInt32 j = 0; j < unpaired; j++)
					{
						PlayerCharacter::ObjDesc * od = CreateObjDesc(cont->entries[i]->form, 1, nullptr);
						StandardItemData * sid = CreateHookedStandardItemData(view, od, &cngrh);
						RegisterNumber(&sid->fxValue, "nth", double(j));
						sidvec.push_back(sid);
					}
				}
			}
			else
			{
				if (count)
				{
					PlayerCharacter::ObjDesc * od = CreateObjDesc(cont->entries[i]->form, count, nullptr, curEntry ? curEntry->extendDataList : nullptr);
					StandardItemData * sid = CreateHookedStandardItemData(view, od, &cngrh);
					RegisterNumber(&sid->fxValue, "nth", double(0));
					sidvec.push_back(sid);
				}
			}
		}

		for (UInt32 i = 0; i < ecc->data->objList->Count(); i++)
		{
			Matcher mat(ecc->data->objList->GetNthItem(i)->type);
			if (!cont->Find<Matcher>(mat) && (ecc->data->objList->GetNthItem(i)->type->IsWeapon() || ecc->data->objList->GetNthItem(i)->type->IsArmor()))
			{
				UInt32 paired = UInt32(ecc->data->objList->GetNthItem(i)->extendDataList->Count()) > 0 ? UInt32(ecc->data->objList->GetNthItem(i)->extendDataList->Count()) : 0;
				UInt32 unpaired = (ecc->data->objList->GetNthItem(i)->countDelta - paired) > 0 ? ecc->data->objList->GetNthItem(i)->countDelta - paired : 0;
				for (UInt32 j = 0; j < paired; j++)
				{
					PlayerCharacter::ObjDesc * od = CreateObjDesc(ecc->data->objList->GetNthItem(i)->type, 1, ecc->data->objList->GetNthItem(i)->extendDataList->GetNthItem(j));
					StandardItemData * sid = CreateHookedStandardItemData(view, od, &cngrh);
					RegisterNumber(&sid->fxValue, "nth", double(j));
					sidvec.push_back(sid);
				}
				for (UInt32 j = 0; j < unpaired; j++)
				{

					PlayerCharacter::ObjDesc * od = CreateObjDesc(ecc->data->objList->GetNthItem(i)->type, 1, nullptr);
					StandardItemData * sid = CreateHookedStandardItemData(view, od, &cngrh);
					RegisterNumber(&sid->fxValue, "nth", double(j));
					sidvec.push_back(sid);
				}
			}
			else if (!cont->Find<Matcher>(mat))
			{
				PlayerCharacter::ObjDesc * od = CreateObjDesc(ecc->data->objList->GetNthItem(i)->type, ecc->data->objList->GetNthItem(i)->countDelta, nullptr, ecc->data->objList->GetNthItem(i)->extendDataList->Count() ? ecc->data->objList->GetNthItem(i)->extendDataList : nullptr);
				StandardItemData * sid = CreateHookedStandardItemData(view, od, &cngrh);
				RegisterNumber(&sid->fxValue, "nth", double(0));
				sidvec.push_back(sid);
			}
		}

		//get player items in vector
		if (container != player)
		{
			container = player; //do this so i can cheekily reuse the code

			TESContainer * cont = DYNAMIC_CAST(container->baseForm, TESForm, TESContainer);

			ExtraContainerChanges * ecc = static_cast<ExtraContainerChanges*>(container->extraData.GetByType(kExtraData_ContainerChanges));

			for (UInt32 i = 0; i < cont->numEntries; i++)
			{
				ExtraContainerChanges::EntryData * curEntry = ecc->data->FindItemEntry(cont->entries[i]->form);
				UInt32 count = SInt32(cont->entries[i]->count) + (curEntry ? curEntry->countDelta : 0) > 0 ? SInt32(cont->entries[i]->count) + (curEntry ? curEntry->countDelta : 0) : 0;

				if (cont->entries[i]->form->IsArmor() || cont->entries[i]->form->IsWeapon())
				{
					if (count)
					{
						UInt32 paired = curEntry ? curEntry->extendDataList->Count() : 0;
						UInt32 unpaired = count - paired;

						for (UInt32 j = 0; j < paired; j++)
						{
							PlayerCharacter::ObjDesc * od = CreateObjDesc(cont->entries[i]->form, 1, curEntry->extendDataList->GetNthItem(j));
							StandardItemData * sid = CreateHookedStandardItemData(view, od, &cngrh);
							RegisterNumber(&sid->fxValue, "nth", double(j));
							sidvec.push_back(sid);
						}

						for (UInt32 j = 0; j < unpaired; j++)
						{
							PlayerCharacter::ObjDesc * od = CreateObjDesc(cont->entries[i]->form, 1, nullptr);
							StandardItemData * sid = CreateHookedStandardItemData(view, od, &cngrh);
							RegisterNumber(&sid->fxValue, "nth", double(j));
							sidvec.push_back(sid);
						}
					}
				}
				else
				{
					if (count)
					{
						PlayerCharacter::ObjDesc * od = CreateObjDesc(cont->entries[i]->form, count, nullptr, curEntry ? curEntry->extendDataList : nullptr);
						StandardItemData * sid = CreateHookedStandardItemData(view, od, &cngrh);
						RegisterNumber(&sid->fxValue, "nth", double(0));
						sidvec.push_back(sid);
					}
				}
			}

			for (UInt32 i = 0; i < ecc->data->objList->Count(); i++)
			{
				Matcher mat(ecc->data->objList->GetNthItem(i)->type);
				if (!cont->Find<Matcher>(mat) && (ecc->data->objList->GetNthItem(i)->type->IsWeapon() || ecc->data->objList->GetNthItem(i)->type->IsArmor()))
				{
					UInt32 paired = UInt32(ecc->data->objList->GetNthItem(i)->extendDataList->Count()) > 0 ? UInt32(ecc->data->objList->GetNthItem(i)->extendDataList->Count()) : 0;
					UInt32 unpaired = (ecc->data->objList->GetNthItem(i)->countDelta - paired) > 0 ? ecc->data->objList->GetNthItem(i)->countDelta - paired : 0;
					for (UInt32 j = 0; j < paired; j++)
					{
						PlayerCharacter::ObjDesc * od = CreateObjDesc(ecc->data->objList->GetNthItem(i)->type, 1, ecc->data->objList->GetNthItem(i)->extendDataList->GetNthItem(j));
						StandardItemData * sid = CreateHookedStandardItemData(view, od, &cngrh);
						RegisterNumber(&sid->fxValue, "nth", double(j));
						sidvec.push_back(sid);
					}
					for (UInt32 j = 0; j < unpaired; j++)
					{

						PlayerCharacter::ObjDesc * od = CreateObjDesc(ecc->data->objList->GetNthItem(i)->type, 1, nullptr);
						StandardItemData * sid = CreateHookedStandardItemData(view, od, &cngrh);
						RegisterNumber(&sid->fxValue, "nth", double(j));
						sidvec.push_back(sid);
					}
				}
				else if (!cont->Find<Matcher>(mat))
				{
					PlayerCharacter::ObjDesc * od = CreateObjDesc(ecc->data->objList->GetNthItem(i)->type, ecc->data->objList->GetNthItem(i)->countDelta, nullptr, ecc->data->objList->GetNthItem(i)->extendDataList->Count() ? ecc->data->objList->GetNthItem(i)->extendDataList : nullptr);
					StandardItemData * sid = CreateHookedStandardItemData(view, od, &cngrh);
					RegisterNumber(&sid->fxValue, "nth", double(0));
					sidvec.push_back(sid);
				}
			}
		}

		if (sorted)
		{
			std::sort(sidvec.begin(), sidvec.end(), sidSortFunction);
		}

		tArray<StandardItemData*> sidarr;
		if (sidarr.Allocate(sidvec.size()));
		{
			for (UInt32 i = 0; i < sidvec.size(); i++)
			{
				sidarr[i] = sidvec[i];
			}
		}
		return sidarr;
	}

	void SetDurabilities(tArray<StandardItemData*> &sidarr)
	{
		for (UInt32 i = 0; i < sidarr.count; i++)
		{
			GFxValue fx;
			sidarr[i]->fxValue.GetMember("nth", &fx);
			TESObjectREFR * ref = nullptr;
			LookupREFRByHandle(&sidarr[i]->OrginRefHandle, &ref);
			DurabilityPair * dp = g_DurabilityTracker->GetNth(UInt32(fx.GetNumber()), nullptr, sidarr[i]->objDesc->form, ref, sidarr[i]->objDesc->extraData->GetNthItem(0));
			RegisterNumber(&sidarr[i]->fxValue, "maxDurability", double(dp->maxDurability));
			RegisterNumber(&sidarr[i]->fxValue, "curDurability", double(dp->curDurability));
		}
	}

	/*
	0 - none
	1 - equipped
	2 - left
	3 - right
	4 - left and right
	*/
	void SetEquipStates(tArray<StandardItemData*> &sidarr)
	{
		//armors (infcludes shield
		for (UInt32 j = 0; j < g_DurabilityTracker->ev->size() - 2; j++)
		{
			for (UInt32 i = 0; i < sidarr.count; i++)
			{
				GFxValue fx;
				GFxValue _fx;
				sidarr[i]->fxValue.GetMember("nth", &fx);
				_fx.SetNumber(double(1));

				EquipData ed = ResolveEquippedObject(static_cast<Actor*>(*g_thePlayer), kHand_None, 1 << j);
				if ((ed.pForm == sidarr[i]->objDesc->form) && (UInt32(fx.GetNumber()) == (*g_DurabilityTracker->ev)[j]))
				{
					sidarr[i]->fxValue.SetMember("equipState", &_fx);
				}
			}
		}

		//left hand
		for (UInt32 i = 0; i < sidarr.count; i++)
		{
			GFxValue fx;
			GFxValue _fx;
			sidarr[i]->fxValue.GetMember("nth", &fx);
			_fx.SetNumber(double(2));

			EquipData ed = ResolveEquippedObject(static_cast<Actor*>(*g_thePlayer), 0, 0);
			if ((ed.pForm == sidarr[i]->objDesc->form) && (UInt32(fx.GetNumber()) == (*g_DurabilityTracker->ev)[32]))
			{
				sidarr[i]->fxValue.SetMember("equipState", &_fx);
			}
		}
		
		//right hand
		for (UInt32 i = 0; i < sidarr.count; i++)
		{
			GFxValue fx;
			GFxValue _fx;
			sidarr[i]->fxValue.GetMember("nth", &fx);
			_fx.SetNumber(double(3));

			EquipData ed = ResolveEquippedObject(static_cast<Actor*>(*g_thePlayer), 1, 0);
			if ((ed.pForm == sidarr[i]->objDesc->form) && (UInt32(fx.GetNumber()) == (*g_DurabilityTracker->ev)[33]))
			{
				sidarr[i]->fxValue.SetMember("equipState", &_fx);
			}
		}
	}

	void Scaleform_GetDurabilityInfos::Invoke(Args * args)
	{
		MenuManager * mm = MenuManager::GetSingleton();
		UIStringHolder * sh = UIStringHolder::GetSingleton();

		if (mm->IsMenuOpen(&sh->inventoryMenu))
		{
			InventoryMenu * im = static_cast<InventoryMenu*>(mm->GetMenu(&sh->inventoryMenu));
			im->itemData->items = GetCompleteContainerContents(&args->movie);
			SetDurabilities(im->itemData->items);
			SetEquipStates(im->itemData->items);
		}
		else if (mm->IsMenuOpen(&sh->containerMenu))
		{
			ContainerMenu * cm = static_cast<ContainerMenu*>(mm->GetMenu(&sh->containerMenu));
			cm->itemData->items = GetCompleteContainerContents(&args->movie);
			SetDurabilities(cm->itemData->items);
			SetEquipStates(cm->itemData->items);
		}
		else if (mm->IsMenuOpen(&sh->barterMenu))
		{
			BarterMenuExt * bm = static_cast<BarterMenuExt*>(mm->GetMenu(&sh->barterMenu));
			bm->itemData->items = GetCompleteContainerContents(&args->movie);
			SetDurabilities(bm->itemData->items);
			SetEquipStates(bm->itemData->items);
		}
		else if (mm->IsMenuOpen(&sh->giftMenu))
		{
			GiftMenu * gm = static_cast<GiftMenu*>(mm->GetMenu(&sh->giftMenu));
			gm->itemData->items = GetCompleteContainerContents(&args->movie);
			SetDurabilities(gm->itemData->items);
			SetEquipStates(gm->itemData->items);
		}
		else
		{
			_ERROR("No container found!");
		}
	}

	void Scaleform_SetCurrentDurability::Invoke(Args * args)
	{
		MenuManager * mm = MenuManager::GetSingleton();
		UIStringHolder * sh = UIStringHolder::GetSingleton();

		if (mm->IsMenuOpen(&sh->inventoryMenu))
		{
			InventoryMenu * im = static_cast<InventoryMenu*>(mm->GetMenu(&sh->inventoryMenu));
			GFxValue _nth, _max, _cur;
			StandardItemData * sid = im->itemData->GetSelectedItem();
			sid->fxValue.GetMember("nth", &_nth);
			sid->fxValue.GetMember("maxDurability", &_max);
			sid->fxValue.GetMember("curDurability", &_cur);
			g_DurabilityTracker->currentT = DurabilityTracker::CurrentT(
				DurabilityPair(UInt32(_max.GetNumber()), UInt32(_cur.GetNumber())),
				UInt32(_nth.GetNumber()),
				DurabilityTracker::FormREFRPair(sid->objDesc->form->formID, CalcItemId(sid->objDesc->form, sid->objDesc->extraData->GetNthItem(0))),
				sid->OrginRefHandle);
		}
		else if (mm->IsMenuOpen(&sh->containerMenu))
		{
			ContainerMenu * cm = static_cast<ContainerMenu*>(mm->GetMenu(&sh->containerMenu));
			GFxValue _nth, _max, _cur;
			StandardItemData * sid = cm->itemData->GetSelectedItem();
			sid->fxValue.GetMember("nth", &_nth);
			sid->fxValue.GetMember("maxDurability", &_max);
			sid->fxValue.GetMember("curDurability", &_cur);
			g_DurabilityTracker->currentT = DurabilityTracker::CurrentT(
				DurabilityPair(UInt32(_max.GetNumber()), UInt32(_cur.GetNumber())),
				UInt32(_nth.GetNumber()),
				DurabilityTracker::FormREFRPair(sid->objDesc->form->formID, CalcItemId(sid->objDesc->form, sid->objDesc->extraData->GetNthItem(0))),
				sid->OrginRefHandle);
		}
		else if (mm->IsMenuOpen(&sh->barterMenu))
		{
			BarterMenuExt * bm = static_cast<BarterMenuExt*>(mm->GetMenu(&sh->barterMenu));
			GFxValue _nth, _max, _cur;
			StandardItemData * sid = bm->itemData->GetSelectedItem();
			sid->fxValue.GetMember("nth", &_nth);
			sid->fxValue.GetMember("maxDurability", &_max);
			sid->fxValue.GetMember("curDurability", &_cur);
			g_DurabilityTracker->currentT = DurabilityTracker::CurrentT(
				DurabilityPair(UInt32(_max.GetNumber()), UInt32(_cur.GetNumber())),
				UInt32(_nth.GetNumber()),
				DurabilityTracker::FormREFRPair(sid->objDesc->form->formID, CalcItemId(sid->objDesc->form, sid->objDesc->extraData->GetNthItem(0))),
				sid->OrginRefHandle);
		}
		else if (mm->IsMenuOpen(&sh->giftMenu))
		{
			GiftMenu * gm = static_cast<GiftMenu*>(mm->GetMenu(&sh->giftMenu));
			GFxValue _nth, _max, _cur;
			StandardItemData * sid = gm->itemData->GetSelectedItem();
			sid->fxValue.GetMember("nth", &_nth);
			sid->fxValue.GetMember("maxDurability", &_max);
			sid->fxValue.GetMember("curDurability", &_cur);
			g_DurabilityTracker->currentT = DurabilityTracker::CurrentT(
				DurabilityPair(UInt32(_max.GetNumber()), UInt32(_cur.GetNumber())),
				UInt32(_nth.GetNumber()),
				DurabilityTracker::FormREFRPair(sid->objDesc->form->formID, CalcItemId(sid->objDesc->form, sid->objDesc->extraData->GetNthItem(0))),
				sid->OrginRefHandle);
		}
		else
		{
			_ERROR("No container found!");
		}
	}
	
	void Scaleform_SetEquip::Invoke(Args * args)
	{
		ASSERT(args->numArgs == 1);

		MenuManager * mm = MenuManager::GetSingleton();
		UIStringHolder * sh = UIStringHolder::GetSingleton();

		if (mm->IsMenuOpen(&sh->inventoryMenu))
		{
			InventoryMenu * im = static_cast<InventoryMenu*>(mm->GetMenu(&sh->inventoryMenu));

			GFxValue _nth;
			StandardItemData * sid = im->itemData->GetSelectedItem(); 
			sid->fxValue.GetMember("nth", &_nth);

			if (sid->objDesc->form->IsArmor())
			{
				BGSBipedObjectForm * bof = DYNAMIC_CAST(sid->objDesc->form, TESForm, BGSBipedObjectForm);
				if (bof)
				{
					ExtraContainerChanges::FoundEquipData ed = ResolveEquippedObject(static_cast<Actor*>(*g_thePlayer), kHand_None, bof->data.parts);
					if (ed.pForm == sid->objDesc->form)
					{
						(*g_DurabilityTracker->ev)[GetArmorMaskNumber(bof->data.parts)] = UInt32(_nth.GetNumber());
					}
					else
					{
						(*g_DurabilityTracker->ev)[GetArmorMaskNumber(bof->data.parts)] = NULL_POSITION;
					}
				}
				else
				{
					_MESSAGE("Selected item was not casted correctly but was an armor");
				}
			}
			else if (sid->objDesc->form->IsWeapon())
			{
				UInt32 hand = UInt32(args->args[0].GetNumber());
				TESForm * eo = static_cast<Actor*>(*g_thePlayer)->GetEquippedObject(hand == kHand_Left);
				if (eo == sid->objDesc->form)
				{
					(*g_DurabilityTracker->ev)[32+hand] = UInt32(_nth.GetNumber());
				}
				else
				{
					(*g_DurabilityTracker->ev)[32+hand] = NULL_POSITION;
				}
			}
		}
		else if (mm->IsMenuOpen(&sh->containerMenu))
		{
			ContainerMenu * cm = static_cast<ContainerMenu*>(mm->GetMenu(&sh->containerMenu));

			GFxValue _nth;
			StandardItemData * sid = cm->itemData->GetSelectedItem();
			sid->fxValue.GetMember("nth", &_nth);

			if (sid->objDesc->form->IsArmor())
			{
				BGSBipedObjectForm * bof = DYNAMIC_CAST(sid->objDesc->form, TESForm, BGSBipedObjectForm);
				if (bof)
				{
					ExtraContainerChanges::FoundEquipData ed = ResolveEquippedObject(static_cast<Actor*>(*g_thePlayer), kHand_None, bof->data.parts);
					if (ed.pForm == sid->objDesc->form)
					{
						(*g_DurabilityTracker->ev)[GetArmorMaskNumber(bof->data.parts)] = UInt32(_nth.GetNumber());
					}
					else
					{
						(*g_DurabilityTracker->ev)[GetArmorMaskNumber(bof->data.parts)] = NULL_POSITION;
					}
				}
				else
				{
					_MESSAGE("Selected item was not casted correctly but was an armor");
				}
			}
			else if (sid->objDesc->form->IsWeapon())
			{
				UInt32 hand = UInt32(args->args[0].GetNumber());
				TESForm * eo = static_cast<Actor*>(*g_thePlayer)->GetEquippedObject(hand == kHand_Left);
				if (eo == sid->objDesc->form)
				{
					(*g_DurabilityTracker->ev)[32 + hand] = UInt32(_nth.GetNumber());
				}
				else
				{
					(*g_DurabilityTracker->ev)[32 + hand] = NULL_POSITION;
				}
			}
		}
		else if (mm->IsMenuOpen(&sh->barterMenu))
		{
			BarterMenuExt * bm = static_cast<BarterMenuExt*>(mm->GetMenu(&sh->barterMenu));

			GFxValue _nth;
			StandardItemData * sid = bm->itemData->GetSelectedItem();
			sid->fxValue.GetMember("nth", &_nth);

			if (sid->objDesc->form->IsArmor())
			{
				BGSBipedObjectForm * bof = DYNAMIC_CAST(sid->objDesc->form, TESForm, BGSBipedObjectForm);
				if (bof)
				{
					ExtraContainerChanges::FoundEquipData ed = ResolveEquippedObject(static_cast<Actor*>(*g_thePlayer), kHand_None, bof->data.parts);
					if (ed.pForm == sid->objDesc->form)
					{
						(*g_DurabilityTracker->ev)[GetArmorMaskNumber(bof->data.parts)] = UInt32(_nth.GetNumber());
					}
					else
					{
						(*g_DurabilityTracker->ev)[GetArmorMaskNumber(bof->data.parts)] = NULL_POSITION;
					}
				}
				else
				{
					_MESSAGE("Selected item was not casted correctly but was an armor");
				}
			}
			else if (sid->objDesc->form->IsWeapon())
			{
				UInt32 hand = UInt32(args->args[0].GetNumber());
				TESForm * eo = static_cast<Actor*>(*g_thePlayer)->GetEquippedObject(hand == kHand_Left);
				if (eo == sid->objDesc->form)
				{
					(*g_DurabilityTracker->ev)[32 + hand] = UInt32(_nth.GetNumber());
				}
				else
				{
					(*g_DurabilityTracker->ev)[32 + hand] = NULL_POSITION;
				}
			}
		}
		else if (mm->IsMenuOpen(&sh->giftMenu))
		{
			GiftMenu * gm = static_cast<GiftMenu*>(mm->GetMenu(&sh->giftMenu));
			GFxValue _nth;
			StandardItemData * sid = gm->itemData->GetSelectedItem();
			sid->fxValue.GetMember("nth", &_nth);

			if (sid->objDesc->form->IsArmor())
			{
				BGSBipedObjectForm * bof = DYNAMIC_CAST(sid->objDesc->form, TESForm, BGSBipedObjectForm);
				if (bof)
				{
					ExtraContainerChanges::FoundEquipData ed = ResolveEquippedObject(static_cast<Actor*>(*g_thePlayer), kHand_None, bof->data.parts);
					if (ed.pForm == sid->objDesc->form)
					{
						(*g_DurabilityTracker->ev)[GetArmorMaskNumber(bof->data.parts)] = UInt32(_nth.GetNumber());
					}
					else
					{
						(*g_DurabilityTracker->ev)[GetArmorMaskNumber(bof->data.parts)] = NULL_POSITION;
					}
				}
				else
				{
					_MESSAGE("Selected item was not casted correctly but was an armor");
				}
			}
			else if (sid->objDesc->form->IsWeapon())
			{
				UInt32 hand = UInt32(args->args[0].GetNumber());
				TESForm * eo = static_cast<Actor*>(*g_thePlayer)->GetEquippedObject(hand == kHand_Left);
				if (eo == sid->objDesc->form)
				{
					(*g_DurabilityTracker->ev)[32 + hand] = UInt32(_nth.GetNumber());
				}
				else
				{
					(*g_DurabilityTracker->ev)[32 + hand] = NULL_POSITION;
				}
			}
		}
		else
		{
			_ERROR("No container found!");
		}
	}

	void Scaleform_WrapInventory::Invoke(Args * args)
	{
		g_DurabilityTracker->WrapInventory(GetCurrentAccessedContainer());
	}

	bool RegisterScaleformFunctions(GFxMovieView * view, GFxValue * root)
	{
		RegisterFunction <Scaleform_GetDurabilityInfos>(root, view, "GetDurabilityInfos");
		RegisterFunction <Scaleform_SetCurrentDurability>(root, view, "SetCurrentDurability");
		RegisterFunction <Scaleform_SetEquip>(root, view, "SetEquip");
		RegisterFunction <Scaleform_WrapInventory>(root, view, "WrapInventory");
		return true;
	}
}