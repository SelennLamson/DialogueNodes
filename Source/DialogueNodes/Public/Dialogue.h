// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Events/DialogueEvent.h"
#include "Events/DialogueOption.h"
#include "Dialogue.generated.h"

DECLARE_DELEGATE(DialogueDelegate)

UCLASS(BlueprintType)
/** A Dialogue containing entry points to a graph of Dialogue events. */
class DIALOGUENODES_API UDialogue : public UObject
{
	GENERATED_BODY()
	
private:
	/** The starting event, which is dummy and created at setup. */
	UPROPERTY()
	UDialogueEvent* StartingEvent;

	/** Is the dialogue currently executed? If so, only one execution can be done at a time. */
	UPROPERTY()
	bool bIsCurrentlyExecuted;

	/** The event to execute next. */
	UPROPERTY()
	UDialogueEvent* NextEvent;

	/** Does the dialogue is currently waiting for an input before proceeding to next event? */
	UPROPERTY()
	bool bIsWaitingForInput;

	/** The current dialogue option events available, when waiting for input. */
	UPROPERTY()
	TArray<UDialogueEvent*> CurrentOptions;

	/** Event to fire when a TextEvent (not an Option event) is encountered. */
	DialogueDelegate LineDelegate;

	/** The current line struct (already processed), for display purposes. */
	UPROPERTY()
	FLineStruct LineStruct;

	/** The option structs of visible options, for display purposes. */
	UPROPERTY()
	TArray<FOptionStruct> LineOptions;

	/** Event to fire when dialogue reaches an end or is stopped. */
	DialogueDelegate EndDelegate;
	
public:
	UDialogue() { }

	/** Set the starting event of the Dialogue. */
	void SetStartingEvent(UDialogueEvent* Event);
	/** The starting event of the Dialogue. */
	UDialogueEvent* GetStartingEvent();

	/** Begin the execution of the dialogue, by executing the starting event and registering the two execution delegates. */
	void Launch(UObject* Object, FName LineEventName, FName EndEventName);

	/** Select an option when a line event with options was encountered then make the dialogue continue. */
	void SelectOption(int32 OptionId);

	/** Stop the dialogue directly, without waiting for a proper dialogue end. */
	void Stop();

	/** Fire the line event for display and mark the dialogue as waiting for input, with corresponding options. 
	If no options are available to the player, but the OptionEvents array isn't empty, the first event will be executed when input received. */
	void FireLineEvent(FLineStruct Line, TArray<FOptionStruct> Options, TArray<UDialogueEvent*> OptionEvents);

	FLineStruct GetLineStruct();

	TArray<FOptionStruct> GetLineOptions();

private:
	void ExecuteEvent();
};

UCLASS()
/** A Dialogue builder. */
class DIALOGUENODES_API UDialogueBuilder : public UObject
{
	GENERATED_BODY()

private:
	/** The starting event of the Dialogue. */
	UDialogueEvent* StartingEvent;

	/** The list of event that have already been processed during setup, for recursive search end. */
	TArray<UDialogueEvent*> SetupEvents;

	/** The list of found incoming links during recursive search for Gotos. */
	TMap<int32, UDialogueEvent*> IncomeLinks;

	/** The list of found outcoming links during recursive search for Gotos. */
	TMap<int32, TArray<UDialogueEvent*>> OutcomeLinks;

public:
	/** Set the starting event of the Dialogue. */
	void SetStartingEvent(UDialogueEvent* Event);

	/** Setup the Dialogue by looking for Goto events and processing them. */
	UDialogue* SetupDialogue();

private:
	/** Recursively look for Goto events in the Dialogue, registering them. */
	void RecursiveLookForGoto(UDialogueEvent* Event);

	/** Resolve all registered Goto links */
	void ResolveGotoLinks();
};
