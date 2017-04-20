#ifndef _DURABILITY_H
#define _DURABILITY_H

#include "BasicTracker.h"
//#include "EventSinks.h"
#include "skse/PluginAPI.h"

#define DEFAULT_DURABILITY 100

namespace spis
{
	struct DurabilityPair
	{
		UInt32 maxDurability;
		UInt32 curDurability;

		bool operator<(const DurabilityPair & dp);
		bool operator==(const DurabilityPair & dp);
		DurabilityPair(UInt32 md, UInt32 cd) : maxDurability(md), curDurability(cd) {};
		DurabilityPair(UInt32 formID);
		DurabilityPair() : maxDurability(DEFAULT_DURABILITY), curDurability(DEFAULT_DURABILITY) {};
	};

	class DurabilityTracker : public BasicTracker < DurabilityPair >
	{
	public:
		bool WrapInventory(TESObjectREFR * container);

		bool IncrementDurability(UInt32 amount, UInt32 nth, TESObjectREFR * container, TESObjectREFR * obj, TESForm * baseForm, BaseExtraList * extraData = nullptr);
	};

	bool InitializeTracker(bool force = false);

	//bool RegisterSerializationCallbacks(SKSESerializationInterface * intfc);
	//bool RegisterPapyrusFunctions(/*needs stuff*/);
	//bool RegisterScaleformFunctions(/*needs stuff*/);

	
	static DurabilityTracker * g_DurabilityTracker;
	static bool g_isInit = false;
}

#endif