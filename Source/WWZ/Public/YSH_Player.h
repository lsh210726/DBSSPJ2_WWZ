// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "YSH_Player.generated.h"

UCLASS()
class WWZ_API AYSH_Player : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AYSH_Player();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere)
	class USpringArmComponent* springArmComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly )
	class UCameraComponent* cameraComp;

	FVector direction;

	void Move();

	void OnAxisVertical(float value);
	void OnAxisHorizontal(float value);

	void OnAxisTurnYaw(float value);		// Yaw
	void OnAxisLookupPitch(float value);	// Pitch

	void OnActionJump();

	// 총 메시를 추가하고싶다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USkeletalMeshComponent* gunMeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* sniperMeshComp;

	// 전기톱메시를 추가하고싶다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USkeletalMeshComponent* ChainsawMeshComp;

	// 총알공장을 만들고싶다.
	// 마우스 왼쪽 버튼을 누르면 총알공장에서 총알을만들어서 FirePosition소켓에 배치하고싶다.

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AYSH_BulletActor> bulletFactory;

	void OnActionFire();

	void StartReloadGre();
	void ReloadCompleteGre();
	void BeginReloadGre();

	void StartReloadSna();
	void ReloadCompleteSna();
	void BeginReloadSna();

	void OnActionChooseGrenadeGun();
	void OnActionChooseSniperGun();
	void OnActionChooseChainsaw();

public:
	UPROPERTY( EditAnywhere )
	TSubclassOf<class UUserWidget> crossHairFactory;

	UPROPERTY( EditAnywhere )
	TSubclassOf<class UUserWidget> sniperFactory;

	UPROPERTY()
	class UAimUserWidget* crossHairUI;

	UPROPERTY()
	class USniperUserWidget* sniperUI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float targetFOV=90;

	void Zoom();

	FORCEINLINE void OnActionZoomIn();
	FORCEINLINE void OnActionZoomOut();

	/// <summary> true면 SniperGun, false면 GrenadeGun </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bChooseSniperGun = false; //true면 SniperGun, false면 GrenadeGun

	//십자가 키고, 끄기
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCrossOn = false;

	//리로딩 UI 업로드

	UPROPERTY()
	class UReloadUserWidget* reloadUI;

	UPROPERTY( EditAnywhere )
	TSubclassOf<class UUserWidget> reloadFactory;

	//grenadeMagazin
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrentGreMagazin = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int GreMagazin = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int totalGreMagazin = 100;


	//hp, magazine 관련 ui
	UPROPERTY()
	class UPlayerUserWidget* playerUI;

	UPROPERTY( EditAnywhere )
	TSubclassOf<class UUserWidget> playerFactory;

	float ReloadTime = 2.0f;// 재장전에 소요되는 시간
	bool bCanFire = true;    // 현재 총을 발사할 수 있는지 여부
	float AimLoadTime = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrentSnaMagazin = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SnaMagazin = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int totalSnaMagazin = 10;
};
