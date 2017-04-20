#ifndef _BASICTRACKER_H
#define _BASICTRACKER_H

#include <unordered_map>
#include <string>
#include <utility>
#include <vector>
#include <bitset>

#include "UtilFuncs.h"
//#include "EventSinks.h"

#define NULL_POSITION 0xFFFFFFFF

namespace spis
{
	template<class T>
	class BasicTracker
	{
	public:

		struct GroundKey
		{
			UInt32 hash;
			TESObjectREFR * item;
			UInt32 CalculateHash(TESObjectREFR * itm) const
			{
				//return CalcItemId(itm->baseForm, itm->extraData); //keep this here until further notice; might find out this is still relevant
				return const_cast<TESObjectREFR*>(itm)->CreateRefHandle(); //hashes on ref handle for ground objects so they can be found later
			}

			GroundKey(TESObjectREFR * itm) : item(itm), hash(CalculateHash(itm)) {};
			GroundKey(UInt32 _hash) : item(nullptr), hash(_hash)
			{
				LookupREFRByHandle(&hash, &item);
			};

			bool operator==(const GroundKey & other) const
			{
				return other.hash == this->hash;
			}
		};
		struct GroundKeyHash
		{
			std::size_t operator()(const GroundKey & key) const
			{
				return key.hash;
			}
		};

		struct ContainerKey
		{
			UInt32 hash;
			TESObjectREFR * container;
			TESForm * baseForm; //used so that we can always do garbage collection checks
			UInt32 CalculateHash(TESObjectREFR * cont) const
			{
				return const_cast<TESObjectREFR*>(cont)->CreateRefHandle();
			}

			ContainerKey(TESObjectREFR * cont) : container(cont), hash(CalculateHash(cont)), baseForm(cont->baseForm) {};
			ContainerKey(UInt32 _hash) : container(nullptr), hash(_hash)
			{
				LookupREFRByHandle(&hash, &container);
				if (container)
				{
					baseForm = container->baseForm;
				}
			};

			bool operator==(const ContainerKey & other) const
			{
				return other.hash == this->hash;
			}
		};
		struct ContainerKeyHash
		{
			std::size_t operator()(const ContainerKey & key) const
			{
				return key.hash;
			}
		};

		//checks if the obj to be hashed is persistent. if it is, hash on obref, if not, hash on form
		typedef std::pair<UInt32, UInt32> FormREFRPair; //outdated name, now a actually using item id and not handle

		struct ItemKey
		{
			FormREFRPair p;

			ItemKey(TESForm * f, TESObjectREFR * o) { p.first = f ? f->formID : 0; p.second = o ? CalcItemId(f, &o->extraData) : 0; };
			ItemKey(TESForm * f, BaseExtraList * b)	{ p.first = f ? f->formID : 0; p.second = b ? CalcItemId(f, b) : 0; };
			ItemKey(UInt32 formID, UInt32 itemID)   { p.first = formID; p.second = itemID; };
			ItemKey(FormREFRPair _p) : p(_p) {};

			void SetHash(UInt32 itemID, UInt32 formID)
			{
				p.first = formID;
				p.second = itemID;
			}

			bool operator==(const ItemKey & key) const
			{
				return ((key.p.first == this->p.first) && (key.p.second > 0)) || ((key.p.second == this->p.second) && (key.p.first > 0));
			}
		};
		struct ItemKeyHash
		{
			std::size_t operator()(const ItemKey & key) const
			{
				return key.p.second > 0 ? key.p.second : key.p.first;
			}
		};

		typedef std::unordered_map<GroundKey, T, GroundKeyHash> GroundMap;
		typedef std::unordered_map<ContainerKey, std::unordered_map<ItemKey, std::vector<T>, ItemKeyHash>, ContainerKeyHash> ContainerMap;
		typedef std::vector<UInt32> EquippedVector; //remembers the pos of t in container map of currently equiped weapon

		//T, nth, form/ref pair, container from (attached in post)
		typedef std::tuple < T, UInt32, FormREFRPair, UInt32> CurrentT;

		GroundMap * gm;
		ContainerMap * cm;
		EquippedVector * ev; //last 2 are right and left hands respectively; all others follow armor part masks in order
		
		CurrentT currentT; //obj to be used by menu selection
		std::pair<UInt32, UInt32> currentEquip; //first ist pos, 2nd is hand (0 = right, 1 = left)

		//i apparently can't define template class members outside of class
		//fml
		BasicTracker() //add event sinks for activate and container change here
		{
			gm = new GroundMap();
			cm = new ContainerMap();
			ev = new EquippedVector(34, 0);

			//adding these as standalone, vs13 doesn't support nested classes accessing enclosing nonstatic enclosing class members
			//g_eventContainerChangedDispatcher->AddEventSink(eventContainerChangedHandler);
			//g_eventActorEquipDispatcher->AddEventSink(eventActorEquipHandler);
			//g_eventActivateDispatcher->AddEventSink(eventActivateHandler);
		}

		bool AddEntry(T entry, TESObjectREFR * item, TESForm * baseForm = nullptr, TESObjectREFR * container = nullptr, BaseExtraList * extraData = nullptr)
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
		};

		UInt32 GetExact(T entry, TESObjectREFR * item, TESObjectREFR * container, TESForm * baseForm = nullptr, UInt32 nth = NULL_POSITION, BaseExtraList * extraData = nullptr)
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
		}

		T * GetNth(UInt32 nth, TESObjectREFR * item, TESForm * baseForm = nullptr, TESObjectREFR * container = nullptr, BaseExtraList * extraData = nullptr)
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
		}
		UInt32 GetLowest(TESObjectREFR * item, TESObjectREFR * container, TESForm * baseForm = nullptr, UInt32 nth = NULL_POSITION, BaseExtraList * extraData = nullptr)
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
		}
		UInt32 GetHighest(TESObjectREFR * item, TESObjectREFR * container, TESForm * baseForm = nullptr, UInt32 nth = NULL_POSITION, BaseExtraList * extraData = nullptr)
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
		}
		
		bool RemoveExact(T entry, TESObjectREFR * item, TESForm * baseForm = nullptr, TESObjectREFR * container = nullptr, UInt32 nth = NULL_POSITION, BaseExtraList * extraData = nullptr)
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

									(*cm)[ContainerKey(container)][ItemKey(baseForm, extraData)].erase((*cm)[ContainerKey(container)][ItemKey(baseForm, extraData)].begin() + n);
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
		}
		bool RemoveNth(UInt32 nth, TESObjectREFR * item, TESObjectREFR * container, TESForm * baseForm = nullptr, BaseExtraList * extraData = nullptr)
		{
			extraData = item ? &item->extraData : extraData;

			if (container)
			{
				if (cm->count(ContainerKey(container)))
				{
					if ((*cm)[ContainerKey(container)].count(ItemKey(baseForm, extraData)))
					{
						(*cm)[ContainerKey(container)][ItemKey(baseForm, extraData)].erase((*cm)[ContainerKey(container)][ItemKey(baseForm, extraData)].begin() + nth);
						return false;
					}
				}
			}
		}
		bool RemoveLowest(TESObjectREFR * item, TESObjectREFR * container, TESForm * baseForm = nullptr, UInt32 nth = NULL_POSITION, BaseExtraList * extraData = nullptr)
		{
			extraData = item ? &item->extraData : extraData;

			if (container)
			{
				UInt32 pos = GetLowest(item, container, baseForm, nth);
				if (pos != NULL_POSITION)
				{
					(*cm)[ContainerKey(container)][ItemKey(baseForm, extraData)].erase((*cm)[ContainerKey(container)][ItemKey(baseForm, extraData)].begin() + pos);
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
		}
		bool RemoveHighest(TESObjectREFR * item, TESObjectREFR * container, TESForm * baseForm = nullptr, UInt32 nth = NULL_POSITION, BaseExtraList * extraData = nullptr)
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
		}

		//virtual bool WrapInventory(TESObjectREFR * container) = 0;
		
		//bool WrapEquipped(Actor * actor);

		bool SetCurrentEquipped(UInt32 mask, UInt32 pos)
		{
			if (mask < 34)
			{
				(*ev)[mask] == pos ? return false : (*ev)[mask] = pos;
				return true;
			}
			return false;
		}

		//event stuffs, needs to be placed after everything else
		/*
		class ContainerChangedEventHandler : public BSTEventSink < TESContainerChangedEvent >
		{
		public:
			virtual EventResult	ReceiveEvent(TESContainerChangedEvent * evn, EventDispatcher<TESContainerChangedEvent> * dispatcher) override
			{
				//if () ??? check for weapon/armor?
				{
					TESObjectREFR * cnt = nullptr;
					LookupREFRByHandle(&evn->to, &cnt);
					TESObjectREFR * frm = nullptr;
					LookupREFRByHandle(&evn->from, &frm);

					if ((std::get<2>(currentT).second ? std::get<2>(currentT).second : std::get<2>(currentT).first))
					{
						if (std::get<2>(currentT).second && std::get<3>(currentT) == evn->from)
						{
							TESObjectREFR * obj = nullptr;
							LookupREFRByHandle(&(std::get<2>(currentT).second), &obj);

							AddEntry(std::get<0>(currentT), obj, obj->baseForm, cnt);
							RemoveExact(std::get<0>(currentT), obj, obj->baseForm, frm, std::get<1>(currentT));

							std::get<1>(currentT) = 0;
							std::get<2>(currentT).first = 0;
							std::get<2>(currentT).second = 0;
							return kEvent_Continue;
						}
						else if (std::get<2>(currentT).first && std::get<3>(currentT) == evn->from)
						{
							AddEntry<T>(std::get<0>(currentT), nullptr, LookupFormByID(std::get<2>(currentT).first), cnt);
							RemoveExact<T>(std::get<0>(currentT), nullptr, LookupFormByID(std::get<2>(currentT).first), frm, std::get<1>(currentT));

							std::get<1>(currentT) = 0;
							std::get<2>(currentT).first = 0;
							std::get<2>(currentT).second = 0;
							return kEvent_Continue;
						}
					}

					if (!evn->to && evn->from) //operate when dropped, 
					{
						TESObjectREFR * obj = nullptr;
						LookupREFRByHandle(&evn->item, &obj);
						if (!obj)
						{
							LookupREFRByHandle(&evn->item, &obj);
						}

						UInt32 l = GetLowest(obj, frm, LookupFormByID(evn->itemBaseForm));
						if (l != NULL_POSITION)
						{
							T lowest = (*cm)[ContainerKey(frm)][ItemKey(LookupFormByID(evn->itemBaseForm), obj)][l];
							AddEntry(lowest, obj, LookupFormByID(evn->itemBaseForm));
						}

						RemoveLowest(obj, frm, LookupFormByID(evn->itemBaseForm));
					}

					if (!evn->from) //only operate when obj is persistent
					{
						TESObjectREFR * obj = nullptr;
						LookupREFRByHandle(&evn->item, &obj);

						if (obj)
						{
							T t = (*gm)[GroundKey(obj)];
							RemoveExact(t, obj, obj->baseForm);
							AddEntry(t, obj, obj->baseForm, cnt);
						}
					}
				}
			}
		};

		class ActorEquipEventHandler : public BSTEventSink < TESEquipEvent > //more research on event required
		{
		public:
			virtual	EventResult ReceiveEvent(TESEquipEvent * evn, EventDispatcher<TESEquipEvent> * dispatcher) override
			{
				std::bitset<32> eq = evn->unk;
				if (unk[17])
				{
					if (currentEquip.first != NULL_POSITION)
					{
						if (LookupFormByID(evn->formID)->IsWeapon())
						{
							//TESObjectWEAP * weap = DYNAMIC_CAST(LookupFormByID(evn->formID), TESForm, TESObjectWEAP);

							SetCurrentEquipped(32 + currentEquip.second, currentEquip.first);
							return kEvent_Continue;
						}
						else if (LookupFormByID(evn->formID)->IsArmor())
						{
							TESObjectARMO * armo = DYNAMIC_CAST(LookupFormByID(evn->formID), TESForm, TESObjectARMO);

							SetCurrentEquipped(GetArmorMaskNumber(armo->bipedObject.GetSlotMask()), currentEquip.first);
							return kEvent_Continue;
						}

					}
					else
					{
						if (LookupFormByID(evn->formID)->IsWeapon())
						{
							//TESObjectWEAP * weap = DYNAMIC_CAST(LookupFormByID(evn->formID), TESForm, TESObjectWEAP);

							Actor * actor = DYNAMIC_CAST(evn->equippedTo, TESObjectREFR, Actor);

							if (!thisActor->GetEquippedObject(true))
							{
								SetCurrentEquipped(33, 0);
							}
							else if (!thisActor->GetEquippedObject(false))
							{
								SetCurrentEquipped(34, 0);
							}
							return kEvent_Continue;
						}
						else if (LookupFormByID(evn->formID)->IsArmor())
						{
							TESObjectARMO * armo = DYNAMIC_CAST(LookupFormByID(evn->formID), TESForm, TESObjectARMO);

							SetCurrentEquipped(GetArmorMaskNumber(armo->bipedObject.GetSlotMask()), 0);
							return kEvent_Continue;
						}
					}
				}
				else
				{
					//if (currentEquip.first != NULL_POSITION)
					{
						if (LookupFormByID(evn->formID)->IsWeapon())
						{
							//TESObjectWEAP * weap = DYNAMIC_CAST(LookupFormByID(evn->formID), TESForm, TESObjectWEAP);

							SetCurrentEquipped(32 + currentEquip.second, NULL_POSITION);
							return kEvent_Continue;
						}
						else if (LookupFormByID(evn->formID)->IsArmor())
						{
							TESObjectARMO * armo = DYNAMIC_CAST(LookupFormByID(evn->formID), TESForm, TESObjectARMO);

							SetCurrentEquipped(GetArmorMaskNumber(armo->bipedObject.GetSlotMask()), NULL_POSITION);
							return kEvent_Continue;
						}

					}
					else
					{
						if (LookupFormByID(evn->formID)->IsWeapon())
						{
							//TESObjectWEAP * weap = DYNAMIC_CAST(LookupFormByID(evn->formID), TESForm, TESObjectWEAP);

							Actor * actor = DYNAMIC_CAST(evn->equippedTo, TESObjectREFR, Actor);

							if (!thisActor->GetEquippedObject(true))
							{
								SetCurrentEquipped(33, NULL_POSITION);
							}
							else if (!thisActor->GetEquippedObject(false))
							{
								SetCurrentEquipped(34, NULL_POSITION);
							}
							return kEvent_Continue;
						}
						else if (LookupFormByID(evn->formID)->IsArmor())
						{
							TESObjectARMO * armo = DYNAMIC_CAST(LookupFormByID(evn->formID), TESForm, TESObjectARMO);

							SetCurrentEquipped(GetArmorMaskNumber(armo->bipedObject.GetSlotMask()), NULL_POSITION);
							return kEvent_Continue;
						}
					}
				}

				return kEvent_Continue;
			}
		};

		class ActivateEventHandler : public BSTEventSink < TESActivateEvent >
		{
		public:
			virtual EventResult ReceiveEvent(TESActivateEvent * evn, EventDispatcher<TESActivateEvent> * dispatcher) override
			{
				if (gm->count(GroundKey(evn->target))) //operate when obj is not persistent
				{
					TESObjectREFR * player = static_cast<TESObjectREFR*>(g_thePlayer);
					AddEntry((*gm)[GroundKey(evn->target)], evn->target, evn->target->baseForm, player);
					gm->erase(GroundKey(evn->target));
				}
				return kEvent_Continue;
			}
		};

		ContainerChangedEventHandler eventContainerChangedHandler;
		ActorEquipEventHandler eventActorEquipHandler;
		*/
		//moved to eventsinks because of stupid fucking vs13 thing where nested classes DONT MEET CPP11 STANDARD EVEN THOUGH THEY ARE USING CPP11 STD AND OTHER FEATURES WTFFF
	};
	
}

#endif