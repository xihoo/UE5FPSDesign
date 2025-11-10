// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SKGShooterFrameworkEditor : ModuleRules
{
	public SKGShooterFrameworkEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		IWYUSupport = IWYUSupport.Full;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"GameplayTags",
				"AnimGraphRuntime",
				"BlueprintGraph",
				"AnimGraph",
				"AnimationBlueprintLibrary",
				"AnimationModifiers",
				"SKGAnimGraphNodes"
			}
			);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine"
			}
			);
		
		//SetupIrisSupport(Target);
	}
}
