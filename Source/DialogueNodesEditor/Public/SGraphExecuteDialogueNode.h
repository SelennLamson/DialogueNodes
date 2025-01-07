// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "KismetNodes/SGraphNodeK2Default.h"
#include "Nodes/BPNode_ExecuteDialogue.h"
#include "Styling/SlateStyle.h"
#include "EdGraphUtilities.h"

class DIALOGUENODESEDITOR_API SGraphExecuteDialogueNode : public SGraphNodeK2Default
{
	SLATE_BEGIN_ARGS(SGraphExecuteDialogueNode) {}
	SLATE_END_ARGS()

public:
	SGraphExecuteDialogueNode() : SGraphNodeK2Default() {}

	void Construct(const FArguments& InArgs, UEdGraphNode* InNode);
	virtual void CreatePinWidgets() override;
};

class FExecuteDialogueNodeFactory : public FGraphPanelNodeFactory
{
	virtual TSharedPtr<class SGraphNode> CreateNode(class UEdGraphNode* Node) const override
	{
		UBPNode_ExecuteDialogue* DialogueNode = dynamic_cast<UBPNode_ExecuteDialogue*>(Node);

		if (DialogueNode)
		{
			return SNew(SGraphExecuteDialogueNode, Node);
		}
		return NULL;
	}
};