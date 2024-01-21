// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Descent : ModuleRules
{
	public Descent(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "NavigationSystem", "AIModule", "PhysicsCore", "Slate" });
    }
}
