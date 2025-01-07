// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "Nodes/BPNode_DialogueOutcomesEvent.h"
#include "Nodes/BPNode_DialogueTextEvent.h"
#include "DialogueBlueprintLibrary.h"
#include "DialogueNodesUtils.h"

#include "KismetCompiler.h"
#include "ScopedTransaction.h"

#include "Framework/MultiBox/MultiBoxBuilder.h"

#include "K2Node_CallFunction.h"
#include "K2Node_MakeArray.h"

#define LOCTEXT_NAMESPACE "DialogueEventNode"

UBPNode_DialogueOutcomesEvent::UBPNode_DialogueOutcomesEvent(const FObjectInitializer& ObjectInitializer) : UBPNode_DialogueBase(ObjectInitializer)
{
	LibraryFunctionName = GET_FUNCTION_NAME_CHECKED(UDialogueBlueprintLibrary, MakeDialogueDummy);
	NumOutcomes = 1;
}

FName UBPNode_DialogueOutcomesEvent::GetOutcomePinName(const int32 PinIndex) const
{
	return *FString::Printf(TEXT("%d"), PinIndex + 1);
}

int32 UBPNode_DialogueOutcomesEvent::GetOutcomeBeginIndex() const
{
	int32 BeginIndex = 0;

	for (UEdGraphPin* Pin : Pins)
	{
		if (Pin->Direction == EGPD_Output && UDialogueNodesUtils::IsDialogueEventPinType(Pin->PinType))
		{
			break;
		}
		BeginIndex++;
	}

	return BeginIndex;
}

UEdGraphPin* UBPNode_DialogueOutcomesEvent::GetOutcomePin(const int32 OutcomeIndex) const
{
	return GetPinAt(OutcomeIndex + GetOutcomeBeginIndex());
}

void UBPNode_DialogueOutcomesEvent::AllocateDefaultPins()
{
	UBPNode_DialogueBase::AllocateDefaultPins();

	for (int32 OutcomeIndex = 0; OutcomeIndex < NumOutcomes; OutcomeIndex++)
	{
		CreatePin(EGPD_Output, UDialogueNodesUtils::DialogueEventPinType(), GetOutcomePinName(OutcomeIndex));
	}
}

void UBPNode_DialogueOutcomesEvent::InteractiveAddInputPin()
{
	FScopedTransaction Transaction(LOCTEXT("AddPinTx", "Add Pin"));
	AddInputPin();
}

void UBPNode_DialogueOutcomesEvent::AddInputPin()
{
	Modify();
	
	CreatePin(EGPD_Output, UDialogueNodesUtils::DialogueEventPinType(), GetOutcomePinName(NumOutcomes));

	NumOutcomes++;

	const bool bIsCompiling = GetBlueprint()->bBeingCompiled;
	if (!bIsCompiling)
	{
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
	}
}

void UBPNode_DialogueOutcomesEvent::SyncOutcomePinNames()
{
	int32 BeginIndex = GetOutcomeBeginIndex();
	
	for (int32 OutIndex = 0; OutIndex < NumOutcomes; OutIndex++)
	{
		UEdGraphPin* CurrentPin = Pins[BeginIndex + OutIndex];
		CurrentPin->Modify();
		CurrentPin->PinName = GetOutcomePinName(OutIndex);
	}
}

void UBPNode_DialogueOutcomesEvent::RemoveOutcomePin(UEdGraphPin* Pin)
{
	check(Pin->Direction == EGPD_Output);
	check(Pin->ParentPin == nullptr);
	checkSlow(Pins.Contains(Pin));

	FScopedTransaction Transaction(LOCTEXT("RemovePinTx", "Remove Pin"));
	Modify();

	TFunction<void(UEdGraphPin*)> RemoveOutPin = [this, &RemoveOutPin](UEdGraphPin* PinToRemove)
	{
		int32 PinRemovalIndex = INDEX_NONE;
		if (Pins.Find(PinToRemove, PinRemovalIndex))
		{
			Pins.RemoveAt(PinRemovalIndex);
			PinToRemove->MarkPendingKill();
		}
	};

	RemoveOutPin(Pin);

	NumOutcomes--;
	SyncOutcomePinNames();
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
}

void UBPNode_DialogueOutcomesEvent::AutoCreateOptionNode(UEdGraphPin* OutcomePin)
{
	UBPNode_DialogueOption* NewOptionNode = UDialogueNodesUtils::AutoCreateDialogueNode<UBPNode_DialogueOption>(OutcomePin);
}

void UBPNode_DialogueOutcomesEvent::AutoCreateLineNode(UEdGraphPin* OutcomePin)
{
	UBPNode_DialogueLine* NewLineNode = UDialogueNodesUtils::AutoCreateDialogueNode<UBPNode_DialogueLine>(OutcomePin);

	if (UBPNode_DialogueLine* ThisAsLine = dynamic_cast<UBPNode_DialogueLine*>(this))
	{
		NewLineNode->MetadataParameters.Append(ThisAsLine->MetadataParameters);
	}
	else if (UBPNode_DialogueOption* ThisAsOption = dynamic_cast<UBPNode_DialogueOption*>(this))
	{
		if (ThisAsOption->GetInPin()->LinkedTo.Num())
		{
			if (UBPNode_DialogueLine* PreviousAsLine = dynamic_cast<UBPNode_DialogueLine*>(ThisAsOption->GetInPin()->LinkedTo[0]->GetOwningNode()))
			{
				NewLineNode->MetadataParameters.Append(PreviousAsLine->MetadataParameters);
			}
		}
	}
}

void UBPNode_DialogueOutcomesEvent::AutoAddOptionPin()
{
	AddInputPin();
	AutoCreateOptionNode(GetOutcomePin(NumOutcomes - 1));
}

void UBPNode_DialogueOutcomesEvent::AutoAddLinePin()
{
	AddInputPin();
	AutoCreateLineNode(GetOutcomePin(NumOutcomes - 1));
}

void UBPNode_DialogueOutcomesEvent::GetContextMenuActions(const FGraphNodeContextMenuBuilder& Context) const
{
	if (!Context.bIsDebugging)
	{
		FName DialogueEventName = FName("DialogueOutcomes");
		FText DialogueEventStr = LOCTEXT("DialogueOutcomes", "Dialogue Outcomes");
		if (Context.Pin != NULL && UDialogueNodesUtils::IsDialogueEventPinType(Context.Pin->PinType) && Context.Pin->Direction == EGPD_Output)
		{
			Context.MenuBuilder->BeginSection(DialogueEventName, DialogueEventStr);
			Context.MenuBuilder->AddMenuEntry(
				LOCTEXT("RemovePin", "Remove outcome pin"),
				LOCTEXT("RemovePinTooltip", "Remove this pin"),
				FSlateIcon(),
				FUIAction(
					FExecuteAction::CreateUObject(this, &UBPNode_DialogueOutcomesEvent::RemoveOutcomePin, const_cast<UEdGraphPin*>(Context.Pin))
				)
			);
			Context.MenuBuilder->AddMenuEntry(
				LOCTEXT("CreateOption", "Link new option"),
				LOCTEXT("CreateOptionTooltip", "Create and link a new Dialogue option (player) node"),
				FSlateIcon(),
				FUIAction(
					FExecuteAction::CreateUObject(this, &UBPNode_DialogueOutcomesEvent::AutoCreateOptionNode, const_cast<UEdGraphPin*>(Context.Pin))
				)
			);
			Context.MenuBuilder->AddMenuEntry(
				LOCTEXT("CreateLine", "Link new line"),
				LOCTEXT("CreateLineTooltip", "Create and link a new Dialogue line (npc) node"),
				FSlateIcon(),
				FUIAction(
					FExecuteAction::CreateUObject(this, &UBPNode_DialogueOutcomesEvent::AutoCreateLineNode, const_cast<UEdGraphPin*>(Context.Pin))
				)
			);
			Context.MenuBuilder->EndSection();
		}
		else if (CanAddPin())
		{
			Context.MenuBuilder->BeginSection(DialogueEventName, DialogueEventStr);
			Context.MenuBuilder->AddMenuEntry(
				LOCTEXT("AddPin", "Add outcome pin"),
				LOCTEXT("AddPinTooltip", "Add another outcome pin"),
				FSlateIcon(),
				FUIAction(
					FExecuteAction::CreateUObject(this, &UBPNode_DialogueOutcomesEvent::AddInputPin)
				)
			);
			Context.MenuBuilder->AddMenuEntry(
				LOCTEXT("AddOptionPin", "Create option"),
				LOCTEXT("AddOptionPinTooltip", "Create a new Dialogue option (player) connected to a new pin."),
				FSlateIcon(),
				FUIAction(
					FExecuteAction::CreateUObject(this, &UBPNode_DialogueOutcomesEvent::AutoAddOptionPin)
				)
			);
			Context.MenuBuilder->AddMenuEntry(
				LOCTEXT("AddLinePin", "Create line"),
				LOCTEXT("AddLinePinTooltip", "Create a new Dialogue line (npc) connected to a new pin."),
				FSlateIcon(),
				FUIAction(
					FExecuteAction::CreateUObject(this, &UBPNode_DialogueOutcomesEvent::AutoAddLinePin)
				)
			);
			Context.MenuBuilder->EndSection();
		}
	}

	Super::GetContextMenuActions(Context);
}

TArray<UK2Node*> UBPNode_DialogueOutcomesEvent::ExpandDialogueNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph, TArray<UK2Node*> CreatedNodes)
{
	CreatedNodes = UBPNode_DialogueBase::ExpandDialogueNode(CompilerContext, SourceGraph, CreatedNodes);

	for (int32 OutcomeIndex = NumOutcomes - 1; OutcomeIndex >= 0; OutcomeIndex--)
	{
		UEdGraphPin* OutcomePin = GetOutcomePin(OutcomeIndex);
		if (OutcomePin->LinkedTo.Num() == 0)
		{
			RemoveOutcomePin(OutcomePin);
		}
	}
	
	UEdGraphPin* OutcomesArrayPin = FunctionNode->FindPinChecked(FName("Outcomes"));

	///////////////////////////////////////
	//	Make outcomes array node
	
	UK2Node_MakeArray* ArrayNode = CompilerContext.SpawnIntermediateNode<UK2Node_MakeArray>(this, SourceGraph);
	ArrayNode->NumInputs = NumOutcomes;
	ArrayNode->AllocateDefaultPins();
	ArrayNode->GetOutputPin()->MakeLinkTo(OutcomesArrayPin);
	ArrayNode->PostReconstructNode();
	ArrayNode->NodePosX = NodePosX;
	ArrayNode->NodePosY = NodePosY;
	CreatedNodes.Add(ArrayNode);

	TArray<UBPNode_DialogueBase*> NodesToExpand;

	for (int32 OutcomeIndex = 0; OutcomeIndex < NumOutcomes; OutcomeIndex++)
	{
		UEdGraphPin* OtherPin = GetOutcomePin(OutcomeIndex)->LinkedTo[0];
		
		if (UDialogueNodesUtils::IsDialogueEventPinType(OtherPin->PinType))
		{
			ArrayNode->GetPinAt(OutcomeIndex + 1)->MakeLinkTo(OtherPin);

			if (UBPNode_DialogueBase* OtherNode = dynamic_cast<UBPNode_DialogueBase*>(OtherPin->GetOwningNode()))
			{
				NodesToExpand.Add(OtherNode);
			}
		}
		else
		{
			UK2Node_CallFunction* SourceNode = dynamic_cast<UK2Node_CallFunction*>(OtherPin->GetOwningNode());

			if (!CreatedNodes.Contains(SourceNode))
			{
				CompilerContext.MessageLog.Error(*LOCTEXT("DialogueTreeMerging_Error", "Dialogue error : different dialogue trees are merging at @@, which is forbidden.").ToString(), this);
			}

			bool bShouldCreateGotoLink = false;
			for (UEdGraphPin* SourceNodePin : SourceNode->Pins)
			{
				if (SourceNodePin->GetName() == "Outcomes")
				{
					bShouldCreateGotoLink = true;
					break;
				}
			}

			if (bShouldCreateGotoLink)
			{
				int32 GotoReference = SourceGraph->Nodes.Find(this) + SourceGraph->Nodes.Num() + OutcomeIndex;

				///////////////////////////////////////
				//	Make goto origin node

				FName DialogueGoto_FunctionName = GET_FUNCTION_NAME_CHECKED(UDialogueBlueprintLibrary, MakeDialogueGoto);
				UK2Node_CallFunction* OriginNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
				OriginNode->FunctionReference.SetExternalMember(DialogueGoto_FunctionName, UDialogueBlueprintLibrary::StaticClass());
				OriginNode->AllocateDefaultPins();
				OriginNode->FindPinChecked(FName("Reference"))->DefaultValue = FString::FromInt(GotoReference);
				OriginNode->FindPinChecked(FName("bIsDestination"))->DefaultValue = FString::FromInt(0);
				OriginNode->GetReturnValuePin()->MakeLinkTo(ArrayNode->GetPinAt(OutcomeIndex + 1));
				OriginNode->NodePosX = NodePosX;
				OriginNode->NodePosY = NodePosY;
				CreatedNodes.Add(OriginNode);

				///////////////////////////////////////
				//	Make goto destination node

				UK2Node_CallFunction* DestinationNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
				DestinationNode->FunctionReference.SetExternalMember(DialogueGoto_FunctionName, UDialogueBlueprintLibrary::StaticClass());
				DestinationNode->AllocateDefaultPins();
				DestinationNode->FindPinChecked(FName("Reference"))->DefaultValue = FString::FromInt(GotoReference);
				DestinationNode->FindPinChecked(FName("bIsDestination"))->DefaultValue = FString::FromInt(1);
				DestinationNode->NodePosX = NodePosX;
				DestinationNode->NodePosY = NodePosY;
				CreatedNodes.Add(DestinationNode);

				UK2Node_MakeArray* SourceArrayNode = dynamic_cast<UK2Node_MakeArray*>(SourceNode->FindPinChecked(FName("Outcomes"))->LinkedTo[0]->GetOwningNode());
				SourceArrayNode->AddInputPin();
				DestinationNode->GetReturnValuePin()->MakeLinkTo(SourceArrayNode->GetPinAt(SourceArrayNode->NumInputs));
			}
			else
			{
				ArrayNode->GetPinAt(OutcomeIndex + 1)->MakeLinkTo(OtherPin);
			}
		}
	}

	BreakAllNodeLinks();

	for (UBPNode_DialogueBase* NodeToExpand : NodesToExpand)
	{
		CreatedNodes = NodeToExpand->ExpandDialogueNode(CompilerContext, SourceGraph, CreatedNodes);
	}

	return CreatedNodes;
}

#undef LOCTEXT_NAMESPACE