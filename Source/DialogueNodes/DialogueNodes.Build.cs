// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class DialogueNodes : ModuleRules
{

    public DialogueNodes(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PrivateIncludePaths.AddRange(new string[] { "DialogueNodes/Private" });
        PublicDependencyModuleNames.AddRange(new string[] { "Engine", "Core", "CoreUObject", "InputCore" });
    }
}