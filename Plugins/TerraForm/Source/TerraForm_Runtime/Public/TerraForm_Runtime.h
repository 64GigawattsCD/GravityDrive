// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FTerraForm_RuntimeModule : public IModuleInterface
{
public:

	const char* GDALDataPath;
	const char* GDALDriverPath;
	const char* GDALLibrariesPath;
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
