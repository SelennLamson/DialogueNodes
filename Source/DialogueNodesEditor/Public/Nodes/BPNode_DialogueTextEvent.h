// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "Sound/SoundCue.h"
#include "Sound/DialogueWave.h"

#include "Nodes/BPNode_DialogueOutcomesEvent.h"
#include "DialogueData.h"

#include "BPNode_DialogueTextEvent.generated.h"

UCLASS()
class DIALOGUENODESEDITOR_API UBPNode_DialogueTextEvent : public UBPNode_DialogueOutcomesEvent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Dialogue", meta = (MultiLine = true))
	FText Text;

	UPROPERTY(EditAnywhere, Category = "Dialogue")
	TMap<FName, FString> MetadataParameters;

	UPROPERTY(EditAnywhere, Category = "Dialogue Audio")
	USoundBase* Sound;

	UPROPERTY(EditAnywhere, Category = "Dialogue Audio")
	UDialogueWave* DialogueWave;

	FText ProcessedText;
	bool bTextProcessed;

public:
	UBPNode_DialogueTextEvent(const FObjectInitializer& ObjectInitializer);

	virtual TMap<FName, FString> GetDefaultMetadata() const;
	void UpdateMetadataParameters();

	void ProcessTextParams();
	bool RegisterTextParam(FName ParamName);
	void DeleteTextPin(UEdGraphPin* ParamPin);

	virtual FText GetProcessedText();
	bool IsTextParamPin(const UEdGraphPin* Pin) const;

	// Dialogue node expansion
	virtual TArray<UK2Node*> ExpandDialogueNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph, TArray<UK2Node*> CreatedNodes) override;

public:
	// UEdGraphNode interface
	virtual void PostPlacedNewNode() override;
	virtual void ReconstructNode() override;
	// End of UEdGraphNode interface

	// UK2Node interface
	virtual void GetContextMenuActions(const FGraphNodeContextMenuBuilder& Context) const override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;
	virtual void PinDefaultValueChanged(UEdGraphPin* Pin) override;
	// End of UK2Node interface

protected:
	friend class FKismetCompilerContext;
};

UCLASS(DisplayName = "Dialogue Option (Player)", meta = (KeyWords = "Dialogue Option Player Text"))
class UBPNode_DialogueOption : public UBPNode_DialogueTextEvent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Dialogue")
	bool bHasAvailabilityCondition;

	UPROPERTY(EditAnywhere, Category = "Dialogue")
	bool bHasVisibilityCondition;

public:
	UBPNode_DialogueOption(const FObjectInitializer& ObjectInitializer);
	virtual TMap<FName, FString> GetDefaultMetadata() const override;

	// Node aspect and meta-data
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
};

UCLASS(DisplayName = "Dialogue Line (NPC)", meta = (KeyWords = "Dialogue Line NPC Text"))
class UBPNode_DialogueLine : public UBPNode_DialogueTextEvent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Dialogue")
	bool bHasCondition;

public:
	UBPNode_DialogueLine(const FObjectInitializer& ObjectInitializer);

	// Node aspect and meta-data
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
};