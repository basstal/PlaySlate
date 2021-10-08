// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PlaySlate : ModuleRules
{
	public PlaySlate(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"UnrealEd",
			"EditorFramework",
			"BlueprintGraph",
			"Kismet",
			"RHI",
			"TimeManagement",
			"Slate",
			"SlateCore",
			"EditorStyle",
			"AssetTools",
			"ContentBrowser",
			"AdvancedPreviewScene",
			"AnimGraph"
		});
	}
}