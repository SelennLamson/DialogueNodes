// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"
#include "DetailLayoutBuilder.h"

#include "Nodes/BPNode_DialogueTextEvent.h"

class FDialogueDetailsCustomization : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance()
	{
		return MakeShareable(new FDialogueDetailsCustomization);
	}

	void CustomizeDetails(IDetailLayoutBuilder & DetailBuilder)
	{
		FPropertyEditorModule& EditModule = FModuleManager::Get().GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		FDetailsViewArgs DetailsViewArgs();// /*bUpdateFromSelection=*/ false, /*bLockable=*/ false, /*bAllowSearch=*/ true, ENameAreaSettings, /*bHideSelectionTip=*/ true, /*InNotifyHook=*/ NotifyHook, /*InSearchInitialKeyFocus=*/ false, /*InViewIdentifier=*/ InArgs._ViewIdentifier);
		
		TSharedRef<IDetailsView> PropertyView = EditModule.CreateDetailView(DetailsViewArgs);
		TArray<TWeakObjectPtr<UObject>> Objects = DetailBuilder.GetDetailsView()->GetSelectedObjects();
		
		if (Objects.Num() == 1 && Objects[0].Get())
		{
			if (UBPNode_DialogueTextEvent* Node = Cast<UBPNode_DialogueTextEvent>(Objects[0].Get()))
			{
				PropertyView->SetObject(Node->DataObject);
			}
		}
	}
};