#include "WeaponArmorMasks.h"

UInt8 GetWeaponHand(UInt32 type)
{
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
}

UInt8 GetArmorMaskNumber(UInt32 mask)
{
	std::bitset<32> m = mask;
	UInt8 n = 0;
	while (!m[n++]);
	return n;
}