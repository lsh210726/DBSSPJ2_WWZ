// Fill out your copyright notice in the Description page of Project Settings.


#include "YSH_BulletActor.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Components/SphereComponent.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/GameFramework/ProjectileMovementComponent.h"

// Sets default values
AYSH_BulletActor::AYSH_BulletActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// sphereComp를 루트로
	sphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("sphereComp"));
	SetRootComponent(sphereComp);
	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("meshComp"));
	meshComp->SetupAttachment(RootComponent);

	movementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("movementComp"));
	movementComp->SetUpdatedComponent(sphereComp);

	// speed설정, 바운드설정
	movementComp->InitialSpeed = 2000.f;
	movementComp->MaxSpeed = 2000.f;
	movementComp->bShouldBounce = true;

	sphereComp->SetCollisionProfileName(TEXT("BlockAll"));
	meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 메시의 크기를 0.25로 하고싶다.
	meshComp->SetWorldScale3D(FVector(0.25f));
	// 충돌체의 반지름을 12.5 하고싶다
	sphereComp->SetSphereRadius(12.5f);

	//SetLifeSpan(5); 

}

// Called when the game starts or when spawned
void AYSH_BulletActor::BeginPlay()
{
	Super::BeginPlay();
	
	// 총알액터의 수명을 5초로 하고싶다. 타이머를 이용해서 처리하고 싶다.
	//FTimerHandle timerHandle;
	//GetWorld()->GetTimerManager().SetTimer(timerHandle, FTimerDelegate::CreateLambda([this]()->void { this->Destroy(); }), 5, false);
}

// Called every frame
void AYSH_BulletActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

