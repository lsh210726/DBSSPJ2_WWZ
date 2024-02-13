// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LSH_BaseZom.generated.h"

UCLASS()
class WWZ_API ALSH_BaseZom : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ALSH_BaseZom();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FSMComponent")
	class ULSH_EnemyFSM* fsm;


	UPROPERTY(EditAnywhere, Category = "climb")
	float climbDistance = 100.0f;

	UFUNCTION()
	void ClimbMovement(FVector worldDir);

	UPROPERTY(EditAnywhere)
	float climbSpeed = 0.3f;

	///모서리에서 올라가기 계산
	UFUNCTION()
	void LedgeMantleCalc(FVector startLoc);

	//DoOnce
	bool doOnce = false;



	UFUNCTION()
	void ClimbUpCompleted();

	float currentTime = 0;

	UFUNCTION()
	void FallingAction();

	UPROPERTY()
	class UCharacterMovementComponent* CharMov;

	// 충돌 이벤트 핸들러
	UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// 오버랩 이벤트 핸들러
    UFUNCTION()
    void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
