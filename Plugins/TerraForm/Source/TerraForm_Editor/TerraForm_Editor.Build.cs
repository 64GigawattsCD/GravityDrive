// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class TerraForm_Editor : ModuleRules
{
	public TerraForm_Editor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseSharedPCHs;
        PrivatePCHHeaderFile = "PCHTerraForm_Editor.h";
        //bPrecompile = true;
        bUsePrecompiled = true;

        PublicDependencyModuleNames.AddRange(
			new string[]
			{
                "GDAL",
                "TerraForm_EditorAssets",
                "TerraForm_Runtime",
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "Landscape",
                "LandscapeEditor",
                "Foliage",
                "PropertyEditor",
                "ApplicationCore",
                "Http",
                "Json",
                "JsonUtilities",
            }
			);

        PrivateDependencyModuleNames.AddRange(
            new string[] 
            {
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
                "TerraForm_EditorAssets",
                "TerraForm_Runtime",
                "UnrealEd",
                "Landscape",
                "LandscapeEditor",
            });


        DynamicallyLoadedModuleNames.AddRange(
            new string[] 
            {
                "AssetTools",
			});
        
    }
}
