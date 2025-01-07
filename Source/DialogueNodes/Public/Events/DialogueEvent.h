// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DialogueEvent.generated.h"

FString Tabs(int32 Tabs);

DECLARE_DELEGATE(DialogueReadDelegate)

/** General Dialogue event. Instances of this class act as empty events. */
UCLASS()
class DIALOGUENODES_API UDialogueEvent : public UObject
{
	GENERATED_BODY()

private:
	/** The dynamic condition to which this event will be available. */
	UPROPERTY()
	FName Condition;

	/** Should this event be the only one to be processed (if its condition is true), excluding all non-solo events prior to this one. Default is true. */
	UPROPERTY()
	bool bIsEventSolo;

	/** Should this event be processed whatever the other events are. It forbids this event to have any outcome ! Meant for extra-actions events. Default is false. */
	UPROPERTY()
	bool bIsEventExtra;

	/** The priority ordered list of events this event should lead to. Can be empty for Dialogue final events. */
	UPROPERTY()
	TArray<UDialogueEvent*> Outcomes;

protected:
	/** The event to fire in order to compute dynamic arguments. */
	DialogueReadDelegate ReadEvent;

	/** Array filled with visible options at execution time. */
	UPROPERTY()
	TArray<UDialogueEvent*> Options;

public:
	UDialogueEvent() : UDialogueEvent(true, false) {}
	UDialogueEvent(bool bEventSolo, bool bEventExtra) : bIsEventSolo(bEventSolo), bIsEventExtra(bEventExtra), Condition("") {}

	/** Bind the event that will be fired to compute dynamic arguments. Returns true if bound successfully. */
	bool BindReadEvent(UObject* Object, FName EventName);
	/** Fire the event that will compute dynamic arguments. */
	void FireReadEvent();
	/** Retrieves the object which owns the read event */
	UObject* GetReadEventObject();

	/** True means this event will be processed alone (only if its condition is true), excluding all non-solo events prior to this one. Default is true.
	* Only "extra" events will be processed whatsoever.*/
	void SetEventSolo(bool bIsSolo);
	/** Should this event be the only one to be processed (if its condition is true), excluding all non-solo events prior to this one. Default is true. */
	bool IsEventSolo();

	/** True means this event will be processed whatever the other events are (solo or non-solo).
	* This event won't have any outcome taken into consideration. This option is meant for extra-action events : giving an item to the player, updating a variable, etc. */
	void SetEventExtra(bool bIsExtra);
	/** Should this event be processed whatever the other events are. It forbids this event to have any outcome ! Meant for extra-actions events. Default is false. */
	bool IsEventExtra();

	/** Set the dynamic condition to which this event will be available. */
	void SetCondition(FName ConditionIn);
	/** Evaluate the dynamic condition of this event in the NPC-Character's context. */
	bool EvaluateCondition();

	/** Add an outcome at the end of the previous ones. */
	void AddOutcome(UDialogueEvent* Event);
	/** Try to find and remove the first Goto with the specified reference, maintaining events order. */
	void RemoveGoto(int32 Reference);
	/** Replace the first met Goto reference with the NewEvent. */
	void ReplaceGoto(int32 Reference, UDialogueEvent* NewEvent);
	/** Find the index of the first Goto with the specified reference. INDEX_NONE if not found. */
	int32 FindGotoIndex(int32 Referecence);
	/** The priority ordered list of events this event should lead to. Can be empty for Dialogue final events, but prefer using DialogueEnd events. */
	TArray<UDialogueEvent*> GetOutcomes();

	/** Execute event in Character's context. This supposes that condition evaluation and checks have been made before.
	* It also handles the processing and execution of next events so that you only have to start Dialogue with the first event execution. */
	virtual UDialogueEvent* ExecuteEvent(class UDialogue* Dialogue);
};
