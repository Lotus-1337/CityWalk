// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.IO;

public class CityWalk : ModuleRules
{
	public CityWalk(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "NavigationSystem", "Navmesh" });

		PrivateDependencyModuleNames.AddRange(new string[] { "EnhancedInput"});

		PublicIncludePaths.AddRange(new string[] 
		{
			Path.Combine(ModuleDirectory, "Public"),
			Path.Combine(ModuleDirectory, "Public/AI"),
			Path.Combine(ModuleDirectory, "Public/Core"),
			Path.Combine(ModuleDirectory, "Public/PathFinding"),
			Path.Combine(ModuleDirectory, "Public/Player")
		});

        PrivateIncludePaths.AddRange(new string[]
        {
            Path.Combine(ModuleDirectory, "Private"),
            Path.Combine(ModuleDirectory, "Private/AI"),
            Path.Combine(ModuleDirectory, "Private/Core"),
            Path.Combine(ModuleDirectory, "Private/PathFinding"),
            Path.Combine(ModuleDirectory, "Private/Player")
        });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
