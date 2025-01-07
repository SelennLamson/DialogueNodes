// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Events/DialogueTextEvent.h"
#include "DialogueOption.generated.h"

USTRUCT(BlueprintType)
struct DIALOGUENODES_API FOptionStruct
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString String;

	UPROPERTY(BlueprintReadWrite)
	TMap<FName, FString> MetadataParameters;

	UPROPERTY(BlueprintReadWrite)
	bool bVisible;

	UPROPERTY(BlueprintReadWrite)
	bool bAvailable;

	UPROPERTY(BlueprintReadWrite)
	USoundBase* Sound;

	UPROPERTY(BlueprintReadWrite)
	UDialogueWave* DialogueWave;

	FOptionStruct() : FOptionStruct("", false, false, nullptr, nullptr) { }
	FOptionStruct(FString StringIn, bool bVisibleIn, bool bAvailableIn, USoundBase* SoundIn, UDialogueWave* WaveIn) :
		String(StringIn), bVisible(bVisibleIn), bAvailable(bAvailableIn), Sound(SoundIn), DialogueWave(WaveIn) { }
};

UCLASS()
/** Dialogue option available to the player in order to answer a NPC, or to make a general choice during a Dialogue. */
class DIALOGUENODES_API UDialogueOption : public UDialogueTextEvent
{
	GENERATED_BODY()
	
private:
	/** Dynamic condition to which this Dialogue option will be available. */
	UPROPERTY()
	FName AvailabilityCondition;

public:
	UDialogueOption() : UDialogueTextEvent(false), AvailabilityCondition("") {}

	/** Set the dynamic condition to which this Dialogue option will be available.
	* If the event's general condition is true but its availability condition is false, the option will be visible to the player but grayed-out.
	* If the event's general condition is false, the option won't be visible, whatever its availability condition value. */
	void SetAvailabilityCondition(FName ConditionIn);

	/** Evaluate the dynamic condition to which the option will be available to the player, in the NPC-Character's context */
	bool EvaluateAvailability();

	/** Execute event in Character's context. This supposes that condition evaluation and checks have been made before. */
	virtual UDialogueEvent* ExecuteEvent(class UDialogue* Dialogue) override;

	FOptionStruct GetOptionStruct();
};
