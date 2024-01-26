// Copyright Epic Games, Inc. All Rights Reserved.

#include "WWZGameMode.h"
#include "WWZCharacter.h"
#include "UObject/ConstructorHelpers.h"

AWWZGameMode::AWWZGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
