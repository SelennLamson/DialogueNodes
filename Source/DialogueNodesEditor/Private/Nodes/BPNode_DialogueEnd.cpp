// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "Nodes/BPNode_DialogueEnd.h"
#include "DialogueBlueprintLibrary.h"
#include "DialogueNodesUtils.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"

#define LOCTEXT_NAMESPACE "DialogueEndNode"

UBPNode_DialogueEnd::UBPNode_DialogueEnd(const FObjectInitializer& ObjectInitializer) : UBPNode_DialogueBase(ObjectInitializer)
{
	LibraryFunctionName = GET_FUNCTION_NAME_CHECKED(UDialogueBlueprintLibrary, MakeDialogueEnd);

	BaseParameters.Add(FDialogueParameter("Condition", false, EDialoguePinType::PT_Condition, nullptr));
}

FText UBPNode_DialogueEnd::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("DialogueEnd", "Dialogue End");
}

FText UBPNode_DialogueEnd::GetCompactNodeTitle() const
{
	return LOCTEXT("DialogueEndCompact", "END");
}

void UBPNode_DialogueEnd::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* ActionKey = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

TArray<UK2Node*> UBPNode_DialogueEnd::ExpandDialogueNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph, TArray<UK2Node*> CreatedNodes)
{
	CreatedNodes = UBPNode_DialogueBase::ExpandDialogueNode(CompilerContext, SourceGraph, CreatedNodes);

	BreakAllNodeLinks();
	return CreatedNodes;
}

#undef LOCTEXT_NAMESPACE
