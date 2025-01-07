// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class DialogueNodesEditor : ModuleRules
{

    public DialogueNodesEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateIncludePaths.AddRange(new string[] {
           "DialogueNodesEditor/Private" });

        PublicDependencyModuleNames.AddRange(new string[] {
           "Core",
           "DialogueNodes" });
        
        PrivateDependencyModuleNames.AddRange(new string[] {
           "Core",
           "CoreUObject",
           "InputCore",
           "Engine",
           "AssetTools",
           "UnrealEd", // for FAssetEditorManager
           "KismetWidgets",
           "KismetCompiler",
           "BlueprintGraph",
           "GraphEditor",
           "Kismet",  // for FWorkflowCentricApplication
           "PropertyEditor",
           "EditorStyle",
           "Slate",
           "SlateCore",
           "DetailCustomizations",
           "Settings",
           "UMG",
           "DialogueNodes"});
    }
}