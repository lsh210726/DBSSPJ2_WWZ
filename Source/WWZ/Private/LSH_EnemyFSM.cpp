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

	//클라임 존 찾기
	climbZone = Cast<ALSH_ClimbZone>(UGameplayStatics::GetActorOfClass(GetWorld(), ALSH_ClimbZone::StaticClass()));

	me = Cast<ALSH_BaseZom>(GetOwner());

	//EnemyAnim 변수에 넣기
	anim = Cast<ULSH_EnemyAnim>(me->GetMesh()->GetAnimInstance());

	//AAIController 할당하기
	ai = Cast<AAIController>(me->GetController());

	//me->GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ULSH_EnemyFSM::ClimbZoneOverlap);
	//me->GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &ULSH_EnemyFSM::ClimbZoneEndOverlap);
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
		itHasPath();


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
	//FVector destination = targetLoc;
	FVector dir = destination - me->GetActorLocation();
	//이동
	//me->AddMovementInput(dir.GetSafeNormal());
	ai->MoveToLocation(destination);

	DrawDebugPoint(GetWorld(), destination, 20, FColor::Purple, false, GetWorld()->GetDeltaSeconds());

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

	auto speed = me->GetVelocity().Size();

	if (speed > 100) isMoving = true;

	//만약 이동속도가 100 이하라면 > 벽에 막힌다면
	if (isMoving && me->GetVelocity().Size() < 100)
	{
		currentTime += GetWorld()->DeltaTimeSeconds;
		if (currentTime > 0.3)
		{
			//주변에 오르기 가능한 위치를 찾는다
			FindClimbPoint();
			currentTime = 0;
			isMoving = false;
		}
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
	// 
	// 
	me->ClimbMovement(me->GetActorUpVector());


}


void ULSH_EnemyFSM::ClimbZoneOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//클라임존인지 확인
	auto a = Cast<ALSH_ClimbZone>(OtherActor);
	if (a != nullptr)
	{

			//이동모드 -> 오르기
		if(me->ClimbAction())
		{
			//오르기 상태로 전환
			mState = EEnemyState::Climb;
			//애니메이션 상태 동기화
			anim->animState = mState;
		}
		
	}
	UE_LOG(LogTemp, Log, TEXT("Im Climb!!"));
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

		if(target->GetActorLocation().Z -me->GetActorLocation().Z<20) climbMode = false;

		me->StopClimb();

		UE_LOG(LogTemp, Log, TEXT("No Climb!!"));
	}
}


void ULSH_EnemyFSM::ClimbUpEvent()
{
	//기어올라가기 애니메이션 몽타지 재생
	//anim->PlayClimbUpAnim();


	//오르기 상태로 전환
	mState = EEnemyState::Move;
	//애니메이션 상태 동기화
	anim->animState = mState;

	climbMode = false;
}


void ULSH_EnemyFSM::itHasPath()
{
	//UNavigationSystemV1* const NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this);
	//FSharedConstNavQueryFilter QueryFilter = UNavigationQueryFilter::GetQueryFilter(*NavSystem, nullptr, NavigationFilterClass);
	//FPathFindingQuery Query(AIController, *NavSystem->GetMainNavData(), AIController->GetPawn()->GetActorLocation(), TargetLocation, QueryFilter);

	//FPathFindingResult Result = NavSystem->TestPathSync(Query);

	//if (Result.IsSuccessful())
	//{
	//	// 이동 요청
	//	EPathFollowingRequestResult::Type MoveResult = AIController->MoveToLocation(TargetLocation);
	//	// 로그 출력 등
	//}
	//else
	//{
	//	// 목적지에 도달할 수 없음
	//	UE_LOG(LogTemp, Warning, TEXT("목적지에 도달할 수 없습니다."));
	//}

	return;

}

void ULSH_EnemyFSM::FindClimbPoint()
{
	for(int i=0;i<=360;i+=30)
	{
		UE_LOG(LogTemp, Log, TEXT("%f"), me->climbDistance);

		FVector startPos = me->GetActorLocation();
		FVector endPos = startPos + (UKismetMathLibrary::GetForwardVector(me->GetActorRotation()+FRotator(0,i,0)) * me->climbDistance);
		DrawDebugLine(GetWorld(), startPos, endPos, FColor::Emerald, false, 1);
		FHitResult hitInfo;
		FCollisionQueryParams params;
		params.AddIgnoredActor(me);
		bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);
		if (bHit)//캐스트로 힛액터가 좀비나 클라임존인지 확인 필요...
		{
			DrawDebugPoint(GetWorld(), hitInfo.ImpactPoint, 5, FColor::Silver, false, 2);

			//ai->MoveToLocation(hitInfo.ImpactPoint);


			auto f1 = hitInfo.Normal * me->GetCapsuleComponent()->GetScaledCapsuleRadius();
			auto f2 = f1 + hitInfo.Location;

			// 벽에 찰싹 달라붙게
			auto r1 = UKismetMathLibrary::MakeRotFromX(-hitInfo.Normal);
			//auto r1 = me->GetActorRotation() + FRotator(0, i, 0)*-1;
			FLatentActionInfo Info;
			Info.CallbackTarget = this;
			Info.ExecutionFunction = TEXT("OnMoveCompleted"); // 이동 완료 후 실행할 함수 이름
			Info.Linkage = 0; // 대기열에서 이 기능의 위치
			//오르기 상태로 전환
			mState = EEnemyState::Climb;
			UKismetSystemLibrary::MoveComponentTo(
				me->GetCapsuleComponent(),
				f2,
				r1,
				false,
				false,
				0.2f,
				false,
				EMoveComponentAction::Type::Move,
				Info
			);
			break;
			//위치로 도달했다면 올라가기 액션 수행할 것...
		}

	}
}

void ULSH_EnemyFSM::OnMoveCompleted()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("찰싹!"));


	//애니메이션 상태 동기화
	anim->animState = mState;
	me->ClimbAction();
}