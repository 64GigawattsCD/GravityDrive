// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using System.IO;
using System;
using UnrealBuildTool;

public class GDAL : ModuleRules
{
    private string ModulePath
    {
        get { return ModuleDirectory; }
    }

    private string GDALPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../GDAL/")); }
    }

    /*
    private string BinariesPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../Binaries/")); }
    }*/

    public GDAL(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;
        bPrecompile = true;
        bUsePrecompiled = true;

        string PluginPath = GDALPath;
        DirectoryInfo PluginDirectory = Directory.GetParent(GDALPath.EndsWith("\\") ? GDALPath : string.Concat(GDALPath, "\\"));
        PluginPath = PluginDirectory.Parent.Parent.FullName;

        string IncludesPath = Path.Combine(PluginPath, "Source");
        IncludesPath = Path.Combine(IncludesPath, "GDAL");
        IncludesPath = Path.Combine(IncludesPath, "Includes");
        PublicSystemIncludePaths.Add(IncludesPath);
        Console.WriteLine("");
        Console.WriteLine("... IncludesPath -> " + IncludesPath);

        if ((Target.Platform == UnrealTargetPlatform.Win64) ||
            (Target.Platform == UnrealTargetPlatform.Win32))
        {
            string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "x86";
            string LibrariesPath = Path.Combine(PluginPath, "Libraries");
            PublicLibraryPaths.Add(LibrariesPath);

            if (Target.Platform == UnrealTargetPlatform.Win64)
            {
                string GLibraryName = Path.Combine(LibrariesPath, "gdal_i.lib");
                string PLibraryName = Path.Combine(LibrariesPath, "proj.lib");
                PublicAdditionalLibraries.Add(GLibraryName);
                PublicAdditionalLibraries.Add(PLibraryName);
                //RuntimeDependencies.Add(LibraryName);

                Console.WriteLine("");
                Console.WriteLine("... LibraryPath -> " + GLibraryName);
            }
            else if (Target.Platform == UnrealTargetPlatform.Win32)
            {
                string LibraryName = Path.Combine(LibrariesPath, "gdal_i.lib");
                PublicAdditionalLibraries.Add(LibraryName);

                Console.WriteLine("");
                Console.WriteLine("... LibraryPath -> " + LibraryName);
            }

            PublicDefinitions.Add(string.Format("INCLUDE_OGR_FRMTS", true));
        }

        bEnableExceptions = true;
    }
}

