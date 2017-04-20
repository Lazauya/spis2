#include "GarbageCollection.h"

namespace spis
{
	bool CollectGarbage()
	{
		bool didCollect = false;
		for (auto it = g_DurabilityTracker->cm->begin(); it != g_DurabilityTracker->cm->end(); it++)
		{
			if (it->first.container)
			{
				TESObjectCONT * cont = static_cast<TESObjectCONT*>(it->first.baseForm);
				std::bitset<32> flags = cont->unk64; //flags
				if (flags[10]) //check 10th bit for respawn
				{
					g_DurabilityTracker->cm->erase(it->first);
				}
			}
			didCollect = true;
		}

		return didCollect;
	}
}