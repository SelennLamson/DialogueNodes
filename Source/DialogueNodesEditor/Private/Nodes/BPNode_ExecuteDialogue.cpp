// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "Nodes/BPNode_ExecuteDialogue.h"
#include "DialogueBlueprintLibrary.h"
#include "DialogueNodesUtils.h"

#include "Engine/BlueprintGeneratedClass.h"
#include "BlueprintNodeSpawner.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "KismetCompiler.h"
#include "UObject/UnrealType.h"

#include "K2Node_CustomEvent.h"
#include "K2Node_CallFunction.h"
#include "K2Node_VariableGet.h"
#include "K2Node_Self.h"

#define LOCTEXT_NAMESPACE "ExecuteDialogueNode"

UBPNode_ExecuteDialogue::UBPNode_ExecuteDialogue(const FObjectInitializer& ObjectInitializer) : UK2Node(ObjectInitializer) { }

// Node aspect and meta-data
FText UBPNode_ExecuteDialogue::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("ExecuteDialogue", "Execute Dialogue");
}

FText UBPNode_ExecuteDialogue::GetMenuCategory() const
{
	return FText::FromString("Dialogue");
}

FLinearColor UBPNode_ExecuteDialogue::GetNodeTitleColor() const
{
	return UDialogueNodesUtils::DialogueNodesTitleColor();
}

bool UBPNode_ExecuteDialogue::IsCompatibleWithGraph(const UEdGraph* TargetGraph) const
{
	bool bIsCompatible = Super::IsCompatibleWithGraph(TargetGraph);
	if (bIsCompatible)
	{
		EGraphType const GraphType = TargetGraph->GetSchema()->GetGraphType(TargetGraph);
		bIsCompatible = (GraphType == EGraphType::GT_Ubergraph);
	}

	return bIsCompatible;
}

UEdGraphPin* UBPNode_ExecuteDialogue::GetLaunchExec() const
{
	return GetPinAt(0);
}

FName UBPNode_ExecuteDialogue::GetLaunchExecName() const
{
	return "Launch Dialogue";
}

UEdGraphPin* UBPNode_ExecuteDialogue::GetDialoguePin() const
{
	return GetPinAt(1);
}

FName UBPNode_ExecuteDialogue::GetDialoguePinName() const
{
	return "Dialogue";
}

UEdGraphPin* UBPNode_ExecuteDialogue::GetSelectExec() const
{
	return GetPinAt(2);
}

FName UBPNode_ExecuteDialogue::GetSelectExecName() const
{
	return "Select Option";
}

UEdGraphPin* UBPNode_ExecuteDialogue::GetOptionIdPin() const
{
	return GetPinAt(3);
}

FName UBPNode_ExecuteDialogue::GetOptionIdPinName() const
{
	return "Option ID";
}

UEdGraphPin* UBPNode_ExecuteDialogue::GetStopExec() const
{
	return GetPinAt(4);
}

FName UBPNode_ExecuteDialogue::GetStopExecName() const
{
	return "Stop Dialogue";
}

UEdGraphPin* UBPNode_ExecuteDialogue::GetLineExec() const
{
	return GetPinAt(5);
}

FName UBPNode_ExecuteDialogue::GetLineExecName() const
{
	return "Dialogue Line";
}

UEdGraphPin* UBPNode_ExecuteDialogue::GetLinePin() const
{
	return GetPinAt(6);
}

FName UBPNode_ExecuteDialogue::GetLinePinName() const
{
	return "Line";
}

UEdGraphPin* UBPNode_ExecuteDialogue::GetOptionsPin() const
{
	return GetPinAt(7);
}

FName UBPNode_ExecuteDialogue::GetOptionsPinName() const
{
	return "Options";
}

UEdGraphPin* UBPNode_ExecuteDialogue::GetEndExec() const
{
	return GetPinAt(8);
}

FName UBPNode_ExecuteDialogue::GetEndExecName() const
{
	return "Dialogue End";
}

