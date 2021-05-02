// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class GravRacerTarget : TargetRules
{
	public GravRacerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		ExtraModuleNames.Add("GravRacer");
		ExtraModuleNames.Add("GravRacerLoadingScreen");
	}
}
