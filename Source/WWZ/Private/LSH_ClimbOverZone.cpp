// Fill out your copyright notice in the Description page of Project Settings.


#include "LSH_ClimbOverZone.h"
#include "Components/BoxComponent.h"


// Sets default values
ALSH_ClimbOverZone::ALSH_ClimbOverZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//좀비가 오버랩되면 기어오르기 애니메이션 재생하도록
	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	SetRootComponent(boxComp);
	boxComp->SetMobility(EComponentMobility::Static);

	boxComp->SetCollisionProfileName(TEXT("ClimbZone"));
}

// Called when the game starts or when spawned
void ALSH_ClimbOverZone::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALSH_ClimbOverZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

