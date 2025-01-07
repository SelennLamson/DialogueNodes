// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Sound/SoundCue.h"
#include "Sound/DialogueWave.h"
#include "Events/DialogueEvent.h"
#include "DialogueTextEvent.generated.h"

USTRUCT(BlueprintType)
struct DIALOGUENODES_API FLineStruct
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString String;

	UPROPERTY(BlueprintReadWrite)
	TMap<FName, FString> MetadataParameters;

	UPROPERTY(BlueprintReadWrite)
	USoundBase* Sound;

	UPROPERTY(BlueprintReadWrite)
	UDialogueWave* DialogueWave;

	FLineStruct() : FLineStruct("", nullptr, nullptr) { }
	FLineStruct(FString StringIn, USoundBase* SoundIn, UDialogueWave* WaveIn) : String(StringIn), Sound(SoundIn), DialogueWave(WaveIn) { }
};

UCLASS()
/** General Dialogue event with text, like NPC-Lines */
class DIALOGUENODES_API UDialogueTextEvent : public UDialogueEvent
{
	GENERATED_BODY()

private:
	/** The text that will be dynamically parsed to be printed during this event processing */
	UPROPERTY()
	FText Text;

	/** The map containing the names of the text parameters and their corresponding properties */
	UPROPERTY()
	TMap<FName, FName> TextParameters;

	/** The map containing the names and the values of metadata parameters */
	UPROPERTY()
	TMap<FName, FString> MetadataParameters;

protected:
	/** The sound asset associated with this text event. */
	UPROPERTY()
	USoundBase* Sound;

	/** The dialogue wave asset associated with this text event. */
	UPROPERTY()
	UDialogueWave* DialogueWave;

public:
	UDialogueTextEvent() : UDialogueTextEvent(true) {}
	UDialogueTextEvent(bool bEventSolo) : UDialogueEvent(bEventSolo, false) {}

	/** Set the map containing the names of the text parameters and their corresponding properties */
	void SetTextParameters(TMap<FName, FName> Parameters);

	/** Set the map containing the names and the values of metadata parameters */
	void SetMetadataParameters(TMap<FName, FString> Parameters);

	/** Set the sound asset associated with this text event */
	void SetSound(USoundBase* SoundIn);

	/** Set the dialogue wave asset associated with this text event */
	void SetDialogueWave(UDialogueWave* WaveIn);

	/** Set the raw text of the Dialogue event */
	void SetText(FText TextIn);
	/** Retrieve unprocessed text */
	FText GetRawText();
	/** Processes the text parameters to retrieve the value of each one of them. It assumes that the read event was fired before. */
	TMap<FName, FString> GetTextParametersValues();
	/** Processes the dynamic text based on the NPC-Character's context. It assumes that the read event was fired before. */
	FString GetProcessedText();
	/** Processes the dynamic metadata parameters based on the NPC-Character's context. It assumes that the read event was fired before. */
	TMap<FName, FString> GetProcessedMetadata();

	/** Execute event in Character's context. This supposes that condition evaluation and checks have been made before. */
	virtual UDialogueEvent* ExecuteEvent(class UDialogue* Dialogue) override;

	FLineStruct GetLineStruct();
};
