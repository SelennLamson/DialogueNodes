// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "BPNode_ExecuteDialogue.generated.h"

UCLASS(DisplayName = "Execute Dialogue", meta = (KeyWords = "Execute Launch Dialogue Start"))
class UBPNode_ExecuteDialogue : public UK2Node
{
	GENERATED_BODY()

public:
	UBPNode_ExecuteDialogue(const FObjectInitializer& ObjectInitializer);

	// Node aspect and meta-data
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetMenuCategory() const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual bool IsCompatibleWithGraph(const UEdGraph* TargetGraph) const override;

	// Pins accessors - Launch dialogue section
	DIALOGUENODESEDITOR_API UEdGraphPin* GetLaunchExec() const;
	DIALOGUENODESEDITOR_API FName GetLaunchExecName() const;
	DIALOGUENODESEDITOR_API UEdGraphPin* GetDialoguePin() const;
	DIALOGUENODESEDITOR_API FName GetDialoguePinName() const;

	// Pins accessors - Select option section
	DIALOGUENODESEDITOR_API UEdGraphPin* GetSelectExec() const;
	DIALOGUENODESEDITOR_API FName GetSelectExecName() const;
	DIALOGUENODESEDITOR_API UEdGraphPin* GetOptionIdPin() const;
	DIALOGUENODESEDITOR_API FName GetOptionIdPinName() const;
	
	// Pins accessors - Stop dialogue section
	DIALOGUENODESEDITOR_API UEdGraphPin* GetStopExec() const;
	DIALOGUENODESEDITOR_API FName GetStopExecName() const;
	
	// Pins accessors - Dialogue line section
	DIALOGUENODESEDITOR_API UEdGraphPin* GetLineExec() const;
	DIALOGUENODESEDITOR_API FName GetLineExecName() const;
	DIALOGUENODESEDITOR_API UEdGraphPin* GetLinePin() const;
	DIALOGUENODESEDITOR_API FName GetLinePinName() const;
	DIALOGUENODESEDITOR_API UEdGraphPin* GetOptionsPin() const;
	DIALOGUENODESEDITOR_API FName GetOptionsPinName() const;

	// Pins accessors - Dialogue end section
	DIALOGUENODESEDITOR_API UEdGraphPin* GetEndExec() const;
	DIALOGUENODESEDITOR_API FName GetEndExecName() const;

	// UEdGraphNode interface
	virtual void AllocateDefaultPins() override;
	// End of UEdGraphNode interface

	// UK2Node interface
	virtual bool IsNodePure() const override { return false; }
	virtual bool CanSplitPin(const UEdGraphPin* Pin) const override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	// End of UK2Node interface
};
