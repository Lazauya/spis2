#include "DurabilitySerialization.h"
#include "version.h"
#include "GarbageCollection.h"

namespace spis
{
	enum
	{
		kTypeInitialized,
		kTypeDurabilityTracker
	};


	void SerializeDurability::Serialize(SKSESerializationInterface * intfc, DurabilityPair obj)
	{
		intfc->WriteRecordData(&obj.maxDurability, sizeof(UInt32));
		intfc->WriteRecordData(&obj.curDurability, sizeof(UInt32));
	}

	DurabilityPair SerializeDurability::Unserialize(SKSESerializationInterface * intfc)
	{
		UInt32 max, cur;
		intfc->ReadRecordData(&max, sizeof(UInt32));
		intfc->ReadRecordData(&cur, sizeof(UInt32));
		return DurabilityPair(max, cur);
	}

	void SerializeDurabilityTracker(SKSESerializationInterface * intfc, DurabilityTracker * dt)
	{
		SerializeDurability sd;
		intfc->OpenRecord(kTypeDurabilityTracker, SPIS_SERIALIZATION_VERSION);
		SerializeBasicTracker<DurabilityPair>(intfc, dt, sd);
	}

	DurabilityTracker UnserializeDurabilityTracker(SKSESerializationInterface * intfc)
	{
		SerializeDurability sd;
		BasicTracker<DurabilityPair> btdp = UnserializeBasicTracker<DurabilityPair>(intfc, sd);
		DurabilityTracker dt;
		dt.cm = btdp.cm;
		dt.gm = btdp.gm;
		dt.ev = btdp.ev;
		return dt;
	}

	void Save(SKSESerializationInterface * intfc)
	{
		intfc->OpenRecord(kTypeInitialized, SPIS_SERIALIZATION_VERSION);
		intfc->WriteRecordData(&g_isInit, sizeof(bool));

		SerializeDurabilityTracker(intfc, g_DurabilityTracker);
	}

	void Load(SKSESerializationInterface * intfc)
	{
		UInt32 type, ver, len;
		while (intfc->GetNextRecordInfo(&type, &ver, &len))
		{
			switch (type)
			{
			case kTypeDurabilityTracker:
				*g_DurabilityTracker = UnserializeDurabilityTracker(intfc);
			case kTypeInitialized:
				intfc->ReadRecordData(&g_isInit, sizeof(bool));
			}
		}
	}

	void Revert(SKSESerializationInterface * intfc)
	{
		CollectGarbage();
	}

	bool RegisterSerializationCallbacks(SKSESerializationInterface * intfc, PluginHandle handle)
	{
		intfc->SetUniqueID(handle, 'SPIS');
		intfc->SetRevertCallback(handle, Revert);
		intfc->SetSaveCallback(handle, Save);
		intfc->SetLoadCallback(handle, Load);
		return true;
	}
	
}