// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "K2Node_AddPinInterface.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Nodes/BPNode_DialogueBase.h"
#include "BPNode_DialogueOutcomesEvent.generated.h"

UCLASS()
class DIALOGUENODESEDITOR_API UBPNode_DialogueOutcomesEvent : public UBPNode_DialogueBase, public IK2Node_AddPinInterface
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int32 NumOutcomes;

public:
	UBPNode_DialogueOutcomesEvent(const FObjectInitializer& ObjectInitializer);

	// Actions linked to a specific pin (when right-clicked)
	void RemoveOutcomePin(UEdGraphPin* Pin);
	void AutoCreateOptionNode(UEdGraphPin* OutcomePin);
	void AutoCreateLineNode(UEdGraphPin* OutcomePin);
	void AutoAddOptionPin();
	void AutoAddLinePin();

	// Pins
	virtual UEdGraphPin* GetOutcomePin(const int32 OutcomeIndex) const;
	int32 GetOutcomeBeginIndex() const;
	FName GetOutcomePinName(const int32 PinIndex) const;
	void SyncOutcomePinNames();

	// Dialogue node expansion
	virtual TArray<UK2Node*> ExpandDialogueNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph, TArray<UK2Node*> CreatedNodes) override;

public:
	// UEdGraphNode interface
	virtual void AllocateDefaultPins() override;
	// End of UEdGraphNode interface

	// UK2Node interface
	virtual void GetContextMenuActions(const FGraphNodeContextMenuBuilder& Context) const override;
	// End of UK2Node interface

	// IK2Node_AddPinInterface interface
	void AddInputPin();
	// End of IK2Node_AddPinInterface interface

protected:
	friend class FKismetCompilerContext;

	/** Helper function for context menu add pin to ensure transaction is set up correctly. */
	void InteractiveAddInputPin();
};
