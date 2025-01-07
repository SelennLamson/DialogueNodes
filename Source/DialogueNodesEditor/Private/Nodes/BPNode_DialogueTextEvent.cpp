// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "Nodes/BPNode_DialogueTextEvent.h"
#include "DialogueBlueprintLibrary.h"
#include "DialogueNodesUtils.h"
#include "DialogueSettings.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "EdGraph/EdGraphNode.h"
#include "K2Node_MakeMap.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"

#include "KismetCompiler.h"

#define LOCTEXT_NAMESPACE "DialogueTextNode"

UBPNode_DialogueTextEvent::UBPNode_DialogueTextEvent(const FObjectInitializer& ObjectInitializer) : UBPNode_DialogueOutcomesEvent(ObjectInitializer)
{
	BaseParameters.Add(FDialogueParameter("Condition", false, EDialoguePinType::PT_Condition, nullptr));
	BaseParameters.Add(FDialogueParameter("Text", false, EDialoguePinType::PT_Text, nullptr));
	bTextProcessed = false;
}

void UBPNode_DialogueTextEvent::PostPlacedNewNode()
{
	UpdateMetadataParameters();
	UBPNode_DialogueOutcomesEvent::PostPlacedNewNode();
}

void UBPNode_DialogueTextEvent::ReconstructNode()
{
	UpdateMetadataParameters();
	UBPNode_DialogueBase::ReconstructNode();
}

TMap<FName, FString> UBPNode_DialogueTextEvent::GetDefaultMetadata() const
{
	const UDialogueSettings* DialogueSettings = GetDefault<UDialogueSettings>();
	return DialogueSettings->LineMetadata;
}

void UBPNode_DialogueTextEvent::UpdateMetadataParameters()
{
	TMap<FName, FString> Default = GetDefaultMetadata();

	for (auto It = MetadataParameters.CreateIterator(); It; ++It)
	{
		if (!Default.Contains(It.Key()) && It.Value() == "")
		{
			It.RemoveCurrent();
		}
	}

	for (auto It = Default.CreateIterator(); It; ++It)
	{
		if (!MetadataParameters.Contains(It.Key()))
		{
			MetadataParameters.Add(It.Key(), It.Value());
		}
	}
}

void UBPNode_DialogueTextEvent::ProcessTextParams()
{
	bTextProcessed = false;

	TArray<FString> Strs;
	MetadataParameters.GenerateValueArray(Strs);
	Strs.Add(Text.ToString());

	bool bTextParamAdded = false;

	for (FString Str : Strs)
	{
		FString TextParamName = "";
		bool bIsInBrackets = false;

		for (char c : Str.GetCharArray())
		{
			if (c == '[')
			{
				TextParamName = "";
				bIsInBrackets = true;
			}
			else if (c == ']' && bIsInBrackets)
			{
				if (TextParamName != "") // TODO : better checks on custom param name (make it a real Blueprint variable name)
				{
					bTextParamAdded |= RegisterTextParam(FName(*TextParamName));
				}
				bIsInBrackets = false;
			}
			else if (bIsInBrackets)
			{
				TextParamName += c;
			}
		}
	}

	if (bTextParamAdded)
	{
		ReconstructNode();
	}
}

bool UBPNode_DialogueTextEvent::RegisterTextParam(FName ParamName)
{
	for (FDialogueParameter Param : BaseParameters)
	{
		if (Param.Name == ParamName)
		{
			return false;
		}
	}

	for (FDialogueParameter Param : TextParameters)
	{
		if (Param.Name == ParamName)
		{
			return false;
		}
	}
	
	TextParameters.Add(FDialogueParameter(ParamName, true, EDialoguePinType::PT_Text, nullptr));
	return true;
}

void UBPNode_DialogueTextEvent::DeleteTextPin(UEdGraphPin* ParamPin)
{
	int32 ToRemove = -1;
	for (int32 ParamIndex = 0; ParamIndex < TextParameters.Num(); ParamIndex++)
	{
		FDialogueParameter Param = TextParameters[ParamIndex];

		if (Param.Pin == ParamPin)
		{
			ToRemove = ParamIndex;
			break;
		}
	}

	if (ToRemove > -1)
	{
		TextParameters.RemoveAt(ToRemove);
		ReconstructNode();
	}

	bTextProcessed = false;
}

FText UBPNode_DialogueTextEvent::GetProcessedText()
{
	if (!bTextProcessed)
	{
		TMap<FName, FString> ParamValues;

		for (FDialogueParameter Param : TextParameters)
		{
			if (Param.Pin && Param.Pin->LinkedTo.Num() == 0 && Param.Pin->DefaultTextValue.ToString() != "")
			{
				ParamValues.Add(Param.Name, Param.Pin->DefaultTextValue.ToString());
			}
			else
			{
				ParamValues.Add(Param.Name, "*" + Param.Name.ToString() + "*");
			}
		}

		ProcessedText = FText::AsCultureInvariant(UDialogueBlueprintLibrary::ProcessString(Text.ToString(), ParamValues));
		bTextProcessed = true;
	}

	return ProcessedText;
}

bool UBPNode_DialogueTextEvent::IsTextParamPin(const UEdGraphPin* Pin) const
{
	if (!Pin)
	{
		return false;
	}

	for (FDialogueParameter Param : TextParameters)
	{
		if (Param.Pin == Pin)
		{
			return true;
		}
	}
	return false;
}

void UBPNode_DialogueTextEvent::GetContextMenuActions(const FGraphNodeContextMenuBuilder& Context) const
{
	if (!Context.bIsDebugging)
	{
		bool bRemoveTextParam = false;
		bool bReconstructPins = false;

		if (IsTextParamPin(Context.Pin))
		{
			bRemoveTextParam = true;
		}
		else if (Context.Pin == NULL || Context.Pin->Direction == EGPD_Input)
		{
			bReconstructPins = true;
		}

		if (bReconstructPins || bRemoveTextParam)
		{
			Context.MenuBuilder->BeginSection(FName("TextParameters"), LOCTEXT("TextParameters", "Text Parameters"));
			if (bRemoveTextParam)
			{
				Context.MenuBuilder->AddMenuEntry(
					LOCTEXT("DeleteParam", "Delete parameter"),
					LOCTEXT("DeleteParamTooltip", "Delete this text parameter"),
					FSlateIcon(),
					FUIAction(
						FExecuteAction::CreateUObject(this, &UBPNode_DialogueTextEvent::DeleteTextPin, const_cast<UEdGraphPin*>(Context.Pin))
					)
				);
			}
			if (bReconstructPins)
			{
				Context.MenuBuilder->AddMenuEntry(
					LOCTEXT("ProcessText", "Create text pins"),
					LOCTEXT("ProcessTextTooltip", "Generate the pins for text parameters."),
					FSlateIcon(),
					FUIAction(
						FExecuteAction::CreateUObject(this, &UBPNode_DialogueTextEvent::ProcessTextParams)
					)
				);
			}
			Context.MenuBuilder->EndSection();
		}
	}

	Super::GetContextMenuActions(Context);
}

void UBPNode_DialogueTextEvent::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (PropertyName == "Text" || PropertyName == "MetadataParameters")
	{
		ProcessTextParams();
		UK2Node::PostEditChangeProperty(PropertyChangedEvent);
	}
	else
	{
		UBPNode_DialogueOutcomesEvent::PostEditChangeProperty(PropertyChangedEvent);
	}
}

void UBPNode_DialogueTextEvent::PinConnectionListChanged(UEdGraphPin* Pin)
{
	if (IsTextParamPin(Pin))
	{
		bTextProcessed = false;
	}

	UK2Node::PinConnectionListChanged(Pin);
}

void UBPNode_DialogueTextEvent::PinDefaultValueChanged(UEdGraphPin* Pin)
{
	if (IsTextParamPin(Pin))
	{
		bTextProcessed = false;
	}

	UK2Node::PinDefaultValueChanged(Pin);
}

TArray<UK2Node*> UBPNode_DialogueTextEvent::ExpandDialogueNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph, TArray<UK2Node*> CreatedNodes)
{
	CreatedNodes = UBPNode_DialogueOutcomesEvent::ExpandDialogueNode(CompilerContext, SourceGraph, CreatedNodes);

	if (UEdGraphPin* MetadataPin = FunctionNode->FindPin(FName("MetadataParameters")))
	{
		UK2Node_MakeMap* MetadataMapNode = CompilerContext.SpawnIntermediateNode<UK2Node_MakeMap>(this);
		MetadataMapNode->NumInputs = MetadataParameters.Num();
		MetadataMapNode->AllocateDefaultPins();
		MetadataMapNode->GetOutputPin()->MakeLinkTo(MetadataPin);
		MetadataMapNode->PostReconstructNode();
		MetadataMapNode->NodePosX = NodePosX;
		MetadataMapNode->NodePosY = NodePosY;
		CreatedNodes.Add(MetadataMapNode);

		int32 Index = 0;
		for (TPair<FName, FString> Metadata : MetadataParameters)
		{
			MetadataMapNode->GetPinAt(Index * 2 + 1)->DefaultValue = Metadata.Key.ToString();
			MetadataMapNode->GetPinAt(Index * 2 + 2)->DefaultValue = Metadata.Value;
			Index++;
		}
	}

	if (UEdGraphPin* SoundPin = FunctionNode->FindPin(FName("Sound")))
	{
		SoundPin->DefaultObject = Sound;
	}

	if (UEdGraphPin* WavePin = FunctionNode->FindPin(FName("DialogueWave")))
	{
		WavePin->DefaultObject = DialogueWave;
	}

	return CreatedNodes;
}

//////////////////////////////////////////////////
///////		UBPNode_DialogueOption

UBPNode_DialogueOption::UBPNode_DialogueOption(const FObjectInitializer& ObjectInitializer) : UBPNode_DialogueTextEvent(ObjectInitializer)
{
	LibraryFunctionName = GET_FUNCTION_NAME_CHECKED(UDialogueBlueprintLibrary, MakeDialogueOption);

	BaseParameters.Add(FDialogueParameter("AvailabilityCondition", false, EDialoguePinType::PT_Condition, nullptr));
}

FText UBPNode_DialogueOption::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	const UDialogueSettings* DialogueSettings = GetDefault<UDialogueSettings>();
	FName TitleParameter = DialogueSettings->OptionTitleParameter;

	if (MetadataParameters.Contains(TitleParameter))
	{
		FString Value = MetadataParameters[TitleParameter];
		
		if (Value != "" && !Value.Contains("[") && !Value.Contains("]"))
		{
			return FText::AsCultureInvariant(Value + " - Dialogue Option (Player)");
		}
	}

	return LOCTEXT("DialogueOption", "Dialogue Option (Player)");
}

FLinearColor UBPNode_DialogueOption::GetNodeTitleColor() const
{
	const UDialogueSettings* DialogueSettings = GetDefault<UDialogueSettings>();
	FName TitleParameter = DialogueSettings->OptionTitleParameter;

	if (MetadataParameters.Contains(TitleParameter))
	{
		FString Value = MetadataParameters[TitleParameter];
		if (DialogueSettings->OptionColorMap.Contains(Value))
		{
			return DialogueSettings->OptionColorMap[Value];
		}
	}

	return UDialogueNodesUtils::DialogueOptionTitleColor();
}

TMap<FName, FString> UBPNode_DialogueOption::GetDefaultMetadata() const
{
	const UDialogueSettings* DialogueSettings = GetDefault<UDialogueSettings>();
	return DialogueSettings->OptionMetadata;
}

void UBPNode_DialogueOption::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* ActionKey = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

//////////////////////////////////////////////////
///////		UBPNode_DialogueLine

UBPNode_DialogueLine::UBPNode_DialogueLine(const FObjectInitializer& ObjectInitializer) : UBPNode_DialogueTextEvent(ObjectInitializer)
{
	LibraryFunctionName = GET_FUNCTION_NAME_CHECKED(UDialogueBlueprintLibrary, MakeDialogueLine);
}

FText UBPNode_DialogueLine::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	const UDialogueSettings* DialogueSettings = GetDefault<UDialogueSettings>();
	FName TitleParameter = DialogueSettings->LineTitleParameter;

	if (MetadataParameters.Contains(TitleParameter))
	{
		FString Value = MetadataParameters[TitleParameter];

		if (Value != "" && !Value.Contains("[") && !Value.Contains("]"))
		{
			return FText::AsCultureInvariant(Value + " - Dialogue Line (NPC)");
		}
	}

	return LOCTEXT("DialogueLine", "Dialogue Line (NPC)");
}

FLinearColor UBPNode_DialogueLine::GetNodeTitleColor() const
{
	const UDialogueSettings* DialogueSettings = GetDefault<UDialogueSettings>();
	FName TitleParameter = DialogueSettings->LineTitleParameter;

	if (MetadataParameters.Contains(TitleParameter))
	{
		FString Value = MetadataParameters[TitleParameter];
		if (DialogueSettings->LineColorMap.Contains(Value))
		{
			return DialogueSettings->LineColorMap[Value];
		}
	}

	return UDialogueNodesUtils::DialogueNodesTitleColor();
}

void UBPNode_DialogueLine::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* ActionKey = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

#undef LOCTEXT_NAMESPACE