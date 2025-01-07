// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "Nodes/BPNode_DialogueBlueprintEvent.h"
#include "DialogueBlueprintLibrary.h"
#include "DialogueNodesUtils.h"

#include "BlueprintNodeSpawner.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "KismetCompiler.h"

#include "K2Node_CustomEvent.h"
#include "K2Node_CallFunction.h"

#define LOCTEXT_NAMESPACE "DialogueBlueprintEventNode"

UBPNode_DialogueBlueprintEvent::UBPNode_DialogueBlueprintEvent(const FObjectInitializer& ObjectInitializer) : UBPNode_DialogueBase(ObjectInitializer)
{
	LibraryFunctionName = GET_FUNCTION_NAME_CHECKED(UDialogueBlueprintLibrary, MakeDialogueBlueprintEvent);

	BaseParameters.Add(FDialogueParameter("EventName", false, EDialoguePinType::PT_Name, nullptr));
	BaseParameters.Add(FDialogueParameter("Condition", false, EDialoguePinType::PT_Condition, nullptr));
}

FText UBPNode_DialogueBlueprintEvent::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("DialogueBlueprintEvent", "Dialogue To Blueprint");
}

FText UBPNode_DialogueBlueprintEvent::GetCompactNodeTitle() const
{
	return LOCTEXT("DialogueBlueprintEventCompact", "To BP");
}

FName UBPNode_DialogueBlueprintEvent::GetThenPinName() const
{
	return "";
}

UEdGraphPin* UBPNode_DialogueBlueprintEvent::GetThenPin() const
{
	return GetPinAt(Pins.Num() - 1);
}

void UBPNode_DialogueBlueprintEvent::AllocateDefaultPins()
{
	UBPNode_DialogueBase::AllocateDefaultPins();

	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, GetThenPinName());
}

void UBPNode_DialogueBlueprintEvent::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* ActionKey = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

TArray<UK2Node*> UBPNode_DialogueBlueprintEvent::ExpandDialogueNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph, TArray<UK2Node*> CreatedNodes)
{
	EventName = NAME_None;
	
	if (GetThenPin()->LinkedTo.Num() > 0)
	{
		//////////////////////////
		//	Custom event node

		UK2Node_CustomEvent* EventNode = CompilerContext.SpawnIntermediateEventNode<UK2Node_CustomEvent>(this);
		EventNode->CustomFunctionName = "DialogueEvent";
		EventNode->RenameCustomEventCloseToName();
		EventNode->AllocateDefaultPins();
		EventNode->GetPinAt(1)->MakeLinkTo(GetThenPin()->LinkedTo[0]);
		EventNode->NodePosX = NodePosX;
		EventNode->NodePosY = NodePosY;
		CreatedNodes.Add(EventNode);

		EventName = EventNode->GetFunctionName();
	}

	CreatedNodes = UBPNode_DialogueBase::ExpandDialogueNode(CompilerContext, SourceGraph, CreatedNodes);

	BreakAllNodeLinks();
	return CreatedNodes;
}

#undef LOCTEXT_NAMESPACE
