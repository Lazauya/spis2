#ifndef _UTILFUNCS_H
#define _UTILFUNCS_H

#include "skse/HashUtil.h"
#include "skse/GameForms.h"
#include "skse/GameBSExtraData.h"
#include "skse/GameExtraData.h"
#include "skse/GameTypes.h"
#include "skse/GameRTTI.h"
#include "skse/GameReferences.h"
#include "skse/ScaleformMovie.h"

#include "ExtraMenuDefines.h"

#include <string>
#include <fstream>

#define NULL_VALUE = 0xFFFFFFFF

//from PapyrusActor.cpp
SInt32 CalcItemId(TESForm * form, BaseExtraList * extraList);
//from PapyrusWornObject.cpp
class MatchByEquipSlot : public FormMatcher
{
	UInt32 m_mask;
	UInt32 m_hand;
	Actor * m_actor;
public:
	MatchByEquipSlot(Actor * actor, UInt32 hand, UInt32 slot) :
		m_hand(hand),
		m_mask(slot),
		m_actor(actor)
	{

	}

	enum
	{
		kSlotID_Left = 0,
		kSlotID_Right
	};

	bool Matches(TESForm* pForm) const {
		if (pForm) {
			if (pForm->formType != TESObjectWEAP::kTypeID) { // If not a weapon use mask
				BGSBipedObjectForm* pBip = DYNAMIC_CAST(pForm, TESForm, BGSBipedObjectForm);
				if (pBip)
					return (pBip->data.parts & m_mask) != 0;
			}
			else if (m_mask == 0) { // Use hand if no mask specified
				TESForm * equippedForm = m_actor->GetEquippedObject(m_hand == kSlotID_Left);
				return (equippedForm && equippedForm == pForm);
			}
		}
		return false;
	}
};

//for TESContainer::Find
class Matcher
{
public:
	TESForm * form;
	Matcher(TESForm * f) : form(f) {};

	bool Accept(TESContainer::Entry* en) const
	{
		return en->form == form;
	}
};

typedef ExtraContainerChanges::FoundEquipData EquipData;
EquipData ResolveEquippedObject(Actor * actor, UInt32 weaponSlot, UInt32 slotMask);

namespace spis
{
	TESObjectREFR * GetCurrentAccessedContainer(UInt32 nth = 0);
	std::string GetValueFromFile(std::ifstream infile, std::string key, std::string path = "NULL");
	std::string int_to_hex(UInt32 i);

	PlayerCharacter::ObjDesc * CreateObjDesc(TESForm * type, UInt32 count, BaseExtraList * extraDataList = nullptr, tList<BaseExtraList> * bel = nullptr);
	StandardItemData * CreateHookedStandardItemData(GFxMovieView ** view, PlayerCharacter::ObjDesc * od, UInt32 * src);
}
#endif