// Copyright Epic Games, Inc. All Rights Reserved.

#include "baseTPSGameMode.h"
#include "baseTPSCharacter.h"
#include "UObject/ConstructorHelpers.h"

AbaseTPSGameMode::AbaseTPSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
