// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "Nodes/BPNode_DialogueBase.h"
#include "DialogueBlueprintLibrary.h"

#include "Engine/BlueprintGeneratedClass.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "KismetCompiler.h"
#include "UObject/UnrealType.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"

#include "K2Node_CustomEvent.h"
#include "K2Node_MakeMap.h"
#include "K2Node_Self.h"

#define LOCTEXT_NAMESPACE "DialogueBaseNode"

UBPNode_DialogueBase::UBPNode_DialogueBase(const FObjectInitializer& ObjectInitializer) : UK2Node(ObjectInitializer) { }

FText UBPNode_DialogueBase::GetMenuCategory() const
{
	return FText::FromString("Dialogue");
}

FLinearColor UBPNode_DialogueBase::GetNodeTitleColor() const
{
	return UDialogueNodesUtils::DialogueNodesTitleColor();
}

bool UBPNode_DialogueBase::IsCompatibleWithGraph(const UEdGraph* TargetGraph) const
{
	bool bIsCompatible = Super::IsCompatibleWithGraph(TargetGraph);
	if (bIsCompatible)
	{
		EGraphType const GraphType = TargetGraph->GetSchema()->GetGraphType(TargetGraph);
		bIsCompatible = (GraphType == EGraphType::GT_Ubergraph);
	}

	return bIsCompatible;
}

UEdGraphPin* UBPNode_DialogueBase::GetInPin() const
{
	return GetPinAt(0);
}

void UBPNode_DialogueBase::ProcessConditionPins()
{
	UBoolProperty* HasConditionProp = FindField<UBoolProperty>(GetClass(), "bHasCondition");
	if (HasConditionProp)
	{
		SetConditionParamValue("Condition", *HasConditionProp->ContainerPtrToValuePtr<bool>(this));
	}

	UBoolProperty* HasVisibilityConditionProp = FindField<UBoolProperty>(GetClass(), "bHasVisibilityCondition");
	if (HasVisibilityConditionProp)
	{
		SetConditionParamValue("Condition", *HasVisibilityConditionProp->ContainerPtrToValuePtr<bool>(this));
	}

	UBoolProperty* HasAvailabilityConditionProp = FindField<UBoolProperty>(GetClass(), "bHasAvailabilityCondition");
	if (HasAvailabilityConditionProp)
	{
		SetConditionParamValue("AvailabilityCondition", *HasAvailabilityConditionProp->ContainerPtrToValuePtr<bool>(this));
	}

	ReconstructNode();
}

void UBPNode_DialogueBase::SetConditionParamValue(FName ParamName, bool Value)
{
	for (int32 ParamIndex = 0; ParamIndex < BaseParameters.Num(); ParamIndex++)
	{
		FDialogueParameter Param = BaseParameters[ParamIndex];
		if (Param.Name == ParamName)
		{
			BaseParameters[ParamIndex] = FDialogueParameter(Param.Name, Value, Param.Type, Param.Pin);
			return;
		}
	}
}

FName UBPNode_DialogueBase::GetInPinName() const
{
	return FName("");
}

void UBPNode_DialogueBase::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, UDialogueNodesUtils::DialogueEventPinType(), GetInPinName());

	AllocateParamPins(BaseParameters);
	AllocateParamPins(TextParameters);
}

void UBPNode_DialogueBase::AllocateParamPins(TArray<FDialogueParameter> &Parameters)
{
	for (int32 ParamIndex = 0; ParamIndex < Parameters.Num(); ParamIndex++)
	{
		FDialogueParameter Param = Parameters[ParamIndex];
		if (Param.bHasPin)
		{
			Parameters[ParamIndex] = Param.GetWithPin(CreatePin(EGPD_Input, Param.GetPinType(), Param.Name));
		}
	}
}

int32 UBPNode_DialogueBase::ReconstructParamPins(TArray<FDialogueParameter> &Parameters, int32 BeginIndex)
{
	TFunction<void(UEdGraphPin*)> RemovePin = [this, &RemovePin](UEdGraphPin* PinToRemove)
	{
		int32 PinRemovalIndex = INDEX_NONE;
		if (Pins.Find(PinToRemove, PinRemovalIndex))
		{
			Pins.RemoveAt(PinRemovalIndex);
			PinToRemove->MarkPendingKill();
		}
	};

	int32 PinIndex = BeginIndex;

	for (int32 ParamIndex = 0; ParamIndex < Parameters.Num(); ParamIndex++)
	{
		FDialogueParameter Param = Parameters[ParamIndex];

		if (Param.bHasPin && Param.Pin)
		{
			PinIndex++;
		}
		else if (Param.bHasPin && !Param.Pin)
		{
			if (PinIndex < Pins.Num())
			{
				UEdGraphPin* Pin = Pins[PinIndex];
				if (Pin->Direction == EGPD_Input && Pin->PinName == Param.Name && Pin->ParentPin == nullptr)
				{
					Parameters[ParamIndex] = Param.GetWithPin(Pin);
				}
				else
				{
					Parameters[ParamIndex] = Param.GetWithPin(CreatePin(EGPD_Input, Param.GetPinType(), Param.Name, PinIndex));
				}
			}
			else
			{
				Parameters[ParamIndex] = Param.GetWithPin(CreatePin(EGPD_Input, Param.GetPinType(), Param.Name, PinIndex));
			}

			PinIndex++;
		}
		else if (!Param.bHasPin && Param.Pin)
		{
			RemovePin(Param.Pin);
			Parameters[ParamIndex] = Param.GetWithoutPin();
		}
	}

	return PinIndex;
}

void UBPNode_DialogueBase::ReconstructNode()
{
	for (int32 PinIndex = Pins.Num() - 1; PinIndex > 0; PinIndex--)
	{
		UEdGraphPin* Pin = Pins[PinIndex];

		if (Pin->Direction == EGPD_Output || Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct || Pin->ParentPin != nullptr)
		{
			continue;
		}

		bool bShouldDeletePin = true;
		for (FDialogueParameter Param : BaseParameters)
		{
			if (Param.Name == Pin->PinName)
			{
				bShouldDeletePin = false;
				break;
			}
		}

		for (FDialogueParameter Param : TextParameters)
		{
			if (Param.Name == Pin->PinName || !bShouldDeletePin)
			{
				bShouldDeletePin = false;
				break;
			}
		}

		if (bShouldDeletePin)
		{
			Pins.RemoveAt(PinIndex);
			Pin->MarkPendingKill();
		}
	}
	
	int32 PinIndex = 1;
	PinIndex = ReconstructParamPins(BaseParameters, PinIndex);
	ReconstructParamPins(TextParameters, PinIndex);
	
	UK2Node::ReconstructNode();
}

void UBPNode_DialogueBase::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (UBoolProperty* BoolProperty = Cast<UBoolProperty>(PropertyChangedEvent.Property))
	{
		ProcessConditionPins();
	}
}

TArray<UK2Node*> UBPNode_DialogueBase::ExpandDialogueNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph, TArray<UK2Node*> CreatedNodes)
{
	// Ensure that all node pins are consistent with param array.
	ReconstructNode();

	UEdGraphPin* CurrentExecPin = nullptr;
	FName ReadEventName = "";

	///////////////////////////////////////
	//	Function call node

	FunctionNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this);
	FunctionNode->FunctionReference.SetExternalMember(LibraryFunctionName, UDialogueBlueprintLibrary::StaticClass());
	FunctionNode->AllocateDefaultPins();
	FunctionNode->NodePosX = NodePosX;
	FunctionNode->NodePosY = NodePosY;
	CreatedNodes.Add(FunctionNode);

	///////////////////////////////////////
	//	Self node

	UK2Node_Self* SelfNode = CompilerContext.SpawnIntermediateNode<UK2Node_Self>(this, SourceGraph);
	SelfNode->AllocateDefaultPins();
	SelfNode->NodePosX = NodePosX;
	SelfNode->NodePosY = NodePosY;
	CreatedNodes.Add(SelfNode);

	UEdGraphPin* SelfPin = SelfNode->GetPinAt(0);

	///////////////////////////////////////
	//	Read event node (if necessary)

	bool bShouldCreateReadEvent = TextParameters.Num() != 0;

	for (FDialogueParameter Param : BaseParameters)
	{
		if (Param.bHasPin)
		{
			bShouldCreateReadEvent = true;
			break;
		}
	}

	if (bShouldCreateReadEvent)
	{
		UK2Node_CustomEvent* ReadEventNode = CompilerContext.SpawnIntermediateEventNode<UK2Node_CustomEvent>(this);
		ReadEventNode->CustomFunctionName = "DialogueReadEvent";
		ReadEventNode->RenameCustomEventCloseToName();
		ReadEventNode->AllocateDefaultPins();
		ReadEventNode->NodePosX = NodePosX;
		ReadEventNode->NodePosY = NodePosY;
		CreatedNodes.Add(ReadEventNode);

		CurrentExecPin = ReadEventNode->GetPinAt(1);
		ReadEventName = ReadEventNode->GetFunctionName();
	}

	///////////////////////////////////////
	//	Text parameters map node

	UK2Node_MakeMap* TextParamsMapNode = nullptr;
	UEdGraphPin* TextParamsPin = FunctionNode->FindPin(FName("TextParameters"));

	if (TextParamsPin)
	{
		TextParamsMapNode = CompilerContext.SpawnIntermediateNode<UK2Node_MakeMap>(this);
		TextParamsMapNode->NumInputs = TextParameters.Num();
		TextParamsMapNode->AllocateDefaultPins();
		TextParamsMapNode->GetOutputPin()->MakeLinkTo(TextParamsPin);
		TextParamsMapNode->PostReconstructNode();
		TextParamsMapNode->NodePosX = NodePosX;
		TextParamsMapNode->NodePosY = NodePosY;
		CreatedNodes.Add(TextParamsMapNode);
	}
	
	///////////////////////////////////////
	//	Linking inputs

	if (UEdGraphPin* ReturnValuePin = FunctionNode->FindPin(UEdGraphSchema_K2::PN_ReturnValue))
	{
		for (UEdGraphPin* OtherPin : GetInPin()->LinkedTo)
		{
			OtherPin->MakeLinkTo(ReturnValuePin);
		}
	}

	if (UEdGraphPin* ReadEventPin = FunctionNode->FindPin(FName("ReadEventName")))
	{
		ReadEventPin->DefaultValue = ReadEventName.ToString();
	}

	if (UEdGraphPin* ObjectPin = FunctionNode->FindPin(FName("Object")))
	{
		ObjectPin->MakeLinkTo(SelfPin);
	}
	
	///////////////////////////////////////
	//	Handling base parameters

	for (FDialogueParameter Param : BaseParameters)
	{
		UEdGraphPin* ArgumentPin = FunctionNode->FindPin(Param.Name);
		if (ArgumentPin && Param.bHasPin && Param.Pin)
		{
			///////////////////////////////////////
			//	New property

			FName VarName = FName(*(Param.Name.ToString() + FString::FromInt(GetUniqueID())));
			UProperty* NewProperty = FKismetCompilerUtilities::CreatePropertyOnScope(CompilerContext.NewClass, VarName, Param.Pin->PinType, CompilerContext.NewClass, CPF_None, CompilerContext.GetSchema(), CompilerContext.MessageLog);
			if (NewProperty)
			{
				NewProperty->SetFlags(RF_LoadCompleted);
				FKismetCompilerUtilities::LinkAddedProperty(CompilerContext.NewClass, NewProperty);

				///////////////////////////////////////
				//	Set variable node

				FName SetBool_FunctionName = GET_FUNCTION_NAME_CHECKED(UDialogueBlueprintLibrary, SetBoolVariableByName);

				UK2Node_CallFunction* SetVarNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this);
				SetVarNode->FunctionReference.SetExternalMember(SetBool_FunctionName, UDialogueBlueprintLibrary::StaticClass());
				SetVarNode->AllocateDefaultPins();
				SetVarNode->NodePosX = NodePosX;
				SetVarNode->NodePosY = NodePosY;
				CreatedNodes.Add(SetVarNode);

				UEdGraphPin* SetVarExecPin = SetVarNode->GetExecPin();
				UEdGraphPin* SetVarThenPin = SetVarNode->GetThenPin();
				UEdGraphPin* SetVarObjectPin = SetVarNode->FindPinChecked(FName("Object"));
				UEdGraphPin* SetVarNamePin = SetVarNode->FindPinChecked(FName("VarName"));
				UEdGraphPin* SetVarValuePin = SetVarNode->FindPinChecked(FName("Value"));

				///////////////////////////////////////
				//	Establish links

				SelfPin->MakeLinkTo(SetVarObjectPin);
				SetVarNamePin->DefaultValue = VarName.ToString();
				CompilerContext.MovePinLinksToIntermediate(*Param.Pin, *SetVarValuePin);
				CurrentExecPin->MakeLinkTo(SetVarExecPin);
				CurrentExecPin = SetVarThenPin;

				ArgumentPin->DefaultValue = VarName.ToString();
			}
		}
		else if (ArgumentPin)
		{
			UProperty* Property = GetClass()->FindPropertyByName(Param.Name);

			if (Property)
			{
				void* ptrValue = Property->ContainerPtrToValuePtr<void>(this);

				if (ptrValue)
				{
					FName Type = ArgumentPin->PinType.PinCategory;
					if (Type == UEdGraphSchema_K2::PC_Boolean)
					{
						bool* Value = static_cast<bool*>(ptrValue);
						if (Value)
						{
							ArgumentPin->DefaultValue = *Value ? "1" : "0";
						}
					}
					else if (Type == UEdGraphSchema_K2::PC_Float)
					{
						float* Value = static_cast<float*>(ptrValue);
						if (Value)
						{
							ArgumentPin->DefaultValue = FString::SanitizeFloat(*Value);
						}
					}
					else if (Type == UEdGraphSchema_K2::PC_Int)
					{
						int32* Value = static_cast<int32*>(ptrValue);
						if (Value)
						{
							ArgumentPin->DefaultValue = FString::FromInt(*Value);
						}
					}
					else if (Type == UEdGraphSchema_K2::PC_Name)
					{
						FName* Value = static_cast<FName*>(ptrValue);
						if (Value)
						{
							ArgumentPin->DefaultValue = Value->ToString();
						}
					}
					else if (Type == UEdGraphSchema_K2::PC_String)
					{
						FString* Value = static_cast<FString*>(ptrValue);
						if (Value)
						{
							ArgumentPin->DefaultValue = *Value;
						}
					}
					else if (Type == UEdGraphSchema_K2::PC_Text)
					{
						FText* Value = static_cast<FText*>(ptrValue);
						if (Value)
						{
							ArgumentPin->DefaultTextValue = *Value;
						}
					}
				}
			}
		}
	}

	///////////////////////////////////////
	//	Handling text parameters

	if (TextParamsMapNode)
	{
		for (int32 ParamIndex = 0; ParamIndex < TextParameters.Num(); ParamIndex++)
		{
			FDialogueParameter Param = TextParameters[ParamIndex];
			if (Param.Pin)
			{
				///////////////////////////////////////
				//	New property

				FName VarName = FName(*(Param.Name.ToString() + FString::FromInt(GetUniqueID())));
				UProperty* NewProperty = FKismetCompilerUtilities::CreatePropertyOnScope(CompilerContext.NewClass, VarName, Param.Pin->PinType, CompilerContext.NewClass, CPF_None, CompilerContext.GetSchema(), CompilerContext.MessageLog);
				if (NewProperty)
				{
					NewProperty->SetFlags(RF_LoadCompleted);
					FKismetCompilerUtilities::LinkAddedProperty(CompilerContext.NewClass, NewProperty);

					///////////////////////////////////////
					//	Set variable node

					FName SetText_FunctionName = GET_FUNCTION_NAME_CHECKED(UDialogueBlueprintLibrary, SetTextVariableByName);

					UK2Node_CallFunction* SetVarNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this);
					SetVarNode->FunctionReference.SetExternalMember(SetText_FunctionName, UDialogueBlueprintLibrary::StaticClass());
					SetVarNode->AllocateDefaultPins();
					SetVarNode->NodePosX = NodePosX;
					SetVarNode->NodePosY = NodePosY;
					CreatedNodes.Add(SetVarNode);

					UEdGraphPin* SetVarExecPin = SetVarNode->GetExecPin();
					UEdGraphPin* SetVarThenPin = SetVarNode->GetThenPin();
					UEdGraphPin* SetVarObjectPin = SetVarNode->FindPinChecked(FName("Object"));
					UEdGraphPin* SetVarNamePin = SetVarNode->FindPinChecked(FName("VarName"));
					UEdGraphPin* SetVarValuePin = SetVarNode->FindPinChecked(FName("Value"));

					///////////////////////////////////////
					//	Establish links

					SelfPin->MakeLinkTo(SetVarObjectPin);
					SetVarNamePin->DefaultValue = VarName.ToString();
					CompilerContext.MovePinLinksToIntermediate(*Param.Pin, *SetVarValuePin);
					CurrentExecPin->MakeLinkTo(SetVarExecPin);
					CurrentExecPin = SetVarThenPin;

					TextParamsMapNode->GetPinAt(ParamIndex * 2 + 1)->DefaultValue = Param.Name.ToString();
					TextParamsMapNode->GetPinAt(ParamIndex * 2 + 2)->DefaultValue = VarName.ToString();
				}
			}
		}
	}

	return CreatedNodes;
}

#undef LOCTEXT_NAMESPACE
