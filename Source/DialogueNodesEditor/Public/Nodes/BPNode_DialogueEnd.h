// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "Nodes/BPNode_DialogueBase.h"
#include "BPNode_DialogueEnd.generated.h"

UCLASS(DisplayName = "Dialogue End", meta = (KeyWords = "Dialogue End Stop"))
class DIALOGUENODESEDITOR_API UBPNode_DialogueEnd : public UBPNode_DialogueBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Dialogue")
	bool bHasCondition;

public:
	UBPNode_DialogueEnd(const FObjectInitializer& ObjectInitializer);

	// Node aspect and meta-data
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual bool ShouldDrawCompact() const { return true; }
	virtual FText GetCompactNodeTitle() const;

	// Dialogue node expansion
	virtual TArray<UK2Node*> ExpandDialogueNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph, TArray<UK2Node*> CreatedNodes) override;

public:
	// UK2Node interface
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	// End of UK2Node interface
};
