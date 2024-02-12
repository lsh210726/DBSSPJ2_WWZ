// Fill out your copyright notice in the Description page of Project Settings.


#include "LSH_ClimbSurface.h"

// Sets default values
ALSH_ClimbSurface::ALSH_ClimbSurface()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	PlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaneMesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/StarterContent/Shapes/Shape_Plane.Shape_Plane"));

	if (MeshAsset.Succeeded())
	{
		PlaneMesh->SetStaticMesh(MeshAsset.Object);
		PlaneMesh->SetRelativeLocation(FVector(0, 0, 0));
	}

}

// Called when the game starts or when spawned
void ALSH_ClimbSurface::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALSH_ClimbSurface::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

