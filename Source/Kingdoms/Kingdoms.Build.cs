// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Kingdoms : ModuleRules
{
	public Kingdoms(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] 
			{ "Core", "CoreUObject", "Engine", "InputCore", "MediaAssets" });

		PublicDependencyModuleNames.AddRange(new string[]
			{ "OnlineSubsystem", "OnlineSubsystemNull", "OnlineSubsystemUtils", "Steamworks" });
		
		PrivateDependencyModuleNames.AddRange(new string[] 
			{ "Slate", "SlateCore", "UMG" });
		
		PrivateDependencyModuleNames.AddRange(new string[] 
			{ "OnlineSubsystem" });
	}
}
