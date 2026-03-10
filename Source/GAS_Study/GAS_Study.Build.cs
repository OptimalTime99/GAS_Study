// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GAS_Study : ModuleRules
{
	public GAS_Study(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[]
		{
			"GAS_Study",
			"GAS_Study/Variant_Platforming",
			"GAS_Study/Variant_Platforming/Animation",
			"GAS_Study/Variant_Combat",
			"GAS_Study/Variant_Combat/AI",
			"GAS_Study/Variant_Combat/Animation",
			"GAS_Study/Variant_Combat/Gameplay",
			"GAS_Study/Variant_Combat/Interfaces",
			"GAS_Study/Variant_Combat/UI",
			"GAS_Study/Variant_SideScrolling",
			"GAS_Study/Variant_SideScrolling/AI",
			"GAS_Study/Variant_SideScrolling/Gameplay",
			"GAS_Study/Variant_SideScrolling/Interfaces",
			"GAS_Study/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}