#include "skse/PluginAPI.h"		// super
#include "skse/skse_version.h"	// What version of SKSE is running?
#include <shlobj.h>				// CSIDL_MYCODUMENTS

#include "version.h"

#include "Durability.h"
#include "ScaleformFuncs.h"

#include "EventSinks.h"

#include "DurabilitySerialization.h"

static PluginHandle					g_pluginHandle = kPluginHandle_Invalid;
static SKSEPapyrusInterface         * g_papyrus = NULL;
static SKSEScaleformInterface		* g_scaleform = NULL;
static SKSESerializationInterface	* g_serialization = NULL;

extern "C"	{

	bool SKSEPlugin_Query(const SKSEInterface * skse, PluginInfo * info)	{	// Called by SKSE to learn about this plugin and check that it's safe to load it
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Skyrim\\SKSE\\spis.log");
		gLog.SetPrintLevel(IDebugLog::kLevel_Error);
		gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);

		_MESSAGE("SplitItemStacks");

		// populate info structure
		info->infoVersion =	PluginInfo::kInfoVersion;
		info->name =		"SplitItemStacks";
		info->version =		(SPIS_VERSION_MAJOR << 16) + SPIS_VERSION_MINOR;

		// store plugin handle so we can identify ourselves later
		g_pluginHandle = skse->GetPluginHandle();

		if(skse->isEditor)
		{
			_MESSAGE("loaded in editor, marking as incompatible");

			return false;
		}
		else if(skse->runtimeVersion != RUNTIME_VERSION_1_9_32_0)
		{
			_MESSAGE("unsupported runtime version %08X", skse->runtimeVersion);

			return false;
		}

		g_scaleform = (SKSEScaleformInterface *)skse->QueryInterface(kInterface_Scaleform);

		g_serialization = (SKSESerializationInterface *)skse->QueryInterface(kInterface_Serialization);
		
		if (!g_serialization)
		{
			_MESSAGE("couldn't get serialization interface");

			return false;
		}

		if (g_serialization->version < SKSESerializationInterface::kVersion)
		{
			_MESSAGE("serialization interface too old (%d expected %d)", g_serialization->version, SKSESerializationInterface::kVersion);

			return false;
		}

		// ### do not do anything else in this callback
		// ### only fill out PluginInfo and return true/false

		// supported runtime version
		return true;
	}

	bool SKSEPlugin_Load(const SKSEInterface * skse)	{	// Called by SKSE to load this plugin
		_MESSAGE("SPIS loaded, version %d.%d", SPIS_VERSION_MAJOR, SPIS_VERSION_MINOR);

		g_papyrus = (SKSEPapyrusInterface *)skse->QueryInterface(kInterface_Papyrus);

		//Check if the function registration was a success...
		bool btest = g_scaleform->Register("spis", spis::RegisterScaleformFunctions);
		bool ctest = spis::RegisterSerializationCallbacks(g_serialization, g_pluginHandle);

		if (btest && ctest) {
			_MESSAGE("Register Succeeded");
			if (spis::InitializeTracker(true))
			{
				_MESSAGE("Tracker initialization succeeded");

				//add event sinks
				if (spis::AddEventSinks())
				{
					_MESSAGE("Events hooked");
				}
			}
		}

		return true;
	}

};