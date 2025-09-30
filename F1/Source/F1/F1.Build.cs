// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class F1 : ModuleRules
{
	public F1(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(new string[]
		{
            "F1"
		});

        PublicDependencyModuleNames.AddRange(new string[] 
		{ 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput",
            "NetCore",
            "GameplayAbilities"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
		{
            "GameplayTasks",
            "GameplayTags",
            "UMG",
            "Slate",
            "SlateCore",
            "AIModule",
            "NavigationSystem",
			"Niagara"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
