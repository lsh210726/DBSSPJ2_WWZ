// Fill out your copyright notice in the Description page of Project Settings.


#include "MissionActor.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Components/BoxComponent.h"
// Sets default values
AMissionActor::AMissionActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("boxComp"));
	SetRootComponent(boxComp);
}

// Called when the game starts or when spawned
void AMissionActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMissionActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

