// Fill out your copyright notice in the Description page of Project Settings.


#include "LSH_BaseZom.h"
#include "LSH_EnemyFSM.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Components/CapsuleComponent.h>
#include "LSH_ClimbSurface.h"
//밑에 두개 헤더는 정리 필요..
#include "AIController.h"
#include "LSH_ClimbZone.h"
#include "LSH_EnemyAnim.h"
#include "LSH_ClimbOverZone.h"



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

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("NoCollision"));

	bIsColliding = false;

}

// Called when the game starts or when spawned
void ALSH_BaseZom::BeginPlay()
{
	Super::BeginPlay();
	CharMov = GetCharacterMovement();

	// OnComponentHit 이벤트 바인딩
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ALSH_BaseZom::OnHit);
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ALSH_BaseZom::OnBeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &ALSH_BaseZom::OnEndOverlap);

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
	CharMov->AddInputVector(Direction);


}

void ALSH_BaseZom::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{


	//만약 부딛힌 게 클라임존이라면
	auto hitZom = Cast<ALSH_BaseZom>(OtherActor);
	auto climbsurface = Cast<ALSH_ClimbSurface>(OtherActor);
	if (!bIsColliding && climbsurface)
	{
		fsm->ClimbAction();
		bIsColliding = true;
		SetActorRotation(FRotator(0, (climbsurface->GetActorUpVector()*-1).Rotation().Yaw, 0));
	}

}

void ALSH_BaseZom::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	//클라임오버존에 도착했다면 기어오르기 시작
		auto a = Cast<ALSH_ClimbZone>(OtherActor);
		if (Cast<ALSH_ClimbOverZone>(OtherActor)!=nullptr)
		{
			fsm->anim->bClimbOver = true;
		}
		if (a != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("다 올라감!"));
			CharMov->SetMovementMode(MOVE_Walking);
			fsm->FallState();
			fsm->climbMode = false;
		}
}

void ALSH_BaseZom::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("endoverlap!"));

	//떨어지기
	if (bIsColliding)
	{
		bIsColliding = false;

		// 여기서 원하는 동작을 수행합니다.
		UE_LOG(LogTemp, Warning, TEXT("이동 혹은 떨어짐!"));

		CharMov->SetMovementMode(MOVE_Walking);
		fsm->FallState();
	}
}
