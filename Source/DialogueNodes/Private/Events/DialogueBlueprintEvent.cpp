// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "Events/DialogueBlueprintEvent.h"
#include "GameFramework/Actor.h"

bool UDialogueBlueprintEvent::BindBlueprintEvent(UObject* Object, FName EventName)
{
	if (EventName != NAME_None)
	{
		BlueprintEvent.BindUFunction(Object, EventName);
	}
	return BlueprintEvent.IsBound();
}

UDialogueEvent* UDialogueBlueprintEvent::ExecuteEvent(UDialogue* Dialogue)
{
	BlueprintEvent.ExecuteIfBound();
	return nullptr;
}
