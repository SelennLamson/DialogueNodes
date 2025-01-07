// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Events/DialogueEvent.h"
#include "DialogueEnd.generated.h"

/**
 *
 */
UCLASS()
class DIALOGUENODES_API UDialogueEnd : public UDialogueEvent
{
	GENERATED_BODY()

public:
	/** Execute event in Character's context. This supposes that condition evaluation and checks have been made before. */
	virtual UDialogueEvent* ExecuteEvent(class UDialogue* Dialogue) override;
};
