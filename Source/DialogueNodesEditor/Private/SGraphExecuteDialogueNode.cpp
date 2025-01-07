// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "SGraphExecuteDialogueNode.h"

#include "SlateOptMacros.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Images/SImage.h"
#include "NodeFactory.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

#define LOCTEXT_NAMESPACE "SGraphExecuteDialogueNode"

void SGraphExecuteDialogueNode::Construct(const FArguments& InArgs, UEdGraphNode* InNode)
{
	if (UK2Node* K2Node = Cast<UK2Node>(InNode))
	{
		SGraphNodeK2Default::Construct(SGraphNodeK2Default::FArguments(), K2Node);
	}
}

void SGraphExecuteDialogueNode::CreatePinWidgets()
{
	UBPNode_ExecuteDialogue* Node = CastChecked<UBPNode_ExecuteDialogue>(GraphNode);

	for (auto PinIt = GraphNode->Pins.CreateConstIterator(); PinIt; ++PinIt)
	{
		UEdGraphPin* Pin = *PinIt;

		if (Pin == Node->GetSelectExec() || Pin == Node->GetStopExec())
		{
			LeftNodeBox->AddSlot()
				.AutoHeight()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				.Padding(1.0f)
				[
					SNew(SImage)
					.Image(FEditorStyle::GetBrush("Graph.Pin.DefaultPinSeparator"))
				];
		}
		else if (Pin == Node->GetEndExec())
		{
			RightNodeBox->AddSlot()
				.AutoHeight()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Center)
				.Padding(1.0f)
				[
					SNew(SImage)
					.Image(FEditorStyle::GetBrush("Graph.Pin.DefaultPinSeparator"))
				];
		}

		TSharedPtr<SGraphPin> NewPin = FNodeFactory::CreatePinWidget(Pin);
		check(NewPin.IsValid());
		
		this->AddPin(NewPin.ToSharedRef());
	}
}

#undef LOCTEXT_NAMESPACE

END_SLATE_FUNCTION_BUILD_OPTIMIZATION