// Fill out your copyright notice in the Description page of Project Settings.


#include "LSH_ClimbZone.h"
#include "Components/BoxComponent.h"

// Sets default values
ALSH_ClimbZone::ALSH_ClimbZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	SetRootComponent(boxComp);
	boxComp->SetMobility(EComponentMobility::Static);

	boxComp->SetCollisionProfileName(TEXT("ClimbZone"));
}

// Called when the game starts or when spawned
void ALSH_ClimbZone::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALSH_ClimbZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

