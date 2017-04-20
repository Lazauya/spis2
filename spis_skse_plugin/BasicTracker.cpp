#include "BasicTracker.h"
#include "WeaponArmorMasks.h"
#include "EventSinks.h"

#include "skse/GameObjects.h"

#include <bitset>

namespace spis
{
	/*template<class T>
	BasicTracker<T>::BasicTracker()
	{
		gm = new GroundMap();
		cm = new ContainerMap();
		ev = new EquippedVector(34, 0);

		//adding these as standalone, vs13 doesn't support nested classes accessing enclosing nonstatic enclosing class members
		//g_eventContainerChangedDispatcher->AddEventSink(eventContainerChangedHandler);
		//g_eventActorEquipDispatcher->AddEventSink(eventActorEquipHandler);
		//g_eventActivateDispatcher->AddEventSink(eventActivateHandler);
	}*/

	/*template<class T>
	bool BasicTracker<T>::AddEntry(T entry, TESObjectREFR * item, TESForm * baseForm, TESObjectREFR * container, BaseExtraList * extraData)
	{
		extraData = item ? &item->extraData : extraData;
		
		if (container)
		{
			if (cm->count(ContainerKey(container)))
			{
				if ((*cm)[ContainerKey(container)].count(ItemKey(baseForm, extraData)))
				{
					(*cm)[ContainerKey(container)][ItemKey(baseForm, extraData)].push_back(entry);
					return true;
				}
				else
				{
					(*cm)[ContainerKey(container)].insert(std::pair<ItemKey, std::vector<T> >(ItemKey(baseForm, extraData), std::vector<T>(0)));
					(*cm)[ContainerKey(container)][ItemKey(baseForm, extraData)].push_back(entry);
					return true;
				}
			}
			else
			{
				std::unordered_map<ItemKey, std::vector<T>, ItemKeyHash> tm;
				tm.insert(std::pair<ItemKey, std::vector<T> >(ItemKey(baseForm, extraData), std::vector<T>(0)));
				cm->insert(std::pair<ContainerKey, std::unordered_map<ItemKey, std::vector<T>, ItemKeyHash> >(ContainerKey(container), tm));
				//(*cm)[ContainerKey(container)].insert(std::pair<ItemKey, std::vector<T> >(ItemKey(baseForm, item), std::vector<T>(0)));
				(*cm)[ContainerKey(container)][ItemKey(baseForm, extraData)].push_back(entry);
				return true;
			} 
			//cm->insert(ContainerKey(container));
			//(*cm)[ContainerKey(container)].insert(std::pair<ItemKey, std::vector<T> >(ItemKey(baseForm, extraData), std::vector<T>(0)));
			//(*cm)[ContainerKey(container)][ItemKey(baseForm, extraData)].push_back(entry);
			return true;
		}
		else
		{
			if (gm->count(GroundKey(item)))
			{
				return false;
			}
			else
			{
				gm->insert(std::pair<GroundKey, T>(GroundKey(item), entry));
				return true;
			}
		}
	};*/

	/*template<class T>
	UInt32 BasicTracker<T>::GetExact(T entry, TESObjectREFR * item, TESObjectREFR * container, TESForm * baseForm, UInt32 nth, BaseExtraList * extraData)
	{
		nth = nth == NULL_POSITION ? 0 : nth;
		extraData = item ? &item->extraData : extraData;

		if (container)
		{
			if (cm->count(ContainerKey(container)))
			{
				if ((*cm)[ContainerKey(container)].count(ItemKey(baseForm, extraData)))
				{
					if ((*cm)[ContainerKey(container)][ItemKey(baseForm, extraData)].size() <= nth)
					{
						UInt32 n = 0;
						UInt32 nf = 0;

						while ((*cm)[ContainerKey(container)][ItemKey(baseForm, extraData)].size() != n++)
						{
							if ((*cm)[ContainerKey(container)][ItemKey(baseForm, extraData)][n] == entry)
							{
								if (nth == nf++)
								{
									return n;
								}
							}
						}

						return NULL_POSITION;
					}
				}
			}
		}
		else
		{
			return NULL_POSITION;
		}
	}*/

	/*template<class T>
	T * BasicTracker<T>::GetNth(UInt32 nth, TESObjectREFR * item, TESForm * baseForm, TESObjectREFR * container, BaseExtraList * extraData)
	{
		extraData = item ? &item->extraData : extraData;

		if (container)
		{
			if (cm->count(ContainerKey(container)))
			{
				if ((*cm)[ContainerKey(container)].count(ItemKey(baseForm, extraData)))
				{
					if ((*cm)[ContainerKey(container)][ItemKey(baseForm, extraData)].size() >= nth)
					{
						return &(*cm)[ContainerKey(container)][ItemKey(baseForm, extraData)][nth];
					}
				}
			}
			return nullptr;
		}
		else if (gm->count(GroundKey(item)))
		{
			return &(*gm)[GroundKey(item)];
		}
		else
		{
			return nullptr;
		}
	}*/

