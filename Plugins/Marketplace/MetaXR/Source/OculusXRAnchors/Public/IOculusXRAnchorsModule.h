// @lint-ignore-every LICENSELINT
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include "Modules/ModuleManager.h"
#include "OculusXRAnchorTypes.h"
#include "OculusXRAnchorComponents.h"

#define OCULUS_ANCHORS_SUPPORTED_PLATFORMS (PLATFORM_WINDOWS && WINVER > 0x0502) || (PLATFORM_ANDROID_ARM || PLATFORM_ANDROID_ARM64 || PLATFORM_ANDROID_X64)

class UOculusXRBaseAnchorComponent;

class IOculusXRCreateAnchorComponent
{
public:
	virtual UOculusXRBaseAnchorComponent* TryCreateAnchorComponent(uint64 AnchorHandle, EOculusXRSpaceComponentType Type, UObject* Outer) = 0;
};

/**
 * The public interface to this module.  In most cases, this interface is only public to sibling modules
 * within this plugin.
 */
class IOculusXRAnchorsModule : public IModuleInterface
{
public:
	/**
	 * Singleton-like access to this module's interface.  This is just for convenience!
	 * Beware of calling this during the shutdown phase, though.  Your module might have been unloaded already.
	 *
	 * @return Returns singleton instance, loading the module on demand if needed
	 */
	static inline IOculusXRAnchorsModule& Get()
	{
		return FModuleManager::LoadModuleChecked<IOculusXRAnchorsModule>("OculusXRAnchors");
	}

	/**
	 * Checks to see if this module is loaded and ready.  It is only valid to call Get() if IsAvailable() returns true.
	 *
	 * @return True if the module is loaded and ready to use
	 */
	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("OculusXRAnchors");
	}

	virtual void AddCreateAnchorComponentInterface(IOculusXRCreateAnchorComponent* CastInterface) = 0;
	virtual void RemoveCreateAnchorComponentInterface(IOculusXRCreateAnchorComponent* CastInterface) = 0;
	virtual UOculusXRBaseAnchorComponent* CreateAnchorComponent(uint64 AnchorHandle, EOculusXRSpaceComponentType Type, UObject* Outer) = 0;
};
