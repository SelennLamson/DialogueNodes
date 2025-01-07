// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SGraphPin.h"
#include "KismetPins/SGraphPinExec.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraphSchema.h"
#include "EdGraphSchema_K2.h"
#include "EdGraphUtilities.h"
#include "DialogueNodesUtils.h"

class DIALOGUENODESEDITOR_API SDialoguePin : public SGraphPinExec
{
public:
	SLATE_BEGIN_ARGS(SDialoguePin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin)
	{
		SGraphPin::Construct(SGraphPin::FArguments(), InPin);

		// Call utility function so inheritors can also call it since arguments can't be passed through
		CachePinIcons();
	}

	virtual FSlateColor GetPinColor() const override
	{
		return FSlateColor(UDialogueNodesUtils::DialoguePinsColor());
	}
};

class FDialoguePinFactory : public FGraphPanelPinFactory
{
	virtual TSharedPtr<class SGraphPin> CreatePin(class UEdGraphPin* InPin) const override
	{
		
		if (UDialogueNodesUtils::IsDialogueEventPinType(InPin->PinType))
		{
			return SNew(SDialoguePin, InPin);
		}
		return nullptr;
	}
};