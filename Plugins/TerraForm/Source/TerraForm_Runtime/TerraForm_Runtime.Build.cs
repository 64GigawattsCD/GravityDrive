// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class TerraForm_Runtime : ModuleRules
{ 

    public TerraForm_Runtime(ReadOnlyTargetRules Target) : base(Target)
	{
        PublicDefinitions.Add("TFCONFIG=2");

        PCHUsage = ModuleRules.PCHUsageMode.UseSharedPCHs;
        PrivatePCHHeaderFile = "PCHTerraForm_Runtime.h";
        //bPrecompile = true;
        bUsePrecompiled = true;
			
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "GDAL",
                "Landscape",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"InputCore",
                "Slate",
                "SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
			);


        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
				// ... add any modules that your module loads dynamically here ...
			}
            );

        bEnableExceptions = true;

    }

   

}
