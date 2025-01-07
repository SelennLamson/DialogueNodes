// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "Nodes/BPNode_MakeDialogue.h"
#include "DialogueBlueprintLibrary.h"
#include "Nodes/BPNode_DialogueOutcomesEvent.h"
#include "DialogueNodesUtils.h"

#include "BlueprintNodeSpawner.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "KismetCompiler.h"

#define LOCTEXT_NAMESPACE "MakeDialogueNode"

UBPNode_MakeDialogue::UBPNode_MakeDialogue(const FObjectInitializer& ObjectInitializer) : UBPNode_DialogueOutcomesEvent(ObjectInitializer)
{
	LibraryFunctionName = GET_FUNCTION_NAME_CHECKED(UDialogueBlueprintLibrary, MakeDialogue);
}

FText UBPNode_MakeDialogue::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("MakeDialogue", "Make Dialogue");
}

FName UBPNode_MakeDialogue::GetRefPinName() const
{
	return "Dialogue Ref";
}

UEdGraphPin* UBPNode_MakeDialogue::GetThenPin() const
{
	return GetPinAt(1);
}

UEdGraphPin* UBPNode_MakeDialogue::GetRefPin() const
{
	return GetPinAt(2);
}

void UBPNode_MakeDialogue::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);

	FEdGraphPinType DialogueRefType = UDialogueNodesUtils::DialoguePinType();
	DialogueRefType.bIsReference = true;
	CreatePin(EGPD_Input, DialogueRefType, GetRefPinName());

	for (int32 OutcomeIndex = 0; OutcomeIndex < NumOutcomes; OutcomeIndex++)
	{
		CreatePin(EGPD_Output, UDialogueNodesUtils::DialogueEventPinType(), GetOutcomePinName(OutcomeIndex));
	}
}

void UBPNode_MakeDialogue::ReconstructNode()
{
	UK2Node::ReconstructNode();
}


void UBPNode_MakeDialogue::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* ActionKey = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

TArray<UK2Node*> UBPNode_MakeDialogue::ExpandDialogueNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph, TArray<UK2Node*> CreatedNodes)
{
	CreatedNodes = UBPNode_DialogueBase::ExpandDialogueNode(CompilerContext, SourceGraph, CreatedNodes);

	CompilerContext.MovePinLinksToIntermediate(*GetInPin(), *FunctionNode->GetExecPin());
	CompilerContext.MovePinLinksToIntermediate(*GetThenPin(), *FunctionNode->GetThenPin());
	CompilerContext.MovePinLinksToIntermediate(*GetRefPin(), *FunctionNode->FindPinChecked(FName("Dialogue")));

	UEdGraphPin* StartingEventPin = FunctionNode->FindPinChecked(FName("StartingEvent"));

	UBPNode_DialogueOutcomesEvent* StartingEventNode = CompilerContext.SpawnIntermediateNode<UBPNode_DialogueOutcomesEvent>(this, SourceGraph);
	StartingEventNode->NumOutcomes = NumOutcomes;
	StartingEventNode->AllocateDefaultPins();
	StartingEventNode->NodePosX = NodePosX;
	StartingEventNode->NodePosY = NodePosY;
	StartingEventPin->MakeLinkTo(StartingEventNode->GetInPin());
	CreatedNodes.Add(StartingEventNode);

	for (int32 OutcomeIndex = 0; OutcomeIndex < NumOutcomes; OutcomeIndex++)
	{
		CompilerContext.MovePinLinksToIntermediate(*GetOutcomePin(OutcomeIndex), *StartingEventNode->GetOutcomePin(OutcomeIndex));
	}

	BreakAllNodeLinks();

	CreatedNodes = StartingEventNode->ExpandDialogueNode(CompilerContext, SourceGraph, CreatedNodes);
	return CreatedNodes;
}

void UBPNode_MakeDialogue::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	ExpandDialogueNode(CompilerContext, SourceGraph, TArray<UK2Node*> ());
}

#undef LOCTEXT_NAMESPACE