void UBPNode_ExecuteDialogue::AllocateDefaultPins()
{
	// Launch dialogue section
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, GetLaunchExecName());
	CreatePin(EGPD_Input, UDialogueNodesUtils::DialoguePinType(), GetDialoguePinName());

	// Select option section
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, GetSelectExecName());
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Int, GetOptionIdPinName());

	// Stop dialogue section
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, GetStopExecName());

	// Dialogue line section
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, GetLineExecName());
	CreatePin(EGPD_Output, UDialogueNodesUtils::LineStructPinType(), GetLinePinName());
	CreatePin(EGPD_Output, UDialogueNodesUtils::OptionStructArrayPinType(), GetOptionsPinName());

	// Dialogue end section
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, GetEndExecName());
}

bool UBPNode_ExecuteDialogue::CanSplitPin(const UEdGraphPin* Pin) const
{
	return false;
}

void UBPNode_ExecuteDialogue::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* ActionKey = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

void UBPNode_ExecuteDialogue::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	///////////////////////////////////////
	//	Create nodes

	UK2Node_CustomEvent* LineEventNode = CompilerContext.SpawnIntermediateEventNode<UK2Node_CustomEvent>(this);
	LineEventNode->CustomFunctionName = "DialogueLineEvent";
	LineEventNode->RenameCustomEventCloseToName();
	LineEventNode->AllocateDefaultPins();
	LineEventNode->NodePosX = NodePosX;
	LineEventNode->NodePosY = NodePosY;
	FName LineEventName = LineEventNode->GetFunctionName();

	FName GetLine_FunctionName = GET_FUNCTION_NAME_CHECKED(UDialogueBlueprintLibrary, GetLineStruct);
	UK2Node_CallFunction* LineNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this);
	LineNode->FunctionReference.SetExternalMember(GetLine_FunctionName, UDialogueBlueprintLibrary::StaticClass());
	LineNode->AllocateDefaultPins();
	LineNode->NodePosX = NodePosX;
	LineNode->NodePosY = NodePosY;

	FName GetOptions_FunctionName = GET_FUNCTION_NAME_CHECKED(UDialogueBlueprintLibrary, GetLineOptions);
	UK2Node_CallFunction* LineOptionsNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this);
	LineOptionsNode->FunctionReference.SetExternalMember(GetOptions_FunctionName, UDialogueBlueprintLibrary::StaticClass());
	LineOptionsNode->AllocateDefaultPins();
	LineOptionsNode->NodePosX = NodePosX;
	LineOptionsNode->NodePosY = NodePosY;

	UK2Node_CustomEvent* EndEventNode = CompilerContext.SpawnIntermediateEventNode<UK2Node_CustomEvent>(this);
	EndEventNode->CustomFunctionName = "EndDialogueEvent";
	EndEventNode->RenameCustomEventCloseToName();
	EndEventNode->AllocateDefaultPins();
	EndEventNode->NodePosX = NodePosX;
	EndEventNode->NodePosY = NodePosY;
	FName EndEventName = EndEventNode->GetFunctionName();

	FName Launch_FunctionName = GET_FUNCTION_NAME_CHECKED(UDialogueBlueprintLibrary, LaunchDialogue);
	UK2Node_CallFunction* LaunchNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this);
	LaunchNode->FunctionReference.SetExternalMember(Launch_FunctionName, UDialogueBlueprintLibrary::StaticClass());
	LaunchNode->AllocateDefaultPins();
	LaunchNode->NodePosX = NodePosX;
	LaunchNode->NodePosY = NodePosY;

	FName Select_FunctionName = GET_FUNCTION_NAME_CHECKED(UDialogueBlueprintLibrary, SelectOption);
	UK2Node_CallFunction* SelectNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this);
	SelectNode->FunctionReference.SetExternalMember(Select_FunctionName, UDialogueBlueprintLibrary::StaticClass());
	SelectNode->AllocateDefaultPins();
	SelectNode->NodePosX = NodePosX;
	SelectNode->NodePosY = NodePosY;

	FName Stop_FunctionName = GET_FUNCTION_NAME_CHECKED(UDialogueBlueprintLibrary, StopDialogue);
	UK2Node_CallFunction* StopNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this);
	StopNode->FunctionReference.SetExternalMember(Stop_FunctionName, UDialogueBlueprintLibrary::StaticClass());
	StopNode->AllocateDefaultPins();
	StopNode->NodePosX = NodePosX;
	StopNode->NodePosY = NodePosY;

	UK2Node_Self* SelfNode = CompilerContext.SpawnIntermediateNode<UK2Node_Self>(this, SourceGraph);
	SelfNode->AllocateDefaultPins();
	SelfNode->NodePosX = NodePosX;
	SelfNode->NodePosY = NodePosY;


	///////////////////////////////////////
	//	Gather parameter pins

	// LineEventNode & Line/Options Getters
	UEdGraphPin* LineEventPin = LineEventNode->GetPinAt(1);
	UEdGraphPin* LinePin = LineNode->GetReturnValuePin();
	UEdGraphPin* LineDialoguePin = LineNode->FindPinChecked(FName("Dialogue"), EGPD_Input);
	UEdGraphPin* OptionsPin = LineOptionsNode->GetReturnValuePin();
	UEdGraphPin* OptionsDialoguePin = LineOptionsNode->FindPinChecked(FName("Dialogue"), EGPD_Input);

	// EndEventNode
	UEdGraphPin* EndEventPin = EndEventNode->GetPinAt(1);

	// LaunchNode
	UEdGraphPin* LaunchPin = LaunchNode->GetExecPin();
	UEdGraphPin* LaunchDialoguePin = LaunchNode->FindPinChecked(FName("Dialogue"), EGPD_Input);
	UEdGraphPin* ObjectPin = LaunchNode->FindPinChecked(FName("Object"), EGPD_Input);
	UEdGraphPin* LineEventNamePin = LaunchNode->FindPinChecked(FName("LineEventName"), EGPD_Input);
	UEdGraphPin* EndEventNamePin = LaunchNode->FindPinChecked(FName("EndEventName"), EGPD_Input);

	// SelectNode
	UEdGraphPin* SelectPin = SelectNode->GetExecPin();
	UEdGraphPin* SelectDialoguePin = SelectNode->FindPinChecked(FName("Dialogue"), EGPD_Input);
	UEdGraphPin* OptionIdPin = SelectNode->FindPinChecked(FName("OptionId"), EGPD_Input);

	// StopNode
	UEdGraphPin* StopPin = StopNode->GetExecPin();
	UEdGraphPin* StopDialoguePin = StopNode->FindPinChecked(FName("Dialogue"), EGPD_Input);

	// SelfNode
	UEdGraphPin* SelfPin = SelfNode->GetPinAt(0);


	///////////////////////////////////////
	//	Establish links

	// LineEventNode
	CompilerContext.MovePinLinksToIntermediate(*GetLineExec(), *LineEventPin);
	CompilerContext.MovePinLinksToIntermediate(*GetLinePin(), *LinePin);
	CompilerContext.MovePinLinksToIntermediate(*GetOptionsPin(), *OptionsPin);

	// EndEventNode
	CompilerContext.MovePinLinksToIntermediate(*GetEndExec(), *EndEventPin);

	// LaunchNode
	CompilerContext.MovePinLinksToIntermediate(*GetLaunchExec(), *LaunchPin);
	CompilerContext.MovePinLinksToIntermediate(*GetDialoguePin(), *LaunchDialoguePin);
	ObjectPin->MakeLinkTo(SelfPin);
	LineEventNamePin->DefaultValue = LineEventName.ToString();
	EndEventNamePin->DefaultValue = EndEventName.ToString();

	// SelectNode
	CompilerContext.MovePinLinksToIntermediate(*GetSelectExec(), *SelectPin);
	CompilerContext.MovePinLinksToIntermediate(*GetOptionIdPin(), *OptionIdPin);

	// StopNode
	CompilerContext.MovePinLinksToIntermediate(*GetStopExec(), *StopPin);

	if (LaunchDialoguePin->LinkedTo.Num() > 0)
	{
		LineDialoguePin->MakeLinkTo(LaunchDialoguePin->LinkedTo[0]);
		OptionsDialoguePin->MakeLinkTo(LaunchDialoguePin->LinkedTo[0]);
		SelectDialoguePin->MakeLinkTo(LaunchDialoguePin->LinkedTo[0]);
		StopDialoguePin->MakeLinkTo(LaunchDialoguePin->LinkedTo[0]);
	}
	

	///////////////////////////////////////
	//	End

	BreakAllNodeLinks();
}

#undef LOCTEXT_NAMESPACE
