// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "EdGraph/EdGraphPin.h"
#include "K2Node_CallFunction.h"
#include "DialogueNodesUtils.h"
#include "Nodes/DialogueParameter.h"
#include "Containers/Map.h"

#include "BPNode_DialogueBase.generated.h"

UCLASS()
class DIALOGUENODESEDITOR_API UBPNode_DialogueBase : public UK2Node
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	FName LibraryFunctionName;
	
	UPROPERTY()
	UK2Node_CallFunction* FunctionNode;

	UPROPERTY()
	TArray<FDialogueParameter> BaseParameters;
	UPROPERTY()
	TArray<FDialogueParameter> TextParameters;

public:
	UBPNode_DialogueBase(const FObjectInitializer& ObjectInitializer);

	// Node aspect and meta-data
	virtual FText GetMenuCategory() const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual bool IsCompatibleWithGraph(const UEdGraph* TargetGraph) const override;

	// Pins accessors
	 UEdGraphPin* GetInPin() const;

	// Params
	void ProcessConditionPins();
	void SetConditionParamValue(FName ParamName, bool Value);
	void AllocateParamPins(TArray<FDialogueParameter> &Parameters);
	int32 ReconstructParamPins(TArray<FDialogueParameter> &Parameters, int32 BeginIndex);

	// Pin names accessors
	FName GetInPinName() const;

	// Dialogue node expansion
	virtual TArray<UK2Node*> ExpandDialogueNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph, TArray<UK2Node*> CreatedNodes);

	// UEdGraphNode interface
	virtual void AllocateDefaultPins() override;
	virtual void ReconstructNode() override;
	// End of UEdGraphNode interface

	// UK2Node interface
	virtual bool IsNodePure() const override { return false; }
	virtual bool ShouldShowNodeProperties() const override { return true; }
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	// End of UK2Node interface
};
