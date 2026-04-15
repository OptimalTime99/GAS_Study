// Copyright Epic Games, Inc. All Rights Reserved.

#include "GAS_StudyGameMode.h"

#include "Character/MyPlayerState.h"

AGAS_StudyGameMode::AGAS_StudyGameMode()
{
	PlayerStateClass = AMyPlayerState::StaticClass();
}

