// Copyright 2019-2029 Thomas Lamson, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class DialogueNodesEditorImpl : public IModuleInterface {
	public:
	/** IModuleInterface implementation */
	void StartupModule();
	void ShutdownModule();
};
