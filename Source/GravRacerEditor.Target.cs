// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class GravRacerEditorTarget : TargetRules
{
	public GravRacerEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		ExtraModuleNames.Add("GravRacer");
		ExtraModuleNames.Add("GravRacerLoadingScreen");
	}
}
