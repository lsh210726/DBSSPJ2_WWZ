// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class WWZ : ModuleRules
{
	public WWZ(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "NavigationSystem", "AIModule" });
	}
}
