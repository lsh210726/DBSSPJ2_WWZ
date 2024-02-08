// Fill out your copyright notice in the Description page of Project Settings.


#include "YSH_PlayerAnim.h"
#include "YSH_Player.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Animation/AnimInstance.h"

void UYSH_PlayerAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	// 1. ���� ���� �����ͼ� ATPSPlayer�� Cast�ϰ� �ʹ�.
	AYSH_Player* player = Cast<AYSH_Player>(TryGetPawnOwner());
	if (nullptr == player)
	{
		return;
	}
	// 2. ������ velocity, forward vector, right vector�� ��������ʹ�. 
	FVector v = player->GetVelocity();
	FVector forward = player->GetActorForwardVector();
	FVector right = player->GetActorRightVector();
	// 3. speed, direction�� ���� ä���ʹ�.
	speed = FVector::DotProduct(v, forward);
	direction = FVector::DotProduct(v, right);
	// 4. isInAir, isCrouched�� ä���ʹ�.
	isFalling = player->GetCharacterMovement()->IsFalling();

	// 5. ������ aim rotation���� �����ͼ� pitch�� ä���ʹ�.
	pitch = player->GetBaseAimRotation().Pitch;

}

void UYSH_PlayerAnim::PlayerAttackAnim()
{
	//Montage_Play(playerFireMontage);
}
