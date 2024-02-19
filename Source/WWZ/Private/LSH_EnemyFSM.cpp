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
#include "LSH_ZombieManager.h"




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


	me = Cast<ALSH_BaseZom>(GetOwner());

	//EnemyAnim 변수에 넣기
	anim = Cast<ULSH_EnemyAnim>(me->GetMesh()->GetAnimInstance());

	//AAIController 할당하기
	ai = Cast<AAIController>(me->GetController());

	con = me->GetController();
	//con->UnPossess();
	//con->Possess(me);
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


}
void ULSH_EnemyFSM::MoveState() 
{
	FVector destination =  climbMode ? climbZoneLocaion : target->GetActorLocation();

	FVector dir = destination - me->GetActorLocation();
	//이동
	//me->AddMovementInput(dir.GetSafeNormal());
	ai->MoveToLocation(destination);

	//DrawDebugPoint(GetWorld(), destination, 20, FColor::Purple, false, GetWorld()->GetDeltaSeconds());
	//DrawDebugLine(GetWorld(), me->GetActorLocation(), destination, FColor::White, false, GetWorld()->GetDeltaSeconds());

	anim->speed = me->GetVelocity().Size();

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

}

void ULSH_EnemyFSM::AttackState()
{
	currentTime += GetWorld()->DeltaTimeSeconds;
	//타깃과의 거리
	float distance = FVector::Distance(target->GetActorLocation(), me->GetActorLocation());
	//거리가 공격범위를 벗어나면
	if (distance > attackRange&& currentTime > attackDelayTime)
	{

		//이동
		mState = EEnemyState::Move;
		//애니메이션 상태 동기화
		anim->animState = mState;
		currentTime = 0;

	}
	//일정 시간마다 공격
	//공격시간이 되면
	else if (currentTime > attackDelayTime)
	{
		ai->StopMovement();
		//공격
		UE_LOG(LogTemp, Log, TEXT("Attack!!!"));
		//시간 초기화
		currentTime = 0;
		anim->bAttackPlay = true;
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

void ULSH_EnemyFSM::OnDamageProcess(int32 damage)
{
	hp-=damage;

	if(zombieManager==nullptr)zombieManager = Cast<ALSH_ZombieManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ALSH_ZombieManager::StaticClass()));//GetActorsOfClass(GetWorld(), ALSH_ClimbZone::StaticClass(), climbZoneArray);

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

		//만약 데미지가 들어왔는데 대기상태라면
		if (mState == EEnemyState::Idle)
		{
			climbMode = false;
			//플레이어 추적
			mState = EEnemyState::Move;
			//애니메이션 상태 동기화
			anim->animState = mState;


		}
	}
	else
	{

		//mState = EEnemyState::Die;
		////충돌 없애기
		//me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		////죽음 애니메이션 재생
		//anim->PlayDamageAnim(TEXT("Die"));

		if (SoundToPlay1 != nullptr && SoundToPlay2 != nullptr && SoundAttenuation != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(me, FMath::RandBool()? SoundToPlay1 : SoundToPlay2, me->GetActorLocation(), 0.3, FMath::RandRange(0.8f,1.2f),0, SoundAttenuation);
		}

		mState = EEnemyState::Idle;

		//애니메이션 상태 동기화
		anim->animState = mState;
		me->GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
		me->GetMesh()->SetSimulatePhysics(true);
		me->CharMov->DisableMovement();
		//캡술컨포넌트 충돌 없애기
		me->GetCapsuleComponent()->SetCollisionProfileName(TEXT("NoCollision"));
		me->CharMov->bUseRVOAvoidance = false;
		con->UnPossess();


		zombieManager->bodyCollecting(me);
	}


	//애니메이션 상태 동기화
	anim->animState = mState;
}

void ULSH_EnemyFSM::ClimbState()
{

	me->ClimbMovement(climbZoneLocaion);

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
	

	//climbMode = height>10;
	climbMode = false;
}

void ULSH_EnemyFSM::ClimbAction()
{
	ai->StopMovement();

	//오르기 상태로 전환
	mState = EEnemyState::Climb;

	//애니메이션 상태 동기화
	anim->animState = mState;
}

void ULSH_EnemyFSM::ActiveAction(FVector targetLocation)
{
	climbZoneLocaion = targetLocation;
	hp = 3;
	con->Possess(me);

	me->GetCapsuleComponent()->SetCollisionProfileName(TEXT("zombie"));

	//오르기 상태로 전환
	mState = EEnemyState::Move;

	//애니메이션 상태 동기화
	anim->animState = mState;
	me->CharMov->bUseRVOAvoidance = true;

	me->CharMov->SetMovementMode(MOVE_Walking);

	climbMode = true;
	anim->bClimbOver = false;
	me->bIsColliding = false;
	me->CharMov->MaxFlySpeed = 100;
	
}

void ULSH_EnemyFSM::DeActiveAction()
{
	auto mesh = me->GetMesh();
	mesh->SetSimulatePhysics(false);
	mesh->SetCollisionProfileName(TEXT("NoCollision"));
	mesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	mesh->SetRelativeLocationAndRotation(FVector(0, 0, -88), FRotator(0, -90, 0));
	me->SetActorLocation(zombieManager->queueLocation->GetComponentLocation());
	//fsm, 애니메이션 초기화 필요
		//오르기 상태로 전환
	mState = EEnemyState::Idle;

	//애니메이션 상태 동기화
	anim->animState = mState;

	me->CharMov->SetMovementMode(MOVE_None);
	
}

void ULSH_EnemyFSM::ShootForce(FVector force)
{
	if (hp <= 0)
	{
		me->GetMesh()->AddImpulse(force);
	}
}