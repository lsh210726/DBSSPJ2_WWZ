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


	static ConstructorHelpers::FObjectFinder<USkeletalMesh> zomMesh1(TEXT("/Script/Engine.SkeletalMesh'/Game/Essets/Character/Zombie/Model/zom2/Ch10_nonPBR__1_.Ch10_nonPBR__1_'"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> zomMesh2(TEXT("/Script/Engine.SkeletalMesh'/Game/Essets/Character/Zombie/Model/zom2/copzombie_l_actisdato.copzombie_l_actisdato'"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> zomMesh3(TEXT("/Script/Engine.SkeletalMesh'/Game/Essets/Character/Zombie/Model/zom2/Prisoner_B_Styperek.Prisoner_B_Styperek'"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> zomMesh4(TEXT("/Script/Engine.SkeletalMesh'/Game/Essets/Character/Zombie/Model/zom2/Survivor_A_Lusth.Survivor_A_Lusth'"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> zomMesh5(TEXT("/Script/Engine.SkeletalMesh'/Game/Essets/Character/Zombie/Model/zom2/Warzombie_F_Pedroso.Warzombie_F_Pedroso'"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> zomMesh6(TEXT("/Script/Engine.SkeletalMesh'/Game/Essets/Character/Zombie/Model/zom2/Yaku_J_Ignite__1_.Yaku_J_Ignite__1_'"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> zomMesh7(TEXT("/Script/Engine.SkeletalMesh'/Game/Essets/Character/Zombie/Model/zom2/Zombiegirl_W_Kurniawan__1_.Zombiegirl_W_Kurniawan__1_'"));

	if (zomMesh1.Succeeded()) mesh1 = zomMesh1.Object;
	if (zomMesh2.Succeeded()) mesh2 = zomMesh2.Object;
	if (zomMesh3.Succeeded()) mesh3 = zomMesh3.Object;
	if (zomMesh4.Succeeded()) mesh4 = zomMesh4.Object;
	if (zomMesh5.Succeeded()) mesh5 = zomMesh5.Object;
	if (zomMesh6.Succeeded()) mesh6 = zomMesh6.Object;
	if (zomMesh7.Succeeded()) mesh7 = zomMesh7.Object;


	if (zomMesh1.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(mesh1);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -88), FRotator(0, -90, 0));
	}

	ConstructorHelpers::FClassFinder<UAnimInstance> tempClass(TEXT("/Script/Engine.AnimBlueprint'/Game/Essets/Character/Zombie/Anim/ABP_Enemy.ABP_Enemy_C'"));
	if (tempClass.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(tempClass.Class);
	}

	//AI FSM
	fsm = CreateDefaultSubobject<ULSH_EnemyFSM>(TEXT("FSM"));

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

	GetMesh()->SetSkeletalMesh(mesh2);


	int32 RandomNumber = FMath::RandRange(1, 7);
	switch (RandomNumber)
	{
	case 1:
		if (mesh1 != nullptr)
		{
			GetMesh()->SetSkeletalMesh(mesh1);
			break;
		}
	case 2:
		if (mesh2 != nullptr)
		{
			GetMesh()->SetSkeletalMesh(mesh2);
			break;
		}
	case 3:
		if (mesh3 != nullptr)
		{
			GetMesh()->SetSkeletalMesh(mesh3);
			break;
		}
	case 4:
		if (mesh4 != nullptr)
		{
			GetMesh()->SetSkeletalMesh(mesh4);
			break;
		}
	case 5:
		if (mesh5 != nullptr)
		{
			GetMesh()->SetSkeletalMesh(mesh5);
			break;
		}
	case 6:
		if (mesh6 != nullptr)
		{
			GetMesh()->SetSkeletalMesh(mesh6);
			break;
		}
	case 7:
		if (mesh7 != nullptr)
		{
			GetMesh()->SetSkeletalMesh(mesh7);
			break;
		}
	default:
		// Handle unexpected cases
		break;
	}

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
	//if (Cast<ALSH_ClimbSurface>(OtherActor) != nullptr)
	//{
	//	fsm->anim->bClimbOver = false;
	//	return;
	//}
	//클라임오버존에 도착했다면 기어오르기 시작
		if (!fsm->anim->bClimbOver&&Cast<ALSH_ClimbOverZone>(OtherActor)!=nullptr)
		{
			fsm->anim->bClimbOver = true;
			//이것 초기화 해야함

		}

		auto a = Cast<ALSH_ClimbZone>(OtherActor);
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
