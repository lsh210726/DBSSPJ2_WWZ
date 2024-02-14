// Fill out your copyright notice in the Description page of Project Settings.


#include "LSH_EnemyFSM.h"
#include "WWZCharacter.h"
#include "LSH_BaseZom.h"
#include <Kismet/GameplayStatics.h>
#include <Components/CapsuleComponent.h>
#include "LSH_EnemyAnim.h"
#include "AIController.h"
#include "LSH_ClimbZone.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "Kismet/KismetMathLibrary.h"
#include "YSH_Player.h"
#include "GameFramework/CharacterMovementComponent.h"




// Sets default values for this component's properties
ULSH_EnemyFSM::ULSH_EnemyFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void ULSH_EnemyFSM::BeginPlay()
{
	Super::BeginPlay();

	//월드에서 플레이어 타깃 찾기
	auto actor = UGameplayStatics::GetActorOfClass(GetWorld(), AYSH_Player::StaticClass());
	target = Cast<AYSH_Player>(actor);

	//클라임 존 찾기
	climbZone = Cast<ALSH_ClimbZone>(UGameplayStatics::GetActorOfClass(GetWorld(), ALSH_ClimbZone::StaticClass()));

	me = Cast<ALSH_BaseZom>(GetOwner());

	//EnemyAnim 변수에 넣기
	anim = Cast<ULSH_EnemyAnim>(me->GetMesh()->GetAnimInstance());

	//AAIController 할당하기
	ai = Cast<AAIController>(me->GetController());

}


// Called every frame
void ULSH_EnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	switch (mState)
	{
	case EEnemyState::Idle:
		IdleState();
		break;
	case EEnemyState::Move:
		MoveState();
		break;
	case EEnemyState::Attack:
		AttackState();
		break;
	case EEnemyState::Damage:
		DamageState();
		break;
	case EEnemyState::Die:
		DieState();
		break;
	case EEnemyState::Climb:
		ClimbState();
		break;	
	case EEnemyState::Fall:
		FallState();
		break;
	}
}

void ULSH_EnemyFSM::IdleState()
{

	//시간이 흐름
	currentTime += GetWorld()->DeltaTimeSeconds;
	//경과시간이 대기시간을 넘김
	if (currentTime > idleDelayTime)
	{
		//이동상태로 전환
		mState = EEnemyState::Move;
		//경과시간 초기화
		currentTime = 0;

		//애니메이션 상태 동기화
		anim->animState = mState;
	}
}
void ULSH_EnemyFSM::MoveState() 
{
	FVector destination =  climbMode ? climbZone->GetActorLocation() : target->GetActorLocation();

	FVector dir = destination - me->GetActorLocation();
	//이동
	//me->AddMovementInput(dir.GetSafeNormal());
	ai->MoveToLocation(destination);

	//DrawDebugPoint(GetWorld(), destination, 20, FColor::Purple, false, GetWorld()->GetDeltaSeconds());
	//DrawDebugLine(GetWorld(), me->GetActorLocation(), destination, FColor::White, false, GetWorld()->GetDeltaSeconds());

	//오르기 모드가 아니고 공격 가능 거리에 플레이어가 있다면
	if (!climbMode && dir.Size() < attackRange)
	{
		//공격 상태로 전환
		mState = EEnemyState::Attack;
		//애니메이션 상태 동기화
		anim->animState = mState;
		//공격 애니메이션 재생 활성화
		anim->bAttackPlay = true;
		//공격 상태 전환 시 대기 시간이 바로 끝나도록 처리
		currentTime = attackDelayTime;
	} 

	//auto speed = me->GetVelocity().Size();

	//if (me->GetVelocity().Size() > 100)isFast = true;

	////만약 오르기 모드고 목적지와 거리가 100 이하고 속도가 100 아래라면
	//if (climbMode && isFast && me->GetVelocity().Size() < 100)
	//{

	//		//오르기 상태로 전환
	//		mState = EEnemyState::Climb;

	//		//애니메이션 상태 동기화
	//		anim->animState = mState;

	//		currentTime = 0;

	//		isFast = false;

	//}
}

void ULSH_EnemyFSM::AttackState()
{
	//일정 시간마다 공격
	currentTime += GetWorld()->DeltaTimeSeconds;
	//공격시간이 되면
	if (currentTime > attackDelayTime)
	{
		//공격
		UE_LOG(LogTemp, Log, TEXT("Attack!!!"));
		//시간 초기화
		currentTime = 0;
		anim->bAttackPlay = true;
	}
	//타깃과의 거리
	float distance = FVector::Distance(target->GetActorLocation(), me->GetActorLocation());
	//거리가 공격범위를 벗어나면
	if (distance > attackRange)
	{
		//이동
		mState = EEnemyState::Idle;
		//애니메이션 상태 동기화
		anim->animState = mState;
	}
}
void ULSH_EnemyFSM::DamageState()
{
	////시간 흐르기
	//currentTime += GetWorld()->DeltaTimeSeconds;
	////경과시간이 대기시간을 지나면
	//if (currentTime > damageDelayTime)
	//{
	//	//대기상태로 전환
	//	mState = EEnemyState::Idle;
	//	//경과시간 초기화
	//	currentTime = 0;
	//	//애니메이션 상태 동기화
	//	anim->animState = mState;
	//}
}
void ULSH_EnemyFSM::DieState()
{
	//아직 죽음 애니메이션이 끝나지 않았다면 바닥 내려가지 않도록
	if (anim->bDieDone == false)
	{
		return;
	}
	//계속 아래로 내려가고 싶다
	FVector P0 = me->GetActorLocation();
	FVector vt = FVector::DownVector * dieSpeed * GetWorld()->DeltaTimeSeconds;
	FVector P = P0 + vt;
	me->SetActorLocation(P);
	if (P.Z < -200.0f)
	{
		me->Destroy();
	}
	UE_LOG(LogTemp, Log, TEXT("Die"));
}

void ULSH_EnemyFSM::OnDamageProcess()
{
	hp--;
	//체력이 0보다 크면 데미지 상태, 아니면 죽음
	if (hp > 0)
	{
		////상태를 피격으로 전환
		//mState = EEnemyState::Damage;

		//currentTime = 0;
		////피격 애니메이션 재생
		//int32 index = FMath::RandRange(0, 1);
		//FString sectionName = FString::Printf(TEXT("Damage%d"), index);
		//anim->PlayDamageAnim(FName(*sectionName));
	}
	else
	{

		//mState = EEnemyState::Die;
		////충돌 없애기
		//me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		////죽음 애니메이션 재생
		//anim->PlayDamageAnim(TEXT("Die"));
		me->GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
		me->GetMesh()->SetSimulatePhysics(true);
		me->CharMov->DisableMovement();
	}


	//애니메이션 상태 동기화
	anim->animState = mState;
}

void ULSH_EnemyFSM::ClimbState()
{

	me->ClimbMovement(climbZone->GetActorLocation());

}


void ULSH_EnemyFSM::FallState()
{
	mState = EEnemyState::Move;

	//애니메이션 상태 동기화
	anim->animState = mState;
}


void ULSH_EnemyFSM::ClimbUpEvent()
{
	//기어올라가기 애니메이션 몽타지 재생
	//anim->PlayClimbUpAnim();

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("끝까지 올라감!"));


	//달리기 상태로 전환
	mState = EEnemyState::Move;
	//애니메이션 상태 동기화
	anim->animState = mState;

	//만약 플레이어가 나보다 높으면
	double height = target->GetActorLocation().Z - me->GetActorLocation().Z;
	

	climbMode = height>10;
}

void ULSH_EnemyFSM::ClimbAction()
{
	ai->StopMovement();

	//오르기 상태로 전환
	mState = EEnemyState::Climb;

	//애니메이션 상태 동기화
	anim->animState = mState;
}