// Fill out your copyright notice in the Description page of Project Settings.


#include "LSH_ZombieManager.h"
#include "LSH_BaseZom.h"


// Sets default values
ALSH_ZombieManager::ALSH_ZombieManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALSH_ZombieManager::BeginPlay()
{
	Super::BeginPlay();
	
	for (int i = 0;  i < zomMaxNum;  i++)
	{
		//좀비를 지정된 수만큼 만든다
		ALSH_BaseZom* zombie = GetWorld()->SpawnActor<ALSH_BaseZom>(zombieFactory);

		//좀비를 비활성화한다

		//좀비를 리스트에 담는다
	}
}

// Called every frame
void ALSH_ZombieManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

