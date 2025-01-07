// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "DialogueNodesEditor.h"
#include "DialoguePin.h"
#include "SGraphDialogueNode.h"
#include "SGraphExecuteDialogueNode.h"


void DialogueNodesEditorImpl::StartupModule() {
	// Factory creation
	TSharedPtr<FDialoguePinFactory> DialoguePinFactory = MakeShareable(new FDialoguePinFactory());
	TSharedPtr<FDialogueNodeFactory> DialogueNodeFactory = MakeShareable(new FDialogueNodeFactory());
	TSharedPtr<FExecuteDialogueNodeFactory> ExecuteDialogueNodeFactory = MakeShareable(new FExecuteDialogueNodeFactory());

	// Factory registering
	FEdGraphUtilities::RegisterVisualPinFactory(DialoguePinFactory);
	FEdGraphUtilities::RegisterVisualNodeFactory(DialogueNodeFactory);
	FEdGraphUtilities::RegisterVisualNodeFactory(ExecuteDialogueNodeFactory);
}

void DialogueNodesEditorImpl::ShutdownModule() { }

IMPLEMENT_MODULE(DialogueNodesEditorImpl, Module)
