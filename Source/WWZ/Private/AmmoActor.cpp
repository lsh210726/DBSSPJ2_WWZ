// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoActor.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Components/SphereComponent.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/GameFramework/ProjectileMovementComponent.h"

// Sets default values
AAmmoActor::AAmmoActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// sphereComp�� ��Ʈ��
	sphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("sphereComp"));
	SetRootComponent(sphereComp);
	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("meshComp"));
	meshComp->SetupAttachment(RootComponent);


	sphereComp->SetCollisionProfileName(TEXT("BlockAll"));
	meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// �޽��� ũ�⸦ 0.25�� �ϰ�ʹ�.
	meshComp->SetWorldScale3D(FVector(2.0f));
	// �浹ü�� �������� 12.5 �ϰ�ʹ�
	sphereComp->SetSphereRadius(50.0f);

	//SetLifeSpan(5); 

}

// Called when the game starts or when spawned
void AAmmoActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAmmoActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

