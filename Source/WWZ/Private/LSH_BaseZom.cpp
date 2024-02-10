// Fill out your copyright notice in the Description page of Project Settings.


#include "LSH_BaseZom.h"
#include "LSH_EnemyFSM.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Components/CapsuleComponent.h>
//밑에 두개 헤더는 정리 필요..
#include "AIController.h"
#include "LSH_ClimbZone.h"
#include "LSH_EnemyAnim.h"



// Sets default values
ALSH_BaseZom::ALSH_BaseZom()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Essets/Character/Zombie/Model/Ch20_nonPBR.Ch20_nonPBR'"));

	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -88), FRotator(0, -90, 0));
	}

	//AI FSM
	fsm = CreateDefaultSubobject<ULSH_EnemyFSM>(TEXT("FSM"));

	ConstructorHelpers::FClassFinder<UAnimInstance> tempClass(TEXT("/Script/Engine.AnimBlueprint'/Game/Essets/Character/Zombie/Anim/ABP_Enemy.ABP_Enemy_C'"));
	if (tempClass.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(tempClass.Class);
	}

	// OnComponentHit 이벤트 바인딩
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ALSH_BaseZom::OnHit);
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ALSH_BaseZom::OnBeginOverlap);
}

// Called when the game starts or when spawned
void ALSH_BaseZom::BeginPlay()
{
	Super::BeginPlay();
	CharMov = GetCharacterMovement();

	
}

// Called every frame
void ALSH_BaseZom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (fsm->mState)
	{
	case EEnemyState::Idle:
		break;
	case EEnemyState::Move:
		//CharMov->SetMovementMode(MOVE_Walking);
		CharMov->bOrientRotationToMovement = true;
		break;
	case EEnemyState::Attack:
		break;
	case EEnemyState::Damage:
		break;
	case EEnemyState::Die:
		break;
	case EEnemyState::Climb:
		CharMov->SetMovementMode(MOVE_Flying);
		CharMov->bOrientRotationToMovement = false;
		break;
	case EEnemyState::Fall:
		CharMov->SetMovementMode(MOVE_Falling);
		CharMov->bOrientRotationToMovement = true;
		break;
	}
}

// Called to bind functionality to input
void ALSH_BaseZom::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


void ALSH_BaseZom::ClimbMovement(FVector worldDir)
{
	FVector Direction = (worldDir - GetActorLocation()).GetSafeNormal();
	float Distance = (worldDir - GetActorLocation()).Size();
	CharMov->AddInputVector(Direction);


}

void ALSH_BaseZom::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//fsm->ai->StopMovement();
	//ClimbMovement(fsm->climbZone->GetActorLocation());
}

void ALSH_BaseZom::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
		// 여기서 원하는 동작을 수행합니다.
		UE_LOG(LogTemp, Warning, TEXT("Collision has ended!"));

		//클라임존인지 확인
		auto a = Cast<ALSH_ClimbZone>(OtherActor);
		if (a != nullptr)
		{
			CharMov->SetMovementMode(MOVE_Walking);
			fsm->FallState();
			fsm->climbMode = false;
		}
}