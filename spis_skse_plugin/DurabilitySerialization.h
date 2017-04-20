

#include "Durability.h"

#ifndef _DURABILITYSERIALIZATION_H
#define _DURABILITYSERIALIZATION_H
#include "SerializationBase.h"

namespace spis
{
	class SerializeDurability : public SerializeT < DurabilityPair >
	{
	public:
		virtual void Serialize(SKSESerializationInterface * intfc, DurabilityPair obj) override;
		virtual DurabilityPair Unserialize(SKSESerializationInterface * intfc) override;
	};

	void SerializeDurabilityTracker(SKSESerializationInterface * intfc, DurabilityTracker * dt);
	DurabilityTracker UnserializeDurabilityTracker(SKSESerializationInterface * intfc);

	void Save(SKSESerializationInterface * intfc, DurabilityTracker * &dt, bool &isInit);
	void Load(SKSESerializationInterface * intfc, DurabilityTracker * &dt, bool &isInit);
	void Revert(SKSESerializationInterface * intfc, DurabilityTracker * &dt);

	bool RegisterSerializationCallbacks(SKSESerializationInterface * intfc, PluginHandle handle);
}

#endif