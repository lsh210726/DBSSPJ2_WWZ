// Fill out your copyright notice in the Description page of Project Settings.


#include "LSH_ZombieManager.h"
#include "LSH_BaseZom.h"
#include "LSH_EnemyFSM.h"
#include <Kismet/GameplayStatics.h>
#include "LSH_ClimbZone.h"


// Sets default values
ALSH_ZombieManager::ALSH_ZombieManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	queueLocation = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("queue location"));
	spawnLocation1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("spawn location1"));
	spawnLocation2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("spawn location2"));

}

// Called when the game starts or when spawned
void ALSH_ZombieManager::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> climbZoneArray;
	//UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), ALSH_ClimbZone::StaticClass(),TEXT("1"));
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALSH_ClimbZone::StaticClass(),climbZoneArray);

	for (auto a : climbZoneArray)
	{
		if (a->GetActorLabel().Contains(TEXT("1")))
		{
			
			spawnVec1 = a->GetActorLocation();
			UE_LOG(LogTemp, Warning, TEXT("spwnvec1 :%f, %f, %f"),spawnVec1.X, spawnVec1.Y, spawnVec1.Z);
		}
		else if (a->GetActorLabel().Contains(TEXT("2")))
		{
			spawnVec2 = a->GetActorLocation();
			UE_LOG(LogTemp, Warning, TEXT("spwnvec2 :%f, %f, %f"), spawnVec2.X, spawnVec2.Y, spawnVec2.Z);

		}
	}

	//레벨에 배치된 클라임존 1, 2를 찾고 위치 정보를 저장해야 한다 
	
	for (int i = 0;  i < zomMaxNum;  i++)
	{
		//좀비를 지정된 수만큼 만든다
		ALSH_BaseZom* zombie = GetWorld()->SpawnActor<ALSH_BaseZom>(zombieFactory);
		if (zombie == nullptr) continue;

		//좀비를 리스트에 담는다
		zombieQueue.Add(zombie);
	}
	for (auto z1 : zombieQueue)
	{
		if (nullptr == z1)
			continue;
		//좀비매니저를 알려준다
		z1->fsm->zombieManager = this;

		//좀비를 비활성화한다
		z1->fsm->DeActiveAction();

	}
	//bisSpawning = true;
}

// Called every frame
void ALSH_ZombieManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	currentTime += DeltaTime;
	if (bisSpawning && zombieQueue.Num() > 0 && currentTime > 0.1)
	{
		auto zombie = zombieQueue[0];
		//zombie->SetActorTransform(spawnLocChecker ? spawnLocation1->GetComponentTransform():spawnLocation2->GetComponentTransform());
		zombie->SetActorLocation(spawnLocChecker ? spawnLocation1->GetComponentLocation() + FMath::VRand() * 200 :spawnLocation2->GetComponentLocation() + FMath::VRand() * 200);
		//zombie->SetActorLocation(GetActorLocation() + FMath::VRand()*100);
		zombie->fsm->ActiveAction(spawnLocChecker ? spawnVec1 : spawnVec2);
		zombieQueue.RemoveAt(0);
		currentTime = 0;
	}
	else if (bisSpawning && zombieQueue.Num() <= 0)
	{
		//스폰 끄기
		bisSpawning = false;
		//스폰 위치 바꾸기
		spawnLocChecker = !spawnLocChecker;
	}

	//UE_LOG(LogTemp, Log, TEXT(" zombieQueueNum : %d, bodyListNum : %d"), zombieQueue.Num(), bodyList.Num());
}

void ALSH_ZombieManager::bodyCollecting(class ALSH_BaseZom* zombie)
{
	//만약 이미 시체리스트가 다 찼다면
	if (bodyMaxNum<=bodyList.Num())
	{
		//제일 먼저 들어온 좀비를 좀비 대기열에 등록한다
		zombieQueue.Add(bodyList[0]);
		//좀비를 비활성화한다
		bodyList[0]->fsm->DeActiveAction();
		//시체리스트에서 제일 먼저 들어온 좀비를 뺀다
		bodyList.RemoveAt(0);
		//시체리스트에 새로 들어온 좀비를 등록한다
		bodyList.Add(zombie);

		//만약 대기열이 꽉 찼다면
		if (zombieQueue.Num() >= queueMaxNum)
		{
			//스폰모드를 켠다
			bisSpawning = true;
		}
	}
	else
	{
		bodyList.Add(zombie);
	}
}
