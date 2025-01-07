// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "DialogueSettings.h"

UDataTable* UDialogueSettings::GetTextStyleSet() const
{
	return Cast<UDataTable>(TextStyleSetName.TryLoad());
}