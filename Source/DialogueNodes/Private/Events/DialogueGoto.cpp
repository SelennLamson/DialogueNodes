// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "Events/DialogueGoto.h"

void UDialogueGoto::SetReference(int32 Ref)
{
	Reference = Ref;
}

int32 UDialogueGoto::GetReference()
{
	return Reference;
}

void UDialogueGoto::SetDestinationNode(bool bIsDestination)
{
	bIsDestinationNode = bIsDestination;
}

bool UDialogueGoto::IsDestinationNode()
{
	return bIsDestinationNode;
}