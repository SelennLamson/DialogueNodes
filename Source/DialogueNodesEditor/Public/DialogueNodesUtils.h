// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EdGraphSchema_K2.h"
#include "EdGraphSchema_K2_Actions.h"
#include "Dialogue.h"
#include "DialogueData.h"
#include "Templates/Casts.h"
#include "DialogueNodesUtils.generated.h"

UENUM()
enum class EDialoguePinType : uint8
{
	PT_Condition,
	PT_Text,
	PT_Name,
	PT_Asset,
	PT_Dialogue,
	PT_DialogueEvent
};

UCLASS()
class DIALOGUENODESEDITOR_API UDialogueNodesUtils : public UObject
{
	GENERATED_BODY()

public:
	static FEdGraphPinType GetPinType(EDialoguePinType DialogPin);

	static FEdGraphPinType DialoguePinType();
	static FEdGraphPinType DialogueEventPinType();
	static FEdGraphPinType ConditionPinType();
	static FEdGraphPinType NamePinType();
	static FEdGraphPinType TextPinType();
	static FEdGraphPinType AssetPinType();
	static FEdGraphPinType LineStructPinType();
	static FEdGraphPinType OptionStructArrayPinType();

	static bool IsDialogueEventPinType(FEdGraphPinType PinType);

	static FLinearColor DialogueNodesTitleColor();
	static FLinearColor DialogueOptionTitleColor();
	static FLinearColor DialoguePinsColor();

	template <typename NodeType>
	static NodeType* AutoCreateDialogueNode(UEdGraphPin* OutcomePin)
	{
		check(OutcomePin->Direction == EGPD_Output);
		check(NodeType::StaticClass() == UBPNode_DialogueOption::StaticClass() || NodeType::StaticClass() == UBPNode_DialogueLine::StaticClass())

		UK2Node* Node = CastChecked<UK2Node>(OutcomePin->GetOwningNode());

		OutcomePin->BreakAllPinLinks();
		int32 PinIndex;
		for (PinIndex = 0; PinIndex < Node->Pins.Num(); PinIndex++)
		{
			if (OutcomePin == Node->GetPinAt(PinIndex))
			{
				break;
			}
		}

		if (UBPNode_DialogueOutcomesEvent* OutcomesEventNode = dynamic_cast<UBPNode_DialogueOutcomesEvent*>(Node))
		{
			PinIndex -= OutcomesEventNode->GetOutcomeBeginIndex();
		}

		FVector2D NodeLocation = FVector2D(Node->NodePosX + 384, Node->NodePosY + PinIndex * 144 - 176);
		NodeType* NewNode = FEdGraphSchemaAction_K2NewNode::SpawnNode<NodeType>(Node->GetGraph(), NodeLocation, EK2NewNodeFlags::SelectNewNode);
		
		OutcomePin->MakeLinkTo(NewNode->GetInPin());

		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Node->GetBlueprint());

		return NewNode;
	}
};
