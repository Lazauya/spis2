#ifndef _SPIS_SCALEFORMFUNCS_H
#define _SPIS_SCALEFORMFUNCS_H

#include "skse/ScaleformAPI.h"
#include "skse/ScaleformCallbacks.h"
#include "skse/ScaleformExtendedData.h"
#include "skse/ScaleformMovie.h"
#include "ExtraMenuDefines.h"

#include "Durability.h"

namespace spis 
{
	tArray<StandardItemData*> GetCompleteContainerContents(GFxMovieView ** view, bool sorted = true);
	void SetDurabilities(tArray<StandardItemData*>& sidarr);

	class Scaleform_GetDurabilityInfos : public GFxFunctionHandler
	{
		virtual void Invoke(Args * args);
	};

	class Scaleform_SetCurrentDurability : public GFxFunctionHandler
	{
		virtual void Invoke(Args * args);
	};

	/*class Scaleform_SetCurrentDurability : public GFxFunctionHandler
	{
		virtual void Invoke(Args * args);
	};*/

	class Scaleform_SetEquip : public GFxFunctionHandler
	{
		virtual void Invoke(Args * args);
	};

	class Scaleform_WrapInventory : public GFxFunctionHandler
	{
		virtual void Invoke(Args * args);
	};

	bool RegisterScaleformFunctions(GFxMovieView * view, GFxValue * root);
}

#endif