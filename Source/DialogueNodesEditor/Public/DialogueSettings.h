// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Engine/DeveloperSettings.h"
#include "Engine/DataTable.h"
#include "DialogueData.h"
#include "Templates/Casts.h"
#include "DialogueSettings.generated.h"

UCLASS(config = Editor, defaultconfig, meta = (DisplayName = "Dialogue"))
class DIALOGUENODESEDITOR_API UDialogueSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	// --- Dialogue --- //

	UPROPERTY(EditAnywhere, config, Category = "Dialogue", meta = (AllowedClasses = "DataTable", RowType = "RichTextStyleRow", DisplayName = "Text Style Set", Tooltip = "The rich text style table that will be used to display dialogue nodes and dialogues in game."))
	FSoftObjectPath TextStyleSetName;


	// --- Dialogue Lines --- //

	UPROPERTY(EditAnywhere, config, Category = "Dialogue Lines", meta = (DisplayName = "Metadata Parameters", Tooltip = "Default metadata parameters and associated values for dialogue lines."))
	TMap<FName, FString> LineMetadata;

	UPROPERTY(EditAnywhere, config, Category = "Dialogue Lines", meta = (DisplayName = "Title Parameter", Tooltip = "The name of the metadata parameter to display directly into the line nodes' title."))
	FName LineTitleParameter;

	UPROPERTY(EditAnywhere, config, Category = "Dialogue Lines", meta = (DisplayName = "Title Color Map", Tooltip = "The line nodes' color depending on the possible values of the title parameter."))
	TMap<FString, FLinearColor> LineColorMap;
	

	// --- Dialogue Options --- //

	UPROPERTY(EditAnywhere, config, Category = "Dialogue Options", meta = (DisplayName = "Metadata Parameters", Tooltip = "Default metadata parameters and associated values for dialogue options."))
	TMap<FName, FString> OptionMetadata;

	UPROPERTY(EditAnywhere, config, Category = "Dialogue Options", meta = (DisplayName = "Title Parameter", Tooltip = "The name of the metadata parameter to display directly into the option nodes' title."))
	FName OptionTitleParameter;

	UPROPERTY(EditAnywhere, config, Category = "Dialogue Options", meta = (DisplayName = "Title Color Map", Tooltip = "The option nodes' color depending on the possible values of the title parameter."))
	TMap<FString, FLinearColor> OptionColorMap;

public:
	UDialogueSettings()
	{
		LineMetadata.Add("Character", "");
	}

	// Returns the default translucent material
	UDataTable* GetTextStyleSet() const;
};
