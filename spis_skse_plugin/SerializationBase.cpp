#include "SerializationBase.h"

namespace spis
{
	//can't define templates in cpp files... why vs2013? what have i done to deserve this?
	/*
	template<class T>
	void SerializeFormREFRPair(SKSESerializationInterface * intfc, typename BasicTracker<T>::FormREFRPair frp)
	{
		intfc->WriteRecordData(&frp.first, sizeof(UInt32));
		intfc->WriteRecordData(&frp.second, sizeof(UInt32));
	}

	template<class T>
	void SerializeEquipVector(SKSESerializationInterface * intfc, typename BasicTracker<T>::EquippedVector * ev)
	{
		UInt32 vs = ev->size();
		intfc->WriteRecordData(&size, sizeof(std::size_t));
		for (UInt32 i = 0; i < vs; i++)
		{
			intfc->WriteRecordData((*ev)[i], sizeof(UInt32));
		}
	}

	template<class T>
	void SerializeItemKey(SKSESerializationInterface * intfc, typename BasicTracker<T>::ItemKey ik)
	{
		SerializeFormREFRPair<T>(intfc, ik.p);
	}

	template<class T>
	void SerializeGroundKey(SKSESerializationInterface * intfc, typename BasicTracker<T>::GroundKey gk)
	{
		UInt32 ref = gk.item->CreateRefHandle();
		intfc->WriteRecordData(&ref, sizeof(UInt32));
		intfc->WriteRecordData(&gk.hash, sizeof(UInt32));
	}

	template<class T>
	void SerializeGroundMap(SKSESerializationInterface * intfc, typename BasicTracker<T>::GroundMap * gm, SerializeT<T>& pred)
	{
		UInt32 ms = gm->size();
		intfc->WriteRecordData(&ms, sizeof(std::size_t));
		for (auto it = gm->begin(); it != gm->end(); it++)
		{
			SerializeGroundKey<T>(it->first);
			pred(it->second);
		}
	}

	template<class T>
	void SerializeContainerKey(SKSESerializationInterface * intfc, typename BasicTracker<T>::ContainerKey ck)
	{
		intfc->WriteRecordData(&ck.hash, sizeof(UInt32));
	}

	template<class T>
	void SerializeTVector(SKSESerializationInterface * intfc, std::vector<T> vt, SerializeT<T>& pred)
	{
		UInt32 vs = ev->size();
		intfc->WriteRecordData(&size, sizeof(std::size_t));
		for (UInt32 i = 0; i < vs; i++)
		{
			pred.Serialize(intfc, vt[i]);
		}
	}

	template<class T>
	void SerializeContainerMap(SKSESerializationInterface * intfc, typename BasicTracker<T>::ContainerMap * cm, SerializeT<T>& pred)
	{
		std::size_t ms = cm->size();
		intfc->WriteRecordData(&ms, sizeof(std::sizt_t));

		for (auto it = cm->begin(); it != cm->end(); it++)
		{
			SerializeContainerKey<T>(intfc, it->first);

			std::size_t mss = it->second.size();
			intfc->WriteRecordData(&mss, sizeof(std::sizt_t));
			for (auto itt = it->second->begin(); itt != it->second->end(); itt++)
			{
				SerializeItemKey<T>(intfc, itt->first);
				SerializeTVector<T>(intfc, itt->second, pred);
			}
		}
	}

	template<class T>
	void SerializeBasicTracker(SKSESerializationInterface * intfc, typename BasicTracker<T> * bt, SerializeT<T>& pred)
	{
		SerializeContainerMap<T>(intfc, bt->cm, pred);
		SerializeGroundMap<T>(intfc, bt->gm, pred);
		SerializeEquipVector<T>(intfc, bt->ev, pred);
	}

	//unserializations

	template<class T>
	typename BasicTracker<T>::FormREFRPair UnserializeFormREFRPair(SKSESerializationInterface * intfc)
	{
		UInt32 first, second;
		intfc->ReadRecordData(&first, sizeof(UInt32));
		intfc->ReadRecordData(&second, sizeof(UInt32));
		return typename BasicTracker<T>::FormREFRPair(first, second);
	}

	template<class T>
	typename BasicTracker<T>::ItemKey UnserializeItemKey(SKSESerializationInterface * intfc)
	{
		return typename BasicTracker<T>::ItemKey(UnserializeFormREFRPair<T>(intfc));
	}

	template<class T>
	std::vector<T> UnserializeTVector(SKSESerializationInterface * intfc, SerializeT<T>& pred)
	{
		UInt32 vs;
		intfc->ReadRecordData(&vs, sizeof(UInt32));

		std::vector<T> ovec;

		for (UInt32 i = 0; i < vs; i++)
		{
			T cur = pred.Unserialize(intfc)
			ovec.push(cur);
		}

		return ovec;
	}

	template<class T>
	typename BasicTracker<T>::ContainerKey UnserializeContainerKey(SKSESerializationInterface * intfc)
	{
		UInt32 han;
		intfc->ReadRecordData(&han, sizeof(UInt32));
		return typename BasicTracker<T>::ContainerKey(han);
	}

	template<class T>
	typename BasicTracker<T>::GroundKey UnserializeGroundKey(SKSESerializationInterface * intfc)
	{
		UInt32 hash;
		intfc->ReadRecordData(&hash, sizeof(UInt32));
		return typename BasicTracker<T>::GroundKey(hash);
	}

	template<class T>
	typename BasicTracker<T>::ContainerMap UnserializeContainerMap(SKSESerializationInterface * intfc, SerializeT<T>& pred)
	{
		UInt32 ms;
		intfc->ReadRecordData(&ms, sizeof(UInt32));
		
		typename BasicTracker<T>::ContainerMap cm;

		for (UInt32 i = 0; i < ms; i++)
		{
			typename BasicTracker<T>::ContainerKey ck = UnserializeContainerKey<T>(intfc);
			UInt32 mss;
			intfc->ReadRecordData(&mss, sizeof(UInt32));
			std::unordered_map<typename BasicTracker<T>::ItemKey, std::vector<T>, typename BasicTracker<T>::ItemKeyHash> scm;
			for (UInt32 j = 0; j < mss; j++)
			{
				scm.insert(std::pair<typename BasicTracker<T>::ItemKey, std::vector<T> >(UnserializeItemKey<T>(intfc), UnserializeTVector<T>(intfc, pred)));
			}
			cm.insert(std::pair<typename BasicTracker<T>::ContainerKey, std::unordered_map<typename BasicTracker<T>::ItemKey, std::vector<T>, typename BasicTracker<T>::ItemKeyHash> >(ck, scm));
		}
		return cm;
	}

	template<class T>
	typename BasicTracker<T>::GroundMap UnserializeGroundMap(SKSESerializationInterface * intfc, SerializeT<T>& pred)
	{
		UInt32 ms;
		intfc->ReadRecordData(&ms, sizeof(UInt32));

		BasicTracker<T>::GroundMap omap;

		for (UInt32 i = 0; i < ms; i++)
		{
			omap.insert(std::pair<typename BasicTracker<T>::GroundKey, T>(UnserializeGroundKey<T>(intfc), pred.Unserialize(intfc)));
		}

		return omap;
	}

	template<class T>
	typename BasicTracker<T>::EquippedVector UnserializeEquipVector(SKSESerializationInterface * intfc)
	{
		UInt32 vs;
		intfc->ReadRecordData(&vs, sizeof(UInt32));

		typename BasicTracker<T>::EquippedVector ovec;

		for (UInt32 i = 0; i < vs; i++)
		{
			UInt32 cur;
			intfc->ReadRecordData(&cur, sizeof(UInt32));
			ovec.push(cur);
		}

		return ovec;
	}
	
	template<class T>
	typename BasicTracker<T> UnserializeBasicTracker(SKSESerializationInterface * intfc, SerializeT<T>& pred)
	{
		typename BasicTracker<T> bt;
		(*bt.cm) = UnserializeContainerMap<T>(intfc, pred);
		(*bt.gm) = UnserializeGroundMap<T>(intfc, pred);
		(*bt.ev) = UnserializeEquipVector<T>(intfc);
		return bt;
	}
	*/
}