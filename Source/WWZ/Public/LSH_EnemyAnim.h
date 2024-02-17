// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "LSH_EnemyFSM.h"
#include "LSH_EnemyAnim.generated.h"

/**
 * 
 */
UCLASS()
class WWZ_API ULSH_EnemyAnim : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FSM")
	EEnemyState animState;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FSM")
	bool bAttackPlay = false;

	UFUNCTION(BlueprintCallable, Category = "FSMEvent")
	void OnEndAttackAnimation();

	UFUNCTION(BlueprintImplementableEvent, Category = "FSMEvent")
	void PlayDamageAnim(FName sectionName);

	//죽음 상태 애니메이션 종료 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FSM")
	bool bDieDone = false;

	//기어올라가기 몽타지 재생
	UFUNCTION(BlueprintImplementableEvent, Category = "FSMEvent")
	void PlayClimbUpAnim();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FSM")
	bool bClimbOver = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FSM")
	float speed = 0;
};
