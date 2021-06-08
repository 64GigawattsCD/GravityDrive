// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class TerraForm_EditorAssets : ModuleRules
{
	public TerraForm_EditorAssets(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseSharedPCHs;
        PrivatePCHHeaderFile = "PCHTerraForm_EditorAssets.h";
        //bPrecompile = true;
        bUsePrecompiled = true;

        PublicDependencyModuleNames.AddRange(
			new string[]
			{
                "GDAL",
                "TerraForm_Runtime",
                "Core",
                "LandscapeEditor",
                "Landscape",
                "Projects",
				// ... add other public dependencies that you statically link with here ...
			}
			);

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "ContentBrowser",
                "Core",
                "CoreUObject",
                "DesktopWidgets",
                "EditorStyle",
                "Engine",
                "InputCore",
                "Projects",
                "Slate",
                "SlateCore",
                "TerraForm_Runtime",
                "UnrealEd",
                "LandscapeEditor",
            });


        DynamicallyLoadedModuleNames.AddRange(
            new string[] {
                "AssetTools",
			});
        
    }
}
