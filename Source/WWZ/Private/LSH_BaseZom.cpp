// Fill out your copyright notice in the Description page of Project Settings.


#include "LSH_BaseZom.h"
#include "LSH_EnemyFSM.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Components/CapsuleComponent.h>
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"




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

}

// Called when the game starts or when spawned
void ALSH_BaseZom::BeginPlay()
{
	Super::BeginPlay();
	
	//GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ALSH_BaseZom::ClimbZoneOverlap);
	//GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic()

}

// Called every frame
void ALSH_BaseZom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

// Called to bind functionality to input
void ALSH_BaseZom::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}



bool ALSH_BaseZom::ClimbAction()
{
	auto CharMov = GetCharacterMovement();
	////만약 현재 이동모드가 플라잉이 아니라면 기어오르기모드(플라잉)
	//if(CharMov->MovementMode != EMovementMode::MOVE_Flying)
	//{
	//	FVector startPos = GetActorLocation()+FVector(0,0,100);
	//	FVector endPos = startPos + GetActorForwardVector() * climbDistance;
	//	DrawDebugLine(GetWorld(), startPos, endPos, FColor::Emerald, false, 1);
	//	FHitResult hitInfo;
	//	FCollisionQueryParams params;
	//	params.AddIgnoredActor(this);
	//	bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);
	//	if (bHit)
	//	{
	//		//날기 상태로 바꾸기
	//		CharMov->SetMovementMode(MOVE_Flying);
	//		//회전 비활성화
	//		CharMov->bOrientRotationToMovement = false;

	//		auto f1 = hitInfo.Normal * GetCapsuleComponent()->GetScaledCapsuleRadius();
	//		auto f2 = f1 + hitInfo.Location;
	//		//GetCapsuleComponent()

	//		//벽에 찰싹 달라붙게
	//		auto r1 = UKismetMathLibrary::MakeRotFromX(hitInfo.Normal * -1.0f);
	//		FLatentActionInfo Info;
	//		Info.CallbackTarget = this;
	//		UKismetSystemLibrary::MoveComponentTo(
	//			GetCapsuleComponent(),
	//			f2,
	//			r1,
	//			false,
	//			false,
	//			0.2f,
	//			false,
	//			EMoveComponentAction::Type::Move,
	//			Info
	//		);
	//		doOnce = false;
	//		return true;
	//	}
	//}
	CharMov->SetMovementMode(MOVE_Flying);
	CharMov->bOrientRotationToMovement = false;
	return false;
}

void ALSH_BaseZom::ClimbMovement(FVector worldDir)
{
	FVector startPos = GetActorLocation() + FVector(0, 0, 100);
	FVector endPos = startPos + GetActorForwardVector() * climbDistance;
	DrawDebugLine(GetWorld(), startPos, endPos, FColor::Emerald, false, 1);
	FHitResult hitInfo;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);
	if (bHit)
	{
		AddMovementInput(worldDir, climbSpeed);
		auto r1 = UKismetMathLibrary::MakeRotFromX(-hitInfo.Normal);
		SetActorRotation(UKismetMathLibrary::RLerp(GetActorRotation(), r1, 0.2, false));
		DrawDebugPoint(GetWorld(), hitInfo.ImpactPoint, 10, FColor(52, 220, 239), false, 1.0F);

	}
	else LedgeMantleCalc(startPos);
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
					5.0f,
					false,
					EMoveComponentAction::Type::Move,
					Info
				);
				//fsm->ClimbUpEvent();
				doOnce = true;
				break;
			}
		}
	}
}

void ALSH_BaseZom::StopClimb()
{
	auto CharMov = GetCharacterMovement();

	//날기 상태로 바꾸기
	CharMov->SetMovementMode(MOVE_Walking);
	//회전 비활성화
	CharMov->bOrientRotationToMovement = true;
	SetActorRotation(FRotator(0, GetActorRotation().Yaw, 0));
}

void ALSH_BaseZom::ClimbUpCompleted()
{
	StopClimb();
	fsm->ClimbUpEvent();
}