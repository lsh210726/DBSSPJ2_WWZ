// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LSH_ZombieManager.generated.h"

UCLASS()
class WWZ_API ALSH_ZombieManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALSH_ZombieManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ALSH_BaseZom> zombieFactory;

	//좀비 리스트
	UPROPERTY()
	TArray<class ALSH_BaseZom*> magazine;

	UPROPERTY(EditAnywhere)
	int32 zomMaxNum = 50;
};
