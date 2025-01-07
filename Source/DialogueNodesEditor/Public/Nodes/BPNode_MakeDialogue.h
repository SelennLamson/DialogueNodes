// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "Nodes/BPNode_DialogueOutcomesEvent.h"
#include "BPNode_MakeDialogue.generated.h"

UCLASS(DisplayName = "Make Dialogue", meta = (KeyWords = "Make Dialogue Construct Create Begin"))
class DIALOGUENODESEDITOR_API UBPNode_MakeDialogue : public UBPNode_DialogueOutcomesEvent
{
	GENERATED_BODY()

public:
	UBPNode_MakeDialogue(const FObjectInitializer& ObjectInitializer);

	// Node aspect and meta-data
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	// Pins accessors
	virtual UEdGraphPin* GetRefPin() const;
	virtual UEdGraphPin* GetThenPin() const;

	// Pin names accessors
	virtual FName GetRefPinName() const;

	// Dialogue node expansion
	virtual TArray<UK2Node*> ExpandDialogueNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph, TArray<UK2Node*> CreatedNodes) override;

public:
	// UEdGraphNode interface
	virtual void AllocateDefaultPins() override;
	virtual void ReconstructNode() override;
	// End of UEdGraphNode interface

	// UK2Node interface
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	// End of UK2Node interface
};
