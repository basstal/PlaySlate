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
			"EditorStyle",
			"EditorFramework",
			"EditorWidgets",
			"BlueprintGraph",
			"Kismet",
			"KismetWidgets",
			"RHI",
			"TimeManagement",
			"Slate",
			"SlateCore",
			"AssetTools",
			"ContentBrowser",
			"AdvancedPreviewScene",
			"AnimGraph",
			"StatusBar",
			"SkeletonEditor"
			
		});
	}
}