#include "ExtraMenuDefines.h"

StandardItemData * StandardItemData::ctor_Hook(GFxMovieView ** movieView, PlayerCharacter::ObjDesc * objDesc, UInt32 * src)
{
	StandardItemData	* result = CALL_MEMBER_FN(this, ctor_data)(movieView, objDesc, src);

	//	_MESSAGE("StandardItemData hook");

	//if (s_bExtendData) //commenting this because data is assumed to be extended
	{
		scaleformExtend::CommonItemData(&result->fxValue, objDesc->form);
		scaleformExtend::StandardItemData(&result->fxValue, objDesc->form);
		scaleformExtend::InventoryData(&result->fxValue, *movieView, objDesc);
		// Calling this to set scrolls, potions, ingredients
		// as this function is called for inventory, barter, container
		scaleformExtend::MagicItemData(&result->fxValue, *movieView, objDesc->form, true, false);
	}

	return result;
}