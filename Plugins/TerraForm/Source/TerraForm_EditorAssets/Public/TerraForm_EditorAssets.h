// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Array.h"
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "Templates/SharedPointer.h"
#include "Toolkits/AssetEditorToolkit.h"

#include "AssetTools/GISProjectionAssetActions.h"
//#include "AssetTools/GISShapeAssetActions.h"
#include "Styles/TerraFormEditorStyle.h"
#include "Shared/TerraFormEditorSettings.h"


class FTerraForm_EditorAssetsModule 
	: public IHasMenuExtensibility
	, public IHasToolBarExtensibility
	, public IModuleInterface
{

public:

	//~ IHasMenuExtensibility interface

	virtual TSharedPtr<FExtensibilityManager> GetMenuExtensibilityManager() override
	{
		return MenuExtensibilityManager;
	}

public:

	//~ IHasToolBarExtensibility interface

	virtual TSharedPtr<FExtensibilityManager> GetToolBarExtensibilityManager() override
	{
		return ToolBarExtensibilityManager;
	}

public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	virtual bool SupportsDynamicReloading() override;

protected:

	/** Registers asset tool actions. */
	void RegisterAssetTools();

	/**
	 * Registers a single asset type action.
	 *
	 * @param AssetTools The asset tools object to register with.
	 * @param Action The asset type action to register.
	 */
	void RegisterAssetTypeAction(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action);

	/** Register the editor settings. */
	void RegisterSettings();

	/** Unregisters asset tool actions. */
	void UnregisterAssetTools();

	/** Unregister the editor settings. */
	void UnregisterSettings();

protected:

	/** Registers main menu and tool bar menu extensions. */
	virtual void RegisterMenuExtensions();

	/** Unregisters main menu and tool bar menu extensions. */
	virtual void UnregisterMenuExtensions();

private:

	/** Holds the menu extensibility manager. */
	TSharedPtr<FExtensibilityManager> MenuExtensibilityManager;

	/** The collection of registered asset type actions. */
	TArray<TSharedRef<IAssetTypeActions>> RegisteredAssetTypeActions;

	/** Holds the plug-ins style set. */
	TSharedPtr<ISlateStyle> Style;

	/** Holds the tool bar extensibility manager. */
	TSharedPtr<FExtensibilityManager> ToolBarExtensibilityManager;
};
