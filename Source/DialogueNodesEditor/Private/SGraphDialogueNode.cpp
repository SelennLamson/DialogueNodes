// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "SGraphDialogueNode.h"

#include "Nodes/BPNode_DialogueBase.h"
#include "Nodes/BPNode_DialogueTextEvent.h"
#include "DialogueSettings.h"

#include "Components/RichTextBlock.h"
#include "Framework/Text/RichTextLayoutMarshaller.h"
#include "Framework/Text/RichTextMarkupProcessing.h"

#include "EdGraph/EdGraph.h"
#include "Engine/DataTable.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Images/SImage.h"
#include "SlateOptMacros.h"
#include "GraphEditorSettings.h"
#include "ScopedTransaction.h"
#include "Widgets/SViewport.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

#define LOCTEXT_NAMESPACE "SGraphDialogueNode"

void SGraphDialogueNode::CreateBelowPinControls(TSharedPtr<SVerticalBox> MainBox)
{
	SGraphNodeK2Default::CreateBelowPinControls(MainBox);

	UBPNode_DialogueBase* DialogueNode = Cast<UBPNode_DialogueBase>(GraphNode);
	UBPNode_DialogueTextEvent* DialogueTextNode = Cast<UBPNode_DialogueTextEvent>(GraphNode);

	if (DialogueTextNode)
	{
		TAttribute<FText> Value = TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateRaw(this, &SGraphDialogueNode::GetTextFromNode));

		const UDialogueSettings* DialogueSettings = GetDefault<UDialogueSettings>();
		UDataTable* TextStyleSet = DialogueSettings->GetTextStyleSet();

		FTextBlockStyle DefaultTextStyle = FTextBlockStyle();
		TArray<TSharedRef<class ITextDecorator>> CreatedDecorators;

		if (!StyleInstance)
		{
			StyleInstance = MakeShareable(new FSlateStyleSet(TEXT("RichTextStyle")));

			if (TextStyleSet && TextStyleSet->RowStruct->IsChildOf(FRichTextStyleRow::StaticStruct()))
			{
				for (const auto& Entry : TextStyleSet->RowMap) // To change for UE-4.21 TextStyleSet->GetRowMap())
				{
					FName SubStyleName = Entry.Key;
					FRichTextStyleRow* RichTextStyle = (FRichTextStyleRow*)Entry.Value;

					if (SubStyleName == FName(TEXT("Default")))
					{
						DefaultTextStyle = RichTextStyle->TextStyle;
					}

					StyleInstance->Set(SubStyleName, RichTextStyle->TextStyle);
				}
			}
		}

		TSharedRef<FRichTextLayoutMarshaller> Marshaller = FRichTextLayoutMarshaller::Create(
			FDefaultRichTextMarkupParser::Create(),
			FDefaultRichTextMarkupWriter::Create(),
			CreatedDecorators,
			StyleInstance.Get());
		
		SAssignNew(RichTextBlock, SRichTextBlock)
		.MinDesiredWidth(250)
		.WrapTextAt(250)
		.Justification(ETextJustify::Left)
		.TextStyle(&DefaultTextStyle)
		.Marshaller(Marshaller)
		.Text(Value);

		MainBox->AddSlot()
			.AutoHeight()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
			.Padding(FMargin(4, 4))
			[
				SNew(SBorder)
				.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
				.ColorAndOpacity(FLinearColor(1.0f, 0.9f, 0.7f, 1.0f))
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.Padding(FMargin(4, 4))
				[
					RichTextBlock.ToSharedRef()
				]
			];
	}
}

void SGraphDialogueNode::Construct(const FArguments& InArgs, UEdGraphNode* InNode)
{
	if (UK2Node* K2Node = Cast<UK2Node>(InNode))
	{
		SGraphNodeK2Default::Construct(SGraphNodeK2Default::FArguments(), K2Node);
	}
}

FText SGraphDialogueNode::GetTextFromNode() const
{
	UBPNode_DialogueTextEvent* DialogueNode = Cast<UBPNode_DialogueTextEvent>(GraphNode);
	if (DialogueNode)
	{
		return DialogueNode->GetProcessedText();
	}
	return LOCTEXT("NoText", "");
}

void SGraphDialogueNode::CreateInputSideAddButton(TSharedPtr<SVerticalBox> InputBox)
{
	UBPNode_DialogueOutcomesEvent* OutcomesNode = Cast<UBPNode_DialogueOutcomesEvent>(GraphNode);
	if (!OutcomesNode)
	{
		return;
	}

	TSharedRef<SWidget> AddPinButton = AddPinButtonContent(
		LOCTEXT("DialogueNodeAddPinButton", "Add out"),
		LOCTEXT("DialogueNodeAddPinButton_ToolTip", "Add new outcome"));

	FMargin AddPinPadding = Settings->GetOutputPinPadding();
	AddPinPadding.Top += 3.0f;

	InputBox->AddSlot()
		.AutoHeight()
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Left)
		.Padding(AddPinPadding)
		[
			AddPinButton
		];
}

FReply SGraphDialogueNode::OnAddPin()
{
	IK2Node_AddPinInterface* AddPinNode = Cast<IK2Node_AddPinInterface>(GraphNode);
	ensure(AddPinNode);
	if (AddPinNode && AddPinNode->CanAddPin())
	{
		FScopedTransaction Transaction(LOCTEXT("AddPinTransaction", "Add Pin"));

		AddPinNode->AddInputPin();
		UpdateGraphNode();
		GraphNode->GetGraph()->NotifyGraphChanged();
	}

	return FReply::Handled();
}

EVisibility SGraphDialogueNode::IsAddPinButtonVisible() const
{
	UBPNode_DialogueOutcomesEvent* OutcomesNode = Cast<UBPNode_DialogueOutcomesEvent>(GraphNode);
	if (OutcomesNode && OutcomesNode->CanAddPin())
	{
		return EVisibility::Visible;
	}
	return EVisibility::Hidden;
}

#undef LOCTEXT_NAMESPACE

END_SLATE_FUNCTION_BUILD_OPTIMIZATION