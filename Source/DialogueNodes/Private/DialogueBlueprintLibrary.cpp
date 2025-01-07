// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "DialogueBlueprintLibrary.h"

//////////////////////////////////
////	Dialogue

void UDialogueBlueprintLibrary::MakeDialogue(UDialogue* &Dialogue, UDialogueEvent* StartingEvent)
{
	UDialogueBuilder* Builder = NewObject<UDialogueBuilder>();
	Builder->SetStartingEvent(StartingEvent);
	
	Dialogue = Builder->SetupDialogue();
}

void UDialogueBlueprintLibrary::LaunchDialogue(UDialogue* Dialogue, UObject* Object, FName LineEventName, FName EndEventName)
{
	if (Dialogue)
	{
		Dialogue->Launch(Object, LineEventName, EndEventName);
	}
}

void UDialogueBlueprintLibrary::SelectOption(UDialogue* Dialogue, int32 OptionId)
{
	if (Dialogue)
	{
		Dialogue->SelectOption(OptionId);
	}
}

void UDialogueBlueprintLibrary::StopDialogue(UDialogue* Dialogue)
{
	if (Dialogue)
	{
		Dialogue->Stop();
	}
}

FLineStruct UDialogueBlueprintLibrary::GetLineStruct(UDialogue* Dialogue)
{
	if (Dialogue)
	{
		return Dialogue->GetLineStruct();
	}
	return FLineStruct();
}

TArray<FOptionStruct> UDialogueBlueprintLibrary::GetLineOptions(UDialogue* Dialogue)
{
	if (Dialogue)
	{
		return Dialogue->GetLineOptions();
	}
	return TArray<FOptionStruct>();
}


//////////////////////////////////
////	EVENTS

UDialogueEnd* UDialogueBlueprintLibrary::MakeDialogueEnd(FName ReadEventName, UObject* Object, FName Condition)
{
	UDialogueEnd* Event = NewObject<UDialogueEnd>();
	Event->BindReadEvent(Object, ReadEventName);
	Event->SetCondition(Condition);

	return Event;
}

UDialogueGoto* UDialogueBlueprintLibrary::MakeDialogueGoto(int32 Reference, bool bIsDestination)
{
	UDialogueGoto* Event = NewObject<UDialogueGoto>();
	Event->SetReference(Reference);
	Event->SetDestinationNode(bIsDestination);

	return Event;
}

UDialogueOption* UDialogueBlueprintLibrary::MakeDialogueOption(FName ReadEventName, UObject* Object, FName Condition, FName AvailabilityCondition, FText Text,
														 TArray<UDialogueEvent*> Outcomes, TMap<FName, FName> TextParameters, TMap<FName, FString> MetadataParameters,
														 USoundBase* Sound, UDialogueWave* DialogueWave)
{
	UDialogueOption* Event = NewObject<UDialogueOption>();
	Event->BindReadEvent(Object, ReadEventName);
	Event->SetCondition(Condition);
	Event->SetAvailabilityCondition(AvailabilityCondition);
	Event->SetText(Text);
	Event->SetTextParameters(TextParameters);
	Event->SetMetadataParameters(MetadataParameters);
	Event->SetSound(Sound);
	Event->SetDialogueWave(DialogueWave);
	
	for (UDialogueEvent* Outcome : Outcomes)
	{
		Event->AddOutcome(Outcome);
	}

	return Event;
}

UDialogueTextEvent* UDialogueBlueprintLibrary::MakeDialogueLine(FName ReadEventName, UObject* Object, FName Condition, FText Text,
														TArray<UDialogueEvent*> Outcomes, TMap<FName, FName> TextParameters, TMap<FName, FString> MetadataParameters,
														USoundBase* Sound, UDialogueWave* DialogueWave)
{
	UDialogueTextEvent* Event = NewObject<UDialogueTextEvent>();
	Event->BindReadEvent(Object, ReadEventName);
	Event->SetCondition(Condition);
	Event->SetText(Text);
	Event->SetTextParameters(TextParameters);
	Event->SetMetadataParameters(MetadataParameters);
	Event->SetSound(Sound);
	Event->SetDialogueWave(DialogueWave);

	for (UDialogueEvent* Outcome : Outcomes)
	{
		Event->AddOutcome(Outcome);
	}

	return Event;
}

UDialogueEvent* UDialogueBlueprintLibrary::MakeDialogueDummy(TArray<UDialogueEvent*> Outcomes)
{
	UDialogueEvent* Event = NewObject<UDialogueEvent>();
	
	for (UDialogueEvent* Outcome : Outcomes)
	{
		Event->AddOutcome(Outcome);
	}

	return Event;
}

UDialogueBlueprintEvent* UDialogueBlueprintLibrary::MakeDialogueBlueprintEvent(FName ReadEventName, UObject* Object, FName Condition, FName EventName)
{
	UDialogueBlueprintEvent* Event = NewObject<UDialogueBlueprintEvent>();
	Event->BindReadEvent(Object, ReadEventName);
	Event->BindBlueprintEvent(Object, EventName);
	Event->SetCondition(Condition);

	return Event;
}

void UDialogueBlueprintLibrary::SetTextVariableByName(UObject* Object, FName VarName, FText Value)
{
	UProperty* Property = Object->GetClass()->FindPropertyByName(VarName);
	
	if (Property)
	{
		FText* ptrValue = Property->ContainerPtrToValuePtr<FText>(Object);
		if (ptrValue)
		{
			*ptrValue = Value;
		}
	}
}

void UDialogueBlueprintLibrary::SetBoolVariableByName(UObject* Object, FName VarName, bool Value)
{
	UProperty* Property = Object->GetClass()->FindPropertyByName(VarName);

	if (Property)
	{
		bool* ptrValue = Property->ContainerPtrToValuePtr<bool>(Object);
		if (ptrValue)
		{
			*ptrValue = Value;
		}
	}
}

FString UDialogueBlueprintLibrary::ProcessString(FString StringIn, TMap<FName, FString> Parameters)
{
	FString Result = StringIn;

	for (TPair<FName, FString> Param : Parameters)
	{
		FString Key = "[" + Param.Key.ToString() + "]";
		Result = Result.Replace(*Key, *Param.Value);
	}

	return Result;
}