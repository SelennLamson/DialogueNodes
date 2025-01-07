// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Events/DialogueEvent.h"
#include "DialogueBlueprintEvent.generated.h"

DECLARE_DELEGATE(DialogueBlueprintEventDelegate)

UCLASS()
/** General Dialogue event with text, like NPC-Lines */
class DIALOGUENODES_API UDialogueBlueprintEvent : public UDialogueEvent
{
	GENERATED_BODY()

private:
	/** The delegate that will fire an event. */
	DialogueBlueprintEventDelegate BlueprintEvent;

public:
	UDialogueBlueprintEvent() : UDialogueEvent(false, true) {}

	/** Set the event that will be fired when this Dialogue event is executed. Returns true if bound successfully. */
	bool BindBlueprintEvent(UObject* Object, FName EventName);

	/** Execute event in Character's context. This supposes that condition evaluation and checks have been made before. */
	virtual UDialogueEvent* ExecuteEvent(class UDialogue* Dialogue) override;
};
