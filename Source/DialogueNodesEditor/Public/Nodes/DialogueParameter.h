// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphPin.h"
#include "DialogueNodesUtils.h"

#include "DialogueParameter.generated.h"

USTRUCT()
struct DIALOGUENODESEDITOR_API FDialogueParameter
{
	GENERATED_BODY()
	
	UPROPERTY()
	FName Name;

	UPROPERTY()
	EDialoguePinType Type;

	UPROPERTY()
	bool bHasPin;

	UEdGraphPin* Pin = nullptr;
	
	FEdGraphPinType GetPinType() { return UDialogueNodesUtils::GetPinType(Type); }
	FDialogueParameter GetWithoutPin() { return FDialogueParameter(Name, false, Type, nullptr); }
	FDialogueParameter GetWithPin(UEdGraphPin* PinIn) { return FDialogueParameter(Name, true, Type, PinIn); }

	FDialogueParameter() : FDialogueParameter(NAME_None, false, EDialoguePinType::PT_Text, nullptr) { }
	FDialogueParameter(FName NameIn, bool bHasPinIn, EDialoguePinType TypeIn, UEdGraphPin* PinIn) : Name(NameIn), bHasPin(bHasPinIn), Type(TypeIn), Pin(PinIn) { }
};
