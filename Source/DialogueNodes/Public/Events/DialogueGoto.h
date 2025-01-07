// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Events/DialogueEvent.h"
#include "DialogueGoto.generated.h"

UCLASS()
/** Dialogue node that establish a goto-link between two points in the Dialogue. Not blueprintable by the user, an easy-to-use node is made for them instead. */
class DIALOGUENODES_API UDialogueGoto : public UDialogueEvent
{
	GENERATED_BODY()

private:
	/** The reference that will be generated later to hook the "From" and the "Destination" nodes together and establish the link. */
	UPROPERTY()
	int32 Reference;

	/** Is this node for the destination of the goto?
	* If true, even if the node is linked to an output of another event, it will be replaced by an incoming link in this event.
	* If false, then it is a "From" node. It will be treated as an outcome for the linked event, and it will replace its own reference to the destination event later. */
	UPROPERTY()
	bool bIsDestinationNode;

public:
	UDialogueGoto() : UDialogueEvent(), Reference(0) {}

	/** Set the reference that will be generated later to hook the "From" and the "Destination" nodes together and establish the link. */
	void SetReference(int32 Ref);
	/** The reference that will be generated later to hook the "From" and the "Destination" nodes together and establish the link. */
	int32 GetReference();

	/** Set if this node should be treated as the destination of the goto. */
	void SetDestinationNode(bool bIsDestination);
	/** Is this node for the destination of the goto? */
	bool IsDestinationNode();
};