	/*template<class T>
	UInt32 BasicTracker<T>::GetLowest(TESObjectREFR * item, TESObjectREFR * container, TESForm * baseForm, UInt32 nth, BaseExtraList * extraData)
	{
		nth = nth == NULL_POSITION ? 0 : nth;
		extraData = item ? &item->extraData : extraData;

		if (container)
		{
			if (cm->count(ContainerKey(container)))
			{
				if ((*cm)[ContainerKey(container)].count(ItemKey(baseForm, extraData)))
				{
					if (nth != NULL_POSITION)
					{
						return GetExact((*cm)[ContainerKey(container)][ItemKey(baseForm, extraData)][GetLowest(item, container, baseForm)], item, container, baseForm, nth);
					}

					UInt32 n = 0;
					UInt32 nf = 0;
					while (n++ != (*cm)[ContainerKey(container)][ItemKey(baseForm, extraData)].size())
					{
						nf = (*cm)[ContainerKey(container)][ItemKey(baseForm, extraData)][n] < (*cm)[ContainerKey(container)][ItemKey(baseForm, extraData)][nf] ? n : nf;
					}
					return nf;
				}
			}
		}
		else
		{
			return NULL_POSITION;
		}
	}*/

	/*template<class T>
	UInt32 BasicTracker<T>::GetHighest(TESObjectREFR * item, TESObjectREFR * container, TESForm * baseForm, UInt32 nth, BaseExtraList * extraData)
	{
		nth = nth == NULL_POSITION ? 0 : nth;
		extraData = item ? &item->extraData : extraData;

		if (container)
		{
			if (cm->count(ContainerKey(container)))
			{
				if ((*cm)[ContainerKey(container)].count(ItemKey(baseForm, extraData)))
				{
					if (nth != NULL_POSITION)
					{
						return GetExact((*cm)[ContainerKey(container)][ItemKey(baseForm, extraData)][GetHighest(item, container, baseForm)], item, container, baseForm, nth);
					}

					UInt32 n = 0;
					UInt32 nf = 0;
					while ((*cm)[ContainerKey(container)][ItemKey(baseForm, extraData)].size() != n++)
					{
						nf = (*cm)[ContainerKey(container)][ItemKey(baseForm, extraData)][n] > (*cm)[ContainerKey(container)][ItemKey(baseForm, extraData)][nf] ? n : nf;
					}
					return nf;
				}
			}
		}
		else
		{
			return NULL_POSITION;
		}
	}*/

	/*template<class T>
	bool BasicTracker<T>::RemoveExact(T entry, TESObjectREFR * item, TESForm * baseForm, TESObjectREFR * container, UInt32 nth, BaseExtraList * extraData)
	{
		nth = nth == NULL_POSITION ? 0 : nth;
		extraData = item ? &item->extraData : extraData;

		if (container)
		{
			if (cm->count(ContainerKey(container)))
			{
				if ((*cm)[ContainerKey(container)].count(ItemKey(baseForm, extraData)))
				{
					UInt32 n = 0;
					UInt32 nf = 0;
					while ((*cm)[ContainerKey(container)][ItemKey(baseForm, extraData)].size() != n++)
					{
						if ((*cm)[ContainerKey(container)][ItemKey(baseForm, extraData)][n] == entry)
						{
							if (nth = nf++)
							{
								
								(*cm)[ContainerKey(container)][ItemKey(baseForm, extraData)].erase((*cm)[ContainerKey(container)][ItemKey(baseForm, extraData)].begin()+n);
								return true;
							}
						}
					}
					return false;
				}
			}
		}
		else if (item)
		{
			gm->erase(GroundKey(item));
			return true;
		}
	}*/

	/*template<class T>
	bool BasicTracker<T>::RemoveNth(UInt32 nth, TESObjectREFR * item, TESObjectREFR * container, TESForm * baseForm, BaseExtraList * extraData)
	{
		extraData = item ? &item->extraData : extraData;

		if (container)
		{
			if (cm->count(ContainerKey(container)))
			{
				if ((*cm)[ContainerKey(container)].count(ItemKey(baseForm, extraData)))
				{
					(*cm)[ContainerKey(container)][ItemKey(baseForm, extraData)].erase((*cm)[ContainerKey(container)][ItemKey(baseForm, extraData)].begin()+nth);
					return false;
				}
			}
		}
	}*/

	/*template<class T>
	bool BasicTracker<T>::RemoveLowest(TESObjectREFR * item, TESObjectREFR * container, TESForm * baseForm, UInt32 nth, BaseExtraList * extraData)
	{
		extraData = item ? &item->extraData : extraData;

		if (container)
		{
			UInt32 pos = GetLowest(item, container, baseForm, nth);
			if (pos != NULL_POSITION)
			{
				(*cm)[ContainerKey(container)][ItemKey(baseForm, extraData)].erase((*cm)[ContainerKey(container)][ItemKey(baseForm, extraData)].begin()+pos);
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}*/

	/*template<class T>
	bool BasicTracker<T>::RemoveHighest(TESObjectREFR * item, TESObjectREFR * container, TESForm * baseForm, UInt32 nth, BaseExtraList * extraData)
	{
		extraData = item ? &item->extraData : extraData;

		if (container)
		{
			UInt32 pos = GetHighest(item, container, baseForm, nth);
			if (pos != NULL_POSITION)
			{
				(*cm)[ContainerKey(container)][ItemKey(baseForm, extraData)].erase((*cm)[ContainerKey(container)][ItemKey(baseForm, extraData)].begin()pos);
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}*/

	/*template<class T>
	bool BasicTracker<T>::SetCurrentEquipped(UInt32 mask, UInt32 pos)
	{
		if (mask < 34)
		{
			(*ev)[mask] == pos ? return false : (*ev)[mask] = pos;
			return true;
		}
		return false;
	}*/

}