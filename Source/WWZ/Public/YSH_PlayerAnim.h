// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "YSH_PlayerAnim.generated.h"

/**
 * 
 */
UCLASS()
class WWZ_API UYSH_PlayerAnim : public UAnimInstance
{
	GENERATED_BODY()
	

public:

virtual void NativeUpdateAnimation( float DeltaSeconds ) override;

void PlayAttackAnim();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isFalling;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float speed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float direction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float pitch;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//class UAnimMontage* playerFireMontage;
public:
	UFUNCTION()
	void PlayerAttackAnim();

	UPROPERTY(EditAnywhere)
	class AYSH_Player* player;

	UPROPERTY(EditDefaultsOnly, Category = PlayerAnim)
	class UAnimMontage* playFireanimMontage;

};
