// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class VRMilitarySimulationTarget : TargetRules
{
	public VRMilitarySimulationTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;

		ExtraModuleNames.AddRange( new string[] { "VRMilitarySimulation" } );

        // 사전 컴파일 활성화
        bPrecompile = true;
    }
}
