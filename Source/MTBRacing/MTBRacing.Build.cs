// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MTBRacing : ModuleRules
{
	public MTBRacing(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		// 按需添加：
		// - Chaos 载具系统：ChaosVehicles
		// - GAS 能力系统：GameplayAbilities, GameplayTags, GameplayTasks
		// - Slate UI：Slate, SlateCore
		// - 网络子系统：OnlineSubsystem
	}
}
