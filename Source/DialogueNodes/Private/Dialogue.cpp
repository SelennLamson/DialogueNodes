// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "Dialogue.h"
#include "Events/DialogueBlueprintEvent.h"
#include "Events/DialogueEnd.h"
#include "Events/DialogueGoto.h"
#include "Events/DialogueOption.h"
#include "Events/DialogueTextEvent.h"

void UDialogue::SetStartingEvent(UDialogueEvent* Event)
{
	StartingEvent = Event;
}

UDialogueEvent* UDialogue::GetStartingEvent()
{
	return StartingEvent;
}

void UDialogue::Launch(UObject* Object, FName LineEventName, FName EndEventName)
{
	if (bIsCurrentlyExecuted)
	{
		return;
	}
	bIsCurrentlyExecuted = true;

	if (LineEventName != NAME_None)
	{
		LineDelegate.BindUFunction(Object, LineEventName);
	}
	
	if (EndEventName != NAME_None)
	{
		EndDelegate.BindUFunction(Object, EndEventName);
	}

	NextEvent = StartingEvent;
	ExecuteEvent();
}

void UDialogue::SelectOption(int32 OptionId)
{
	if (!bIsCurrentlyExecuted)
	{
		return;
	}

	if (bIsWaitingForInput)
	{
		if (CurrentOptions.Num() == 0)
		{
			Stop();
			return;
		}
		else if (OptionId < 0 || OptionId >= CurrentOptions.Num())
		{
			NextEvent = CurrentOptions[0];
		}
		else
		{
			NextEvent = CurrentOptions[OptionId];
		}
	}

	ExecuteEvent();
}

void UDialogue::Stop()
{
	if (!bIsCurrentlyExecuted)
	{
		return;
	}

	NextEvent = nullptr;
	ExecuteEvent();
}

void UDialogue::FireLineEvent(FLineStruct Line, TArray<FOptionStruct> Options, TArray<UDialogueEvent*> OptionEvents)
{
	LineStruct = Line;
	LineOptions = Options;
	CurrentOptions = OptionEvents;
	bIsWaitingForInput = true;

	LineDelegate.ExecuteIfBound();
}

FLineStruct UDialogue::GetLineStruct()
{
	return LineStruct;
}

TArray<FOptionStruct> UDialogue::GetLineOptions()
{
	return LineOptions;
}

void UDialogue::ExecuteEvent()
{
	if (NextEvent)
	{
		bIsWaitingForInput = false;
		NextEvent = NextEvent->ExecuteEvent(this);

		if (!bIsWaitingForInput)
		{
			ExecuteEvent();
		}
	}
	else
	{
		bIsCurrentlyExecuted = false;
		EndDelegate.ExecuteIfBound();
	}
}

void UDialogueBuilder::SetStartingEvent(UDialogueEvent* Event)
{
	StartingEvent = Event;
}

UDialogue* UDialogueBuilder::SetupDialogue()
{
	RecursiveLookForGoto(StartingEvent);
	ResolveGotoLinks();

	UDialogue* Dialogue = NewObject<UDialogue>();
	Dialogue->SetStartingEvent(StartingEvent);

	return Dialogue;
}

void UDialogueBuilder::RecursiveLookForGoto(UDialogueEvent* Event)
{
	SetupEvents.Add(Event);

	for (UDialogueEvent* Outcome : Event->GetOutcomes())
	{
		// We don't want the recursive search to go forever if loops are created.
		if (SetupEvents.Contains(Outcome))
		{
			continue;
		}

		// Check if the outcome is of type Goto.
		UDialogueGoto* Goto = dynamic_cast<UDialogueGoto*>(Outcome);
		if (Goto != nullptr)
		{
			if (Goto->IsDestinationNode())
			{
				// The event is a destination : should be treated as an incoming link.
				Event->RemoveGoto(Goto->GetReference());

				if (IncomeLinks.Contains(Goto->GetReference()))
				{
					continue;
				}
				IncomeLinks.Add(Goto->GetReference(), Event);
			}
			else
			{
				// The event is an origin : should be treated as an outcome link.
				
				if (!OutcomeLinks.Contains(Goto->GetReference()))
				{
					OutcomeLinks.Add(Goto->GetReference(), TArray<UDialogueEvent*>());
				}
				OutcomeLinks[Goto->GetReference()].Add(Event);
			}
		}
		else
		{
			RecursiveLookForGoto(Outcome);
		}
	}
}

void UDialogueBuilder::ResolveGotoLinks()
{
	for (TPair<int32, TArray<UDialogueEvent*>> OutLink : OutcomeLinks)
	{
		if (IncomeLinks.Find(OutLink.Key))
		{
			UDialogueEvent* FinalOutcome = IncomeLinks[OutLink.Key];
			for (UDialogueEvent* Source : OutLink.Value)
			{
				Source->ReplaceGoto(OutLink.Key, FinalOutcome);
			}
		}
		else
		{
			for (UDialogueEvent* Source : OutLink.Value)
			{
				Source->RemoveGoto(OutLink.Key);
			}
		}
	}
	OutcomeLinks.Empty();
	IncomeLinks.Empty();
}
