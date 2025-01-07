// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "Nodes/BPNode_DialogueBase.h"
#include "BPNode_DialogueBlueprintEvent.generated.h"

UCLASS(DisplayName = "Dialogue - To Blueprint", meta = (KeyWords = "Dialogue To Blueprint BP Event"))
class DIALOGUENODESEDITOR_API UBPNode_DialogueBlueprintEvent : public UBPNode_DialogueBase
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FName EventName;

	UPROPERTY(EditAnywhere, Category = "Dialogue")
	bool bHasCondition;

public:
	UBPNode_DialogueBlueprintEvent(const FObjectInitializer& ObjectInitializer);

	// Node aspect and meta-data
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual bool ShouldDrawCompact() const { return true; }
	virtual FText GetCompactNodeTitle() const;

	// Pins accessors
	virtual UEdGraphPin* GetThenPin() const;

	// Pin names accessors
	virtual FName GetThenPinName() const;

	// Dialogue node expansion
	virtual TArray<UK2Node*> ExpandDialogueNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph, TArray<UK2Node*> CreatedNodes) override;

public:
	// UEdGraphNode interface
	virtual void AllocateDefaultPins() override;
	// End of UEdGraphNode interface

	// UK2Node interface
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	// End of UK2Node interface
};
