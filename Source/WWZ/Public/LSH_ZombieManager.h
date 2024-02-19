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
	class UStaticMeshComponent* queueLocation;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* spawnLocation1;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* spawnLocation2;

	FVector spawnVec1;
	FVector spawnVec2;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ALSH_BaseZom> zombieFactory;

	//좀비 스폰 대기열
	UPROPERTY()
	TArray<class ALSH_BaseZom*> zombieQueue;

	//시체 리스트
	UPROPERTY()
	TArray<class ALSH_BaseZom*> bodyList;

	UPROPERTY(EditAnywhere, Category = "zombie manager")
	int32 zomMaxNum = 90;

	UPROPERTY(EditAnywhere, Category = "zombie manager")
	int32 bodyMaxNum = 20;

	UPROPERTY(EditAnywhere, Category = "zombie manager")
	int32 queueMaxNum = 50;

	float currentTime = 0;

	//좀비가 스폰될 지 안될지 결정
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bisSpawning = false;

	//좀비가 어떤 스폰위치에서 스폰될지 결정
	bool spawnLocChecker = true;

	//좀비가 죽으면 현재 시체 수를 세고 시체리스트 최대수보다 적다면 죽은 좀비를 시체리스트에 넣는다
	//시체리스트에 이미 시체가 다 찼다면 맨 처음 시체를 비활성화시키고 대기열에 집어넣는다

	//대기열에 좀비가 대기열최대수보다 많다면 대기열의 좀비를 스폰시킨다
	UFUNCTION()
	void bodyCollecting(class ALSH_BaseZom* zombie);

	//좀비가 스폰되어야 할 타이밍이면 대기열 안의 좀비들을 전부 스폰시킨다

};
