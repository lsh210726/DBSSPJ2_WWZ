// Fill out your copyright notice in the Description page of Project Settings.


#include "LSH_EnemyFSM.h"
#include "WWZCharacter.h"
#include "LSH_BaseZom.h"
#include <Kismet/GameplayStatics.h>
#include <Components/CapsuleComponent.h>


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
	auto actor = UGameplayStatics::GetActorOfClass(GetWorld(), AWWZCharacter::StaticClass());
	target = Cast<AWWZCharacter>(actor);
	me = Cast<ALSH_BaseZom>(GetOwner());

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
	}
}

void ULSH_EnemyFSM::IdleState()
{
	currentTime += GetWorld()->DeltaTimeSeconds;
	if (currentTime > idleDelayTime)
	{
		mState = EEnemyState::Move;
		currentTime = 0;
	}
}
void ULSH_EnemyFSM::MoveState() 
{
	FVector destination = target->GetActorLocation();
	FVector dir = destination - me->GetActorLocation();
	me->AddMovementInput(dir.GetSafeNormal());

	//공격 가능 거리에 플레이어가 있다면
	if (dir.Size() < attackRange)
	{
		//공격 상태로 전환
		mState = EEnemyState::Attack;
	}
}
void ULSH_EnemyFSM::AttackState()
{
	//일정 시간마다 공격
	currentTime += GetWorld()->DeltaTimeSeconds;
	//공격시간이 되면
	if (currentTime > attackDelayTime)
	{
		UE_LOG(LogTemp, Log, TEXT("Attack!!!"));
		currentTime = 0;
	}

	float distance = FVector::Distance(target->GetActorLocation(), me->GetActorLocation());
	if (distance > attackRange)
	{
		mState = EEnemyState::Idle;
	}
}
void ULSH_EnemyFSM::DamageState()
{
	currentTime += GetWorld()->DeltaTimeSeconds;
	if (currentTime > damageDelayTime)
	{
		mState = EEnemyState::Idle;
		currentTime = 0;
	}
}
void ULSH_EnemyFSM::DieState()
{
	me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
	mState = hp > 0 ? EEnemyState::Damage : EEnemyState::Die;
}
