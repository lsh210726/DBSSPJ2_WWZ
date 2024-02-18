// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "YSH_Player.generated.h"

UENUM(BlueprintType)
enum class EWeapon : uint8
{
	GrenadeGun,
	SniperGun,
	RifleGun,
	Chainsaw
};

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

	// �� �޽ø� �߰��ϰ�ʹ�.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USkeletalMeshComponent* gunMeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* sniperMeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* rifleMeshComp;

	// ������޽ø� �߰��ϰ�ʹ�.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USkeletalMeshComponent* ChainsawMeshComp;

	// �Ѿ˰����� �����ʹ�.
	// ���콺 ���� ��ư�� ������ �Ѿ˰��忡�� �Ѿ������� FirePosition���Ͽ� ��ġ�ϰ�ʹ�.

	//UPROPERTY(EditAnywhere)
	//TSubclassOf<class AYSH_BulletActor> bulletFactory;
			
	//LSH 2.19
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ALSH_Granade> bulletFactory;


	void OnActionFire();

	void OnActionChooseGrenadeGun();
	void OnActionChooseSniperGun();
	void OnActionChooseChainsaw();
	void OnActionChooseRifleGun();

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

	void OnActionZoomIn();
	void OnActionZoomOut();

	void OnActionPickUp();
	void EnablePickUpAction();
	bool bCanPickUp = true;

	/// <summary> true�� SniperGun, false�� GrenadeGun </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bChooseSniperGun = false; //true�� SniperGun, false�� GrenadeGun

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bChooseChainSaw = false;

	//���ڰ� Ű��, ���
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCrossOn = false;

	//���ε� UI ���ε�

	UPROPERTY()
	class UReloadUserWidget* reloadUI;

	UPROPERTY( EditAnywhere )
	TSubclassOf<class UUserWidget> reloadFactory;

	//grenadeMagazin
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrentGreMagazin = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int GreMagazin = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int totalGreMagazin = 0;


	//hp, magazine ���� ui
	UPROPERTY()
	class UPlayerUserWidget* playerUI;

	UPROPERTY( EditAnywhere )
	TSubclassOf<class UUserWidget> playerFactory;

	float ReloadTime = 2.0f;// �������� �ҿ�Ǵ� �ð�
	bool bCanFire = true;    // ���� ���� �߻��� �� �ִ��� ����
	float AimLoadTime = 0.01f;

	//magazin ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrentSnaMagazin = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SnaMagazin = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int totalSnaMagazin = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrentRifleMagazin = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int RifleMagazin = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int totalRifleMagazin = 210;

	UPROPERTY(EditAnywhere)
	class UAnimMontage* fireMontage;

	UPROPERTY(EditAnywhere)
	class USoundBase* fireSFX;

	public:
	//���� ����Ʈ
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EWeapon CurrentWeaponType;
		UPROPERTY(EditDefaultsOnly, Category = CameraMotion)
		TSubclassOf<class UCameraShakeBase> cameraShake;

	// Rifle �� ����
	void OnMyActionRiflePressed();
	void OnMyActionRifleFireReleased();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAutoFire;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float currentTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float fireTime = 0.2f;

	UFUNCTION(BlueprintCallable)
	void MakeRifleBullet();

	//������ ����
	void StartReload();

	void ReloadWeapon(int& CurrentMagazin, int MaxMagazin, int& TotalMagazin);

	void ReloadComplete();

	void ReloadCompleteWeapon(int& CurrentMagazin, int& TotalMagazin);
};
