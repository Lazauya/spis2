#include "EventSinks.h"

EventDispatcher<void> * g_eventDispatchArrayOrgin = (EventDispatcher<void> *)0x012E4C30;

//these are the offset in the vftable (i'm like 90% sure)
UInt32 g_eventContainerChangedDispatcherOffset = 0x9;
EventDispatcher<TESContainerChangedEvent> * g_eventContainerChangedDispatcher = reinterpret_cast<EventDispatcher<TESContainerChangedEvent> *>(&g_eventDispatchArrayOrgin[g_eventContainerChangedDispatcherOffset]);

UInt32 g_eventActorEquipDispatcherOffset = 0xD;
EventDispatcher<TESEquipEvent> * g_eventActorEquipDispatcher = reinterpret_cast<EventDispatcher<TESEquipEvent> *>(&g_eventDispatchArrayOrgin[g_eventActorEquipDispatcherOffset]);

UInt32 g_eventActivateDispatcherOffset = 0x1;
EventDispatcher<TESActivateEvent> * g_eventActivateDispatcher = reinterpret_cast<EventDispatcher<TESActivateEvent> *>(&g_eventDispatchArrayOrgin[g_eventActorEquipDispatcherOffset]);

namespace spis
{
	bool AddEventSinks()
	{
		g_eventContainerChangedDispatcher->AddEventSink(eventContainerChangedHandler);
		g_eventActorEquipDispatcher->AddEventSink(eventActorEquipHandler);
		g_eventActivateDispatcher->AddEventSink(eventActivateEventHandler);
		return true;
	}
}