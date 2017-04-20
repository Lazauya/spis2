#ifndef _WEAPONARMORMASKS_H
#define _WEAPONARMORMASKS_H

#include <bitset>

enum //weapon types
{
	kType_HandToHandMelee = 0,
	kType_OneHandSword,
	kType_OneHandDagger,
	kType_OneHandAxe,
	kType_OneHandMace,
	kType_TwoHandSword,
	kType_TwoHandAxe,
	kType_Bow,
	kType_Staff,
	kType_CrossBow,
	kType_H2H,
	kType_1HS,
	kType_1HD,
	kType_1HA,
	kType_1HM,
	kType_2HS,
	kType_2HA,
	kType_Bow2,
	kType_Staff2,
	kType_CBow
};

enum Hands
{
	kHand_Left,
	kHand_Right,
	kHand_Both,
	kHand_None,
	kHand_One
};

enum
{
	kEquipped_Equipped = kHand_One + 1,
	kEquipped_Unequipped,
	kEquipped_Left,
	kEquipped_Right,
	kEquipped_Both
};

UInt8 GetWeaponHand(UInt32 type);
/*{
	switch (type)
	{
	case kType_HandToHandMelee:
		return kHand_None;
	case kType_OneHandSword:
		return kHand_One;
	case kType_OneHandDagger:
		return kHand_One;
	case kType_OneHandAxe:
		return kHand_One;
	case kType_OneHandMace:
		return kHand_One;
	case kType_TwoHandSword:
		return kHand_Both;
	case kType_TwoHandAxe:
		return kHand_Both;
	case kType_Bow:
		return kHand_Both;
	case kType_Staff:
		return kHand_Both;
	case kType_CrossBow:
		return kHand_One;
	case kType_H2H:
		return kHand_None;
	case kType_1HS:
		return kHand_One;
	case kType_1HD:
		return kHand_One;
	case kType_1HA:
		return kHand_One;
	case kType_1HM:
		return kHand_One;
	case kType_2HS:
		return kHand_Both;
	case kType_2HA:
		return kHand_Both;
	case kType_Bow2:
		return kHand_Both;
	case kType_Staff2:
		return kHand_Both;
	case kType_CBow:
		return kHand_Both;
	default:
		return kHand_None;
	}
}*/

enum //armor types
{
	kPart_Head = 1 << 0,
	kPart_Hair = 1 << 1,
	kPart_Body = 1 << 2,
	kPart_Hands = 1 << 3,
	kPart_Forearms = 1 << 4,
	kPart_Amulet = 1 << 5,
	kPart_Ring = 1 << 6,
	kPart_Feet = 1 << 7,
	kPart_Calves = 1 << 8,
	kPart_Shield = 1 << 9,
	kPart_Unnamed10 = 1 << 10,
	kPart_LongHair = 1 << 11,
	kPart_Circlet = 1 << 12,
	kPart_Ears = 1 << 13,
	kPart_Unnamed14 = 1 << 14,
	kPart_Unnamed15 = 1 << 15,
	kPart_Unnamed16 = 1 << 16,
	kPart_Unnamed17 = 1 << 17,
	kPart_Unnamed18 = 1 << 18,
	kPart_Unnamed19 = 1 << 19,
	kPart_Unnamed20 = 1 << 20,
	kPart_Unnamed21 = 1 << 21,
	kPart_Unnamed22 = 1 << 22,
	kPart_Unnamed23 = 1 << 23,
	kPart_Unnamed24 = 1 << 24,
	kPart_Unnamed25 = 1 << 25,
	kPart_Unnamed26 = 1 << 26,
	kPart_Unnamed27 = 1 << 27,
	kPart_Unnamed28 = 1 << 28,
	kPart_Unnamed29 = 1 << 29,
	kPart_Unnamed30 = 1 << 30,
	kPart_FX01 = 1 << 31
};

//gets the "offset" of the bit currently activated
UInt8 GetArmorMaskNumber(UInt32 mask);
/*{
	std::bitset<32> m = mask;
	UInt8 n = 0;
	while (!m[n++]);
	return n;
}*/

#endif