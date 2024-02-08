// Fill out your copyright notice in the Description page of Project Settings.


#include "YSH_PlayerAnim.h"
#include "YSH_Player.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Animation/AnimInstance.h"

void UYSH_PlayerAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	// 1. 오너 폰을 가져와서 ATPSPlayer로 Cast하고 싶다.
	AYSH_Player* player = Cast<AYSH_Player>(TryGetPawnOwner());
	if (nullptr == player)
	{
		return;
	}
	// 2. 오너의 velocity, forward vector, right vector를 가져오고싶다. 
	FVector v = player->GetVelocity();
	FVector forward = player->GetActorForwardVector();
	FVector right = player->GetActorRightVector();
	// 3. speed, direction의 값을 채우고싶다.
	speed = FVector::DotProduct(v, forward);
	direction = FVector::DotProduct(v, right);
	// 4. isInAir, isCrouched를 채우고싶다.
	isFalling = player->GetCharacterMovement()->IsFalling();

	// 5. 오너의 aim rotation값을 가져와서 pitch를 채우고싶다.
	pitch = player->GetBaseAimRotation().Pitch;

}

void UYSH_PlayerAnim::PlayerAttackAnim()
{
	//Montage_Play(playerFireMontage);
}
