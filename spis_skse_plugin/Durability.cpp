#include "Durability.h"
#include "skse/GameExtraData.h"

namespace spis
{
	bool InitializeTracker(bool force)
	{
		if (force || !g_isInit)
		{
			g_DurabilityTracker = new DurabilityTracker;

			g_isInit = true;

			return g_isInit;
		}
		return false;
	}

	bool DurabilityPair::operator<(const DurabilityPair & dp)
	{
		return (this->curDurability < dp.curDurability);
	}

	bool DurabilityPair::operator==(const DurabilityPair & dp)
	{
		return (this->curDurability == dp.curDurability) && (this->maxDurability == dp.maxDurability);
	}

	DurabilityPair::DurabilityPair(UInt32 formID)
	{
		maxDurability = std::stoi(GetValueFromFile(std::ifstream("\\data\\SKSE\\plugins\\SPIS\\durabilities.txt"), std::string("\"").append(int_to_hex(formID).append("\""))));
		curDurability = maxDurability;
	}

	bool DurabilityTracker::WrapInventory(TESObjectREFR * container)
	{
		if (!container)
		{
			return false;
		}

		if (cm->count(ContainerKey(container))) //if it already has things in here, return false
		{
			return false;
		}

		TESContainer * cont = DYNAMIC_CAST(container, TESObjectREFR, TESContainer);

		std::ifstream durabilityFile("\\data\\SKSE\\plugins\\SPIS\\durabilities.txt");
		if (durabilityFile.is_open())
		{
			return false;
		}

		ExtraContainerChanges * ecc = static_cast<ExtraContainerChanges*>(container->extraData.GetByType(kExtraData_ContainerChanges));

		for (UInt32 i = 0; i < cont->numEntries; i++)
		{
			ExtraContainerChanges::EntryData * curEntry = ecc->data->FindItemEntry(cont->entries[i]->form);

			UInt32 count = SInt32(cont->entries[i]->count) + curEntry->countDelta > 0 ? SInt32(cont->entries[i]->count) + curEntry->countDelta : 0;

			if (count && (cont->entries[i]->form->IsWeapon() || cont->entries[i]->form->IsArmor()))
			{
				UInt32 paired = ecc->data->FindItemEntry(cont->entries[i]->form)->extendDataList->Count();
				UInt32 unpaired = count - paired;

				for (UInt32 j = 0; j < paired; j++)
				{
					AddEntry(DurabilityPair(cont->entries[i]->form->formID), nullptr, cont->entries[i]->form, container, ecc->data->FindItemEntry(cont->entries[i]->form)->extendDataList->GetNthItem(j));
				}

				for (UInt32 j = 0; j < unpaired; j++)
				{
					AddEntry(DurabilityPair(cont->entries[i]->form->formID), nullptr, cont->entries[i]->form, container, nullptr);
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
					AddEntry(DurabilityPair(ecc->data->objList->GetNthItem(i)->type->formID), nullptr, ecc->data->objList->GetNthItem(i)->type, container, ecc->data->objList->GetNthItem(i)->extendDataList->GetNthItem(j));
				}
				for (UInt32 j = 0; j < unpaired; j++)
				{
					AddEntry(DurabilityPair(ecc->data->objList->GetNthItem(i)->type->formID), nullptr, ecc->data->objList->GetNthItem(i)->type, container, nullptr);
				}
			}
		}

		return true;
	}

	bool DurabilityTracker::IncrementDurability(UInt32 amount, UInt32 nth, TESObjectREFR * container, TESObjectREFR * obj, TESForm * baseForm, BaseExtraList * extraData)
	{
		DurabilityPair * dp = GetNth(nth, obj, baseForm, container, extraData);
		SInt32((*dp).curDurability) + amount > 0 ? (*dp).curDurability += amount : 0;
		return dp ? true : false;
	}
}