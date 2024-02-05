// Fill out your copyright notice in the Description page of Project Settings.


#include "LSH_EnemyFSM.h"
#include "WWZCharacter.h"
#include "LSH_BaseZom.h"
#include <Kismet/GameplayStatics.h>
#include <Components/CapsuleComponent.h>
#include "LSH_EnemyAnim.h"
#include "AIController.h"
#include "LSH_ClimbZone.h"


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
	//auto actor = UGameplayStatics::GetActorOfClass(GetWorld(), AWWZCharacter::StaticClass());
	//target = Cast<AWWZCharacter>(actor);

	//클라임 존 찾기
	auto actor = UGameplayStatics::GetActorOfClass(GetWorld(), ALSH_ClimbZone::StaticClass());
	target = Cast<ALSH_ClimbZone>(actor);

	me = Cast<ALSH_BaseZom>(GetOwner());

	//EnemyAnim 변수에 넣기
	anim = Cast<ULSH_EnemyAnim>(me->GetMesh()->GetAnimInstance());

	//AAIController 할당하기
	ai = Cast<AAIController>(me->GetController());

	me->GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ULSH_EnemyFSM::ClimbZoneOverlap);
	me->GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &ULSH_EnemyFSM::ClimbZoneEndOverlap);
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
	}
}

void ULSH_EnemyFSM::IdleState()
{
	//FVector p0 = me->GetActorLocation();
	//FVector vt = me->GetActorUpVector() * 800 * GetWorld()->DeltaTimeSeconds;
	//me->SetActorLocation(p0 + vt);
	//return;

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
	FVector destination = target->GetActorLocation();
	FVector dir = destination - me->GetActorLocation();
	//이동
	//me->AddMovementInput(dir.GetSafeNormal());
	ai->MoveToLocation(destination);

	//공격 가능 거리에 플레이어가 있다면
	if (dir.Size() < attackRange)
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

	if (!isInMaxSpeed && me->GetVelocity().Size() > 500)
	{
		isInMaxSpeed = true;
	}
	//만약 이동속도가 100 이하라면 > 벽에 막힌다면
	if (isInMaxSpeed && me->GetVelocity().Size() < 100)
	{
		//멈추기 상태로 전환
		mState = EEnemyState::Idle;
		//애니메이션 상태 동기화
		anim->animState = mState;
	}
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
	//시간 흐르기
	currentTime += GetWorld()->DeltaTimeSeconds;
	//경과시간이 대기시간을 지나면
	if (currentTime > damageDelayTime)
	{
		//대기상태로 전환
		mState = EEnemyState::Idle;
		//경과시간 초기화
		currentTime = 0;
		//애니메이션 상태 동기화
		anim->animState = mState;
	}
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
	//mState = hp > 0 ? EEnemyState::Damage : EEnemyState::Die;
	if (hp > 0)
	{
		//상태를 피격으로 전환
		mState = EEnemyState::Damage;

		currentTime = 0;
		//피격 애니메이션 재생
		int32 index = FMath::RandRange(0, 1);
		FString sectionName = FString::Printf(TEXT("Damage%d"), index);
		anim->PlayDamageAnim(FName(*sectionName));
	}
	else
	{

		mState = EEnemyState::Die;
		//충돌 없애기
		me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//죽음 애니메이션 재생
		anim->PlayDamageAnim(TEXT("Die"));
	}


	//애니메이션 상태 동기화
	anim->animState = mState;
}

void ULSH_EnemyFSM::ClimbState()
{
	//me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//FVector P0 = me->GetActorLocation();
	//FVector vt = FVector::UpVector * dieSpeed * GetWorld()->DeltaTimeSeconds;
	//FVector P = P0 + vt;
	//me->SetActorLocation(P);
	me->ClimbMovement(me->GetActorUpVector());
}


void ULSH_EnemyFSM::ClimbZoneOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//클라임존인지 확인
	auto a = Cast<ALSH_ClimbZone>(OtherActor);
	if (a != nullptr)
	{
		//이동모드 -> 오르기
		me->ClimbAction();
		//오르기 상태로 전환
		mState = EEnemyState::Climb;
		//애니메이션 상태 동기화
		anim->animState = mState;
	}
	//UE_LOG(LogTemp, Log, TEXT("Im Climb!!"));
}

void ULSH_EnemyFSM::ClimbZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//클라임존인지 확인
	auto a = Cast<ALSH_ClimbZone>(OtherActor);
	if (a != nullptr)
	{


		//오르기 상태로 전환
		mState = EEnemyState::Move;
		//애니메이션 상태 동기화
		anim->animState = mState;

		UE_LOG(LogTemp, Log, TEXT("No Climb!!"));
	}
	UE_LOG(LogTemp, Log, TEXT("No Climb!!"));
}


void ULSH_EnemyFSM::ClimbUpEvent()
{
	//기어올라가기 애니메이션 몽타지 재생
	anim->PlayClimbUpAnim();
}