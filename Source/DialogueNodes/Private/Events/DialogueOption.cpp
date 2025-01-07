// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "Events/DialogueOption.h"
#include "Dialogue.h"

void UDialogueOption::SetAvailabilityCondition(FName ConditionIn)
{
	AvailabilityCondition = ConditionIn;
}

bool UDialogueOption::EvaluateAvailability()
{
	FireReadEvent();

	if (AvailabilityCondition == NAME_None || AvailabilityCondition.ToString() == "" || !ReadEvent.IsBound())
	{
		return true;
	}

	UProperty* Property = GetReadEventObject()->GetClass()->FindPropertyByName(AvailabilityCondition);

	if (Property)
	{
		if (bool* ptrValue = Property->ContainerPtrToValuePtr<bool>(GetReadEventObject()))
		{
			return *ptrValue;
		}
	}
	return true;
}

UDialogueEvent* UDialogueOption::ExecuteEvent(UDialogue* Dialogue)
{
	UDialogueEvent* NextEvent = UDialogueEvent::ExecuteEvent(Dialogue);

	if (NextEvent)
	{
		return NextEvent;
	}

	TArray<FOptionStruct> OptionStructs;
	for (UDialogueEvent* Event : Options)
	{
		if (UDialogueOption* Option = Cast<UDialogueOption>(Event))
		{
			OptionStructs.Add(Option->GetOptionStruct());
		}
	}

	Dialogue->FireLineEvent(FLineStruct(), OptionStructs, Options);
	return this;
}

FOptionStruct UDialogueOption::GetOptionStruct()
{
	FOptionStruct Struct = FOptionStruct(GetProcessedText(), EvaluateCondition(), EvaluateAvailability(), Sound, DialogueWave);
	Struct.MetadataParameters = GetProcessedMetadata();
	return Struct;
}
