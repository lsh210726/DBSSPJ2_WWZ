// Fill out your copyright notice in the Description page of Project Settings.


#include "LSH_BaseZom.h"
#include "LSH_EnemyFSM.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Components/CapsuleComponent.h>
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
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
		CharMov->SetMovementMode(MOVE_Walking);
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
	//FVector startPos = GetActorLocation() + FVector(0, 0, 100);
	//FVector endPos = startPos + GetActorForwardVector() * climbDistance;
	//DrawDebugLine(GetWorld(), startPos, endPos, FColor::Purple, false, 1);
	//FHitResult hitInfo;
	//FCollisionQueryParams params;
	//params.AddIgnoredActor(this);
	//bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);
	//if (bHit)
	//{
	//	AddMovementInput(worldDir, climbSpeed);
	//	auto r1 = UKismetMathLibrary::MakeRotFromX(-hitInfo.Normal);
	//	SetActorRotation(UKismetMathLibrary::RLerp(GetActorRotation(), FRotator(0,r1.Yaw,0), 0.2, false));
	//	DrawDebugPoint(GetWorld(), hitInfo.ImpactPoint, 10, FColor(52, 220, 239), false, 1.0F);

	//}
	//else LedgeMantleCalc(startPos);
	FVector dir = worldDir - GetActorLocation();
	dir.Normalize();
	FVector p0 = GetActorLocation();
	FVector vt = dir * 30 * GetWorld()->GetDeltaSeconds();
	//SetActorLocation(p0 + vt,true);

	FVector DesiredMovementThisFrame = UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), worldDir) * 30 * GetWorld()->GetDeltaSeconds();
	//AddActorLocalOffset(DesiredMovementThisFrame, true);


	FVector Direction = (worldDir - GetActorLocation()).GetSafeNormal();
	float Distance = (worldDir - GetActorLocation()).Size();
	/*if (Distance > 50.0f)*/ CharMov->AddInputVector(Direction);
	//else 
	//{
	//	fsm->FallState();
	//	fsm->climbMode = false;
	//}

}

void ALSH_BaseZom::LedgeMantleCalc(FVector startLoc)
{
	if(!doOnce)
	{
		FVector headPos = startLoc + FVector(0, 0, 45);
		FVector fwdVec = GetActorForwardVector();
		FHitResult hitInfo;
		FCollisionQueryParams params;
		params.AddIgnoredActor(this);
		for (int i = 0; i < 5; i++)
		{
			FVector startPos = fwdVec * i * 20 + headPos;
			FVector endPos = startPos - FVector(0, 0, 100);
			DrawDebugLine(GetWorld(), startPos, endPos, FColor::Red, false, 1);
			bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);
			//만약 더이상 올라갈곳이 없고 모서리라면
			if (bHit)
			{
				//SetActorEnableCollision(false);
				//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				DrawDebugPoint(GetWorld(), hitInfo.ImpactPoint, 10, FColor::Black, false, 7.0F);
				DrawDebugPoint(GetWorld(), hitInfo.Location + FVector(0, 0, 90), 10, FColor::Yellow, false, 7.0F);
				FLatentActionInfo Info;
				Info.CallbackTarget = this;
				Info.ExecutionFunction = TEXT("ClimbUpCompleted"); // 이동 완료 후 실행할 함수 이름
				Info.Linkage = 0; // 대기열에서 이 기능의 위치
				UKismetSystemLibrary::MoveComponentTo(
					GetCapsuleComponent(),
					hitInfo.Location + FVector(0, 0, 90),
					GetActorRotation(),
					false,
					false,
					5.0f ,
					false,
					EMoveComponentAction::Type::Move,
					Info
				);
				//break;
				doOnce = true;
				return;
			}
		}
		FallingAction();
	}
}


void ALSH_BaseZom::ClimbUpCompleted()
{
	UE_LOG(LogTemp, Log, TEXT("끝까지 올라감"));

	//auto CharMov = GetCharacterMovement();

	//날기 상태로 바꾸기
	//CharMov->SetMovementMode(MOVE_Walking);
	//회전 비활성화
	CharMov->bOrientRotationToMovement = true;
	SetActorRotation(FRotator(0, GetActorRotation().Yaw, 0));

	//적을 쫒음
	fsm->ClimbUpEvent();
}


void ALSH_BaseZom::FallingAction()
{
	//떨어져야 한다
	//auto CharMov = GetCharacterMovement();
	UE_LOG(LogTemp, Log, TEXT("올라갈곳이 없음"));

	//떨어지기 상태로 바꾸기
	//CharMov->SetMovementMode(MOVE_Falling);
	//회전 비활성화
	CharMov->bOrientRotationToMovement = true;
	SetActorRotation(FRotator(0, GetActorRotation().Yaw, 0));
	fsm->FallingEvent();
}

void ALSH_BaseZom::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("Actor has been hit!"));
	fsm->ai->StopMovement();
	ClimbMovement(fsm->climbZone->GetActorLocation());
}

void ALSH_BaseZom::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
		// 여기서 원하는 동작을 수행합니다.
		UE_LOG(LogTemp, Warning, TEXT("Collision has ended!"));

		//클라임존인지 확인
		auto a = Cast<ALSH_ClimbZone>(OtherActor);
		if (a != nullptr)
		{

			fsm->FallState();
			fsm->climbMode = false;
		}
}