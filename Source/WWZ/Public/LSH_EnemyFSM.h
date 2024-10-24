// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LSH_EnemyFSM.generated.h"

//사용할 상태 정의
UENUM(BlueprintType)
enum class EEnemyState :uint8
{
	Idle,
	Move,
	Attack,
	Damage,
	Die,
	Climb,
	Fall,
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WWZ_API ULSH_EnemyFSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULSH_EnemyFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM")
	EEnemyState mState = EEnemyState::Idle;

	void IdleState();
	void MoveState();
	void AttackState();
	void DamageState();
	void DieState();
	void ClimbState();
	void FallState();

	UPROPERTY(EditDefaultsOnly, Category = "FSM")
	float idleDelayTime = 2;
	float currentTime = 0;

	UPROPERTY(VisibleAnywhere, Category = "FSM")
	class AYSH_Player* target;

	UPROPERTY()
	class ALSH_BaseZom* me;

	UPROPERTY(EditAnywhere, Category = "FSM")
	float attackRange = 120.0f;

	UPROPERTY(EditAnywhere, Category = "FSM")
	float attackDelayTime = 1.5f;

	UFUNCTION(BlueprintCallable)
	void OnDamageProcess(int32 damage = 1);

	//HP
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FSM")
	int32 hp = 3;
	//피격대기시간
	UPROPERTY(EditAnywhere, Category = "FSM")
	float damageDelayTime = 2.0f;
	//아래로 사라지는 속도
	UPROPERTY(EditAnywhere, Category = "FSM")
	float dieSpeed = 50.0f;
	//사용 중인 애니메이션 블루프린트
	UPROPERTY(BlueprintReadOnly)
	class ULSH_EnemyAnim* anim;

	//Enemy를 소유하고 있는 AIController
	UPROPERTY()
	class AAIController* ai;

	UFUNCTION()
	void ClimbUpEvent();

	UPROPERTY(BlueprintReadOnly)
	bool climbMode=true;

	UPROPERTY(EditAnywhere)
	float climbdist = 800;

	//시작하자 마자 기어오르지 못하도록
	bool isFast = false;

	///좀비가 부딛히기 시작할 시 취할 행동
	UFUNCTION()
	void ClimbAction();

	//좀비가 스폰될때 실행될 활성화 이벤트
	UFUNCTION(BlueprintCallable)
	void ActiveAction(FVector targetLocation);

	//좀비가 대기열에 들어갈 때 실행될 비활성화 이벤트
	UFUNCTION(BlueprintCallable)
	void DeActiveAction();

	//대기열로 돌아가기 위한 좀비매니저 저장
	UPROPERTY()
	class ALSH_ZombieManager* zombieManager;

	FVector climbZoneLocaion;

	UPROPERTY()
	class AController* con;

	UFUNCTION()
	void ShootForce(FVector force);

	    // Sound to play
    UPROPERTY(EditAnywhere, Category = "Audio")
    USoundBase* SoundToPlay1;
	UPROPERTY(EditAnywhere, Category = "Audio")
    USoundBase* SoundToPlay2;
	
    // Sound attenuation settings
    UPROPERTY(EditAnywhere, Category = "Audio")
    USoundAttenuation* SoundAttenuation;
};
