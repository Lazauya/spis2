#ifndef _EVENTSINKS_H
#define _EVENTSINKS_H

#include "skse/GameEvents.h"
#include "Durability.h"
#include "WeaponArmorMasks.h"
#include <utility>

struct TESContainerChangedEvent
{
	UInt32		from; //ref ptr?
	UInt32		to; //ref ptr?
	UInt32		item; //probably ref
	UInt32		count;
	UInt32		itemBaseForm; //probably base form, need to check
	UInt32		unk14;
};

struct TESEquipEvent
{
	TESObjectREFR * equippedTo; //probably obj ref, possibly actor
	UInt32	formID;
	UInt32	refHandle; //extra data list ptr?
	UInt32	unk; //17th bit determines equip state??? pointer to base extralist? flags?
};

struct TESActivateEvent
{
	TESObjectREFR	* target;		// 00 - TESObjectREFRPtr
	TESObjectREFR	* caster;		// 04 - TESObjectREFRPtr
};

template<>
class BSTEventSink < TESContainerChangedEvent >
{
public:
	virtual ~BSTEventSink() {}; // todo?
	virtual	EventResult ReceiveEvent(TESContainerChangedEvent * evn, EventDispatcher<TESContainerChangedEvent> * dispatcher) = 0;
};

template<>
class BSTEventSink < TESEquipEvent >
{
public:
	virtual ~BSTEventSink() {}; // todo?
	virtual	EventResult ReceiveEvent(TESEquipEvent * evn, EventDispatcher<TESEquipEvent> * dispatcher) = 0;
};

template<>
class BSTEventSink < TESActivateEvent >
{
public:
	virtual ~BSTEventSink() {}; // todo?
	virtual	EventResult ReceiveEvent(TESActivateEvent * evn, EventDispatcher<TESActivateEvent> * dispatcher) = 0;
};

//EventDispatcher<void> * g_eventDispatchArrayOrgin;// = (EventDispatcher<void> *)0x012E4C30;

//UInt32 g_eventContainerChangedDispatcherOffset;// = 0x9;
//EventDispatcher<TESContainerChangedEvent> * g_eventContainerChangedDispatcher;// = reinterpret_cast<EventDispatcher<TESContainerChangedEvent> *>(&g_eventDispatchArrayOrgin[g_eventContainerChangedDispatcherOffset]);

//UInt32 g_eventActorEquipDispatcherOffset;// = 0x9;
//EventDispatcher<TESEquipEvent> * g_eventActorEquipDispatcher;

//UInt32 g_eventActivateDispatcherOffset;// = 0x1;
//EventDispatcher<TESActivateEvent> * g_eventActivateDispatcher;


namespace spis
{
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

				//g_DurabilityTracker->

				if ((std::get<2>(g_DurabilityTracker->currentT).second ? std::get<2>(g_DurabilityTracker->currentT).second : std::get<2>(g_DurabilityTracker->currentT).first))
				{
					if (std::get<2>(g_DurabilityTracker->currentT).second && std::get<3>(g_DurabilityTracker->currentT) == evn->from)
					{
						TESObjectREFR * obj = nullptr;
						LookupREFRByHandle(&(std::get<2>(g_DurabilityTracker->currentT).second), &obj);

						g_DurabilityTracker->AddEntry(std::get<0>(g_DurabilityTracker->currentT), obj, obj->baseForm, cnt);
						g_DurabilityTracker->RemoveExact(std::get<0>(g_DurabilityTracker->currentT), obj, obj->baseForm, frm, std::get<1>(g_DurabilityTracker->currentT));

						std::get<1>(g_DurabilityTracker->currentT) = 0;
						std::get<2>(g_DurabilityTracker->currentT).first = 0;
						std::get<2>(g_DurabilityTracker->currentT).second = 0;
						return kEvent_Continue;
					}
					else if (std::get<2>(g_DurabilityTracker->currentT).first && std::get<3>(g_DurabilityTracker->currentT) == evn->from)
					{
						g_DurabilityTracker->AddEntry(std::get<0>(g_DurabilityTracker->currentT), nullptr, LookupFormByID(std::get<2>(g_DurabilityTracker->currentT).first), cnt);
						g_DurabilityTracker->RemoveExact(std::get<0>(g_DurabilityTracker->currentT), nullptr, LookupFormByID(std::get<2>(g_DurabilityTracker->currentT).first), frm, std::get<1>(g_DurabilityTracker->currentT));

						std::get<1>(g_DurabilityTracker->currentT) = 0;
						std::get<2>(g_DurabilityTracker->currentT).first = 0;
						std::get<2>(g_DurabilityTracker->currentT).second = 0;
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

					UInt32 l = g_DurabilityTracker->GetLowest(obj, frm, LookupFormByID(evn->itemBaseForm));
					if (l != NULL_POSITION)
					{
						DurabilityPair lowest = (*g_DurabilityTracker->cm)[DurabilityTracker::ContainerKey(frm)][DurabilityTracker::ItemKey(LookupFormByID(evn->itemBaseForm), obj)][l];
						g_DurabilityTracker->AddEntry(lowest, obj, LookupFormByID(evn->itemBaseForm));
					}

					g_DurabilityTracker->RemoveLowest(obj, frm, LookupFormByID(evn->itemBaseForm));
				}

				if (!evn->from) //only operate when obj is persistent
				{
					TESObjectREFR * obj = nullptr;
					LookupREFRByHandle(&evn->item, &obj);

					if (obj)
					{
						DurabilityPair t = (*g_DurabilityTracker->gm)[DurabilityTracker::GroundKey(obj)];
						g_DurabilityTracker->RemoveExact(t, obj, obj->baseForm);
						g_DurabilityTracker->AddEntry(t, obj, obj->baseForm, cnt);
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
			if (LookupFormByID(evn->formID)->IsArmor())
			{
				BGSBipedObjectForm * bof = DYNAMIC_CAST(LookupFormByID(evn->formID), TESForm, BGSBipedObjectForm);
				if (bof)
				{
					ExtraContainerChanges::FoundEquipData ed = ResolveEquippedObject(static_cast<Actor*>(*g_thePlayer), kHand_None, bof->data.parts);
					if (ed.pForm == LookupFormByID(evn->formID))
					{
						(*g_DurabilityTracker->ev)[GetArmorMaskNumber(bof->data.parts)] = 0;
					}
					else
					{
						(*g_DurabilityTracker->ev)[GetArmorMaskNumber(bof->data.parts)] = NULL_POSITION;
					}
				}
				else
				{
					_MESSAGE("Selected item was not casted correctly but was an armor");
				}
			}
			else if (LookupFormByID(evn->formID)->IsWeapon())
			{
				//UInt32 hand = UInt32(args->args[0].GetNumber());
				TESForm * eol = static_cast<Actor*>(*g_thePlayer)->GetEquippedObject(true);
				TESForm * eor = static_cast<Actor*>(*g_thePlayer)->GetEquippedObject(false);
				std::bitset<32> b = evn->unk;

				if ((eol == LookupFormByID(evn->formID)) && b[17])
				{
					(*g_DurabilityTracker->ev)[32 + kHand_Left] = 0;
				}
				else
				{
					(*g_DurabilityTracker->ev)[32 + kHand_Left] = NULL_POSITION;
				}

				if ((eor == LookupFormByID(evn->formID)) && b[17])
				{
					(*g_DurabilityTracker->ev)[32 + kHand_Right] = 0;
				}
				else
				{
					(*g_DurabilityTracker->ev)[32 + kHand_Right] = NULL_POSITION;
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
			if (g_DurabilityTracker->gm->count(DurabilityTracker::GroundKey(evn->target))) //operate when obj is not persistent
			{
				TESObjectREFR * player = static_cast<TESObjectREFR*>(*g_thePlayer);
				g_DurabilityTracker->AddEntry((*g_DurabilityTracker->gm)[DurabilityTracker::GroundKey(evn->target)], evn->target, evn->target->baseForm, player);
				g_DurabilityTracker->gm->erase(DurabilityTracker::GroundKey(evn->target));
			}
			return kEvent_Continue;
		}
	};

	static ContainerChangedEventHandler * eventContainerChangedHandler;
	static ActorEquipEventHandler * eventActorEquipHandler;
	static ActivateEventHandler * eventActivateEventHandler;

	bool AddEventSinks();
}

#endif