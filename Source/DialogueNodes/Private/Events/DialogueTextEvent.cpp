// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "Events/DialogueTextEvent.h"
#include "DialogueBlueprintLibrary.h"
#include "Dialogue.h"

void UDialogueTextEvent::SetTextParameters(TMap<FName, FName> Parameters)
{
	TextParameters = Parameters;
}

void UDialogueTextEvent::SetMetadataParameters(TMap<FName, FString> Parameters)
{
	MetadataParameters = Parameters;
}

void UDialogueTextEvent::SetSound(USoundBase* SoundIn)
{
	Sound = SoundIn;
}

void UDialogueTextEvent::SetDialogueWave(UDialogueWave* WaveIn)
{
	DialogueWave = WaveIn;
}

void UDialogueTextEvent::SetText(FText TextIn)
{
	Text = TextIn;
}

FText UDialogueTextEvent::GetRawText()
{
	return Text;
}

TMap<FName, FString> UDialogueTextEvent::GetTextParametersValues()
{
	TMap<FName, FString> ParamValues;

	for (TPair<FName, FName> Parameter : TextParameters)
	{
		FString Value = "";

		if (Parameter.Value != NAME_None && Parameter.Value.ToString() != "" && ReadEvent.IsBound())
		{
			UProperty* Property = GetReadEventObject()->GetClass()->FindPropertyByName(Parameter.Value);
			if (Property)
			{
				if (FText* ptrValue = Property->ContainerPtrToValuePtr<FText>(GetReadEventObject()))
				{
					Value = (*ptrValue).ToString();
				}
			}
		}

		ParamValues.Add(Parameter.Key, Value);
	}

	return ParamValues;
}

FString UDialogueTextEvent::GetProcessedText()
{
	return UDialogueBlueprintLibrary::ProcessString(Text.ToString(), GetTextParametersValues());
}

TMap<FName, FString> UDialogueTextEvent::GetProcessedMetadata()
{
	TMap<FName, FString> ProcessedMetadata;
	TMap<FName, FString> ParamValues = GetTextParametersValues();

	for (const TPair<FName, FString>& Meta : MetadataParameters)
	{
		ProcessedMetadata.Add(Meta.Key, UDialogueBlueprintLibrary::ProcessString(Meta.Value, ParamValues));
	}

	return ProcessedMetadata;
}

UDialogueEvent* UDialogueTextEvent::ExecuteEvent(UDialogue* Dialogue)
{
	UDialogueEvent* NextEvent = UDialogueEvent::ExecuteEvent(Dialogue);

	TArray<FOptionStruct> OptionStructs;
	for (UDialogueEvent* Event : Options)
	{
		if (UDialogueOption* Option = Cast<UDialogueOption>(Event))
		{
			OptionStructs.Add(Option->GetOptionStruct());
		}
	}

	if (NextEvent)
	{
		Options.Empty();
		Options.Add(NextEvent);
	}

	Dialogue->FireLineEvent(GetLineStruct(), OptionStructs, Options);
	return this;
}

FLineStruct UDialogueTextEvent::GetLineStruct()
{
	FLineStruct Struct = FLineStruct(GetProcessedText(), Sound, DialogueWave);
	Struct.MetadataParameters = GetProcessedMetadata();
	return Struct;
}