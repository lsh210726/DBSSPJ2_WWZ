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

	UFUNCTION()
	void ClimbAction();

	UPROPERTY(EditAnywhere, Category = "climb")
	float climbDistance = 100;

	UFUNCTION()
	void ClimbMovement(FVector worldDir, float scale);
};
