// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "KismetNodes/SGraphNodeK2Default.h"
#include "Nodes/BPNode_DialogueBase.h"
#include "Styling/SlateStyle.h"
#include "EdGraphUtilities.h"
#include "Widgets/Text/SRichTextBlock.h"

class DIALOGUENODESEDITOR_API SGraphDialogueNode : public SGraphNodeK2Default
{
	SLATE_BEGIN_ARGS(SGraphDialogueNode) {}
	SLATE_END_ARGS()

public:
	TSharedPtr<SRichTextBlock> RichTextBlock;
	TSharedPtr<FSlateStyleSet> StyleInstance;

public:
	SGraphDialogueNode() : SGraphNodeK2Default() {}

	void Construct(const FArguments& InArgs, UEdGraphNode* InNode);
	
	// Text box
	virtual void CreateBelowPinControls(TSharedPtr<SVerticalBox> MainBox) override;
	FText GetTextFromNode() const;

	// Add pin button
	virtual void CreateInputSideAddButton(TSharedPtr<SVerticalBox> InputBox) override;
	virtual FReply OnAddPin() override;
	virtual EVisibility IsAddPinButtonVisible() const override;
};

class FDialogueNodeFactory : public FGraphPanelNodeFactory
{
	virtual TSharedPtr<class SGraphNode> CreateNode(class UEdGraphNode* Node) const override
	{
		UBPNode_DialogueBase* DialogueNode = dynamic_cast<UBPNode_DialogueBase*>(Node);

		if (DialogueNode)
		{
			return SNew(SGraphDialogueNode, Node);
		}
		return NULL;
	}
};