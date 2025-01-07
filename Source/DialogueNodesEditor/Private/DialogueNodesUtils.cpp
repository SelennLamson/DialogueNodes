// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "DialogueNodesUtils.h"
#include "Nodes/BPNode_DialogueBase.h"
#include "EdGraph/EdGraphNode.h"
#include "Events/DialogueTextEvent.h"
#include "Events/DialogueOption.h"

FEdGraphPinType UDialogueNodesUtils::GetPinType(EDialoguePinType DialogPin)
{
	switch (DialogPin)
	{
		case EDialoguePinType::PT_Condition:
			return ConditionPinType();
		case EDialoguePinType::PT_Text:
			return TextPinType();
		case EDialoguePinType::PT_Dialogue:
			return DialoguePinType();
		case EDialoguePinType::PT_DialogueEvent:
			return DialogueEventPinType();
		case EDialoguePinType::PT_Asset:
			return AssetPinType();
		case EDialoguePinType::PT_Name:
		default:
			return NamePinType();
	}
}

FEdGraphPinType UDialogueNodesUtils::DialoguePinType()
{
	FEdGraphPinType PinType = FEdGraphPinType();
	PinType.PinCategory = UEdGraphSchema_K2::PC_Object;
	PinType.PinSubCategoryObject = UDialogue::StaticClass();
	return PinType;
}

FEdGraphPinType UDialogueNodesUtils::DialogueEventPinType()
{
	FEdGraphPinType PinType = FEdGraphPinType();
	PinType.PinCategory = UEdGraphSchema_K2::PC_Exec;
	PinType.PinSubCategory = "Dialogue";
	return PinType;
}

FEdGraphPinType UDialogueNodesUtils::ConditionPinType()
{
	FEdGraphPinType PinType = FEdGraphPinType();
	PinType.PinCategory = UEdGraphSchema_K2::PC_Boolean;
	return PinType;
}

FEdGraphPinType UDialogueNodesUtils::NamePinType()
{
	FEdGraphPinType PinType = FEdGraphPinType();
	PinType.PinCategory = UEdGraphSchema_K2::PC_Name;
	return PinType;
}

FEdGraphPinType UDialogueNodesUtils::TextPinType()
{
	FEdGraphPinType PinType = FEdGraphPinType();
	PinType.PinCategory = UEdGraphSchema_K2::PC_Text;
	return PinType;
}

FEdGraphPinType UDialogueNodesUtils::AssetPinType()
{
	FEdGraphPinType PinType = FEdGraphPinType();
	PinType.PinCategory = UEdGraphSchema_K2::PC_Object;
	return PinType;
}

FEdGraphPinType UDialogueNodesUtils::LineStructPinType()
{
	FEdGraphPinType PinType = FEdGraphPinType();
	PinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
	PinType.PinSubCategoryObject = FLineStruct::StaticStruct();
	return PinType;
}

FEdGraphPinType UDialogueNodesUtils::OptionStructArrayPinType()
{
	FEdGraphPinType PinType = FEdGraphPinType();
	PinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
	PinType.ContainerType = EPinContainerType::Array;
	PinType.PinSubCategoryObject = FOptionStruct::StaticStruct();
	return PinType;
}

bool UDialogueNodesUtils::IsDialogueEventPinType(FEdGraphPinType PinType)
{
	FEdGraphPinType EventType = DialogueEventPinType();
	return EventType.PinCategory == PinType.PinCategory && EventType.PinSubCategory == PinType.PinSubCategory;
}

 FLinearColor UDialogueNodesUtils::DialogueNodesTitleColor()
{
	return FLinearColor(1.0f, 0.4f, 0.1f);
}

FLinearColor UDialogueNodesUtils::DialogueOptionTitleColor()
{
	return FLinearColor(1.0f, 0.1f, 0.4f);
}

FLinearColor UDialogueNodesUtils::DialoguePinsColor()
{
	return FLinearColor(1.0f, 0.4f, 0.1f);
}