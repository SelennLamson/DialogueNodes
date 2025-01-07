// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "Events/DialogueEvent.h"
#include "Events/DialogueGoto.h"
#include "Events/DialogueOption.h"
#include "Dialogue.h"

FString Tabs(int32 Tabs)
{
	FString String = "";
	for (int32 i = 0; i < Tabs; i++)
	{
		String += "        ";
	}
	return String;
}

bool UDialogueEvent::BindReadEvent(UObject* Object, FName EventName)
{
	if (EventName != NAME_None)
	{
		ReadEvent.BindUFunction(Object, EventName);
	}
	return ReadEvent.IsBound();
}

void UDialogueEvent::FireReadEvent()
{
	ReadEvent.ExecuteIfBound();
}

UObject* UDialogueEvent::GetReadEventObject()
{
	return ReadEvent.GetUObject();
}

void UDialogueEvent::SetEventSolo(bool bIsSolo)
{
	bIsEventSolo = bIsSolo;
}

bool UDialogueEvent::IsEventSolo()
{
	return bIsEventSolo;
}

void UDialogueEvent::SetEventExtra(bool bIsExtra)
{
	bIsEventExtra = bIsExtra;
}

bool UDialogueEvent::IsEventExtra()
{
	return bIsEventExtra;
}

void UDialogueEvent::SetCondition(FName ConditionIn)
{
	Condition = ConditionIn;
}

bool UDialogueEvent::EvaluateCondition()
{
	FireReadEvent();
	
	if (Condition == NAME_None || Condition.ToString() == "" || !ReadEvent.IsBound())
	{
		return true;
	}
	
	UProperty* Property = GetReadEventObject()->GetClass()->FindPropertyByName(Condition);
	
	if (Property)
	{
		if (bool* ptrValue = Property->ContainerPtrToValuePtr<bool>(GetReadEventObject()))
		{
			return *ptrValue;
		}
	}

	return true;
}

void UDialogueEvent::AddOutcome(UDialogueEvent* Event)
{
	Outcomes.Add(Event);
}

void UDialogueEvent::RemoveGoto(int32 Reference)
{
	int32 Index = FindGotoIndex(Reference);
	if (Index != INDEX_NONE)
	{
		Outcomes.RemoveAt(Index);
	}
}

void UDialogueEvent::ReplaceGoto(int32 Reference, UDialogueEvent* NewEvent)
{
	int32 Index = FindGotoIndex(Reference);
	if (Index != INDEX_NONE)
	{
		Outcomes[Index] = NewEvent;
	}
}

int32 UDialogueEvent::FindGotoIndex(int32 Reference)
{
	int32 Index = 0;
	for (UDialogueEvent* Event : Outcomes)
	{
		UDialogueGoto* Goto = dynamic_cast<UDialogueGoto*>(Event);
		if (Goto != nullptr)
		{
			if (Goto->GetReference() == Reference)
			{
				return Index;
			}
		}
		Index++;
	}
	return INDEX_NONE;
}

TArray<UDialogueEvent*> UDialogueEvent::GetOutcomes()
{
	return Outcomes;
}

UDialogueEvent* UDialogueEvent::ExecuteEvent(UDialogue* Dialogue)
{
	TArray<UDialogueEvent*> ExecutionList;
	UDialogueEvent* NextSoloEvent = nullptr;

	for (UDialogueEvent* Outcome : Outcomes)
	{
		if (Outcome->EvaluateCondition())
		{
			if (Outcome->bIsEventExtra)
			{
				Outcome->ExecuteEvent(Dialogue);
			}
			else if (Outcome->bIsEventSolo)
			{
				ExecutionList.Empty();
				NextSoloEvent = Outcome;
				break;
			}
			else
			{
				ExecutionList.Add(Outcome);
			}
		}
	}

	Options.Empty();

	for (UDialogueEvent* ListItem : ExecutionList)
	{
		if (UDialogueOption* OptionEvent = Cast<UDialogueOption>(ListItem))
		{
			Options.Add(OptionEvent);
		}
		else
		{
			ListItem->ExecuteEvent(Dialogue);
		}
	}

	return NextSoloEvent;
}
