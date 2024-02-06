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


	// �� �޽ø� �߰��ϰ�ʹ�.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USkeletalMeshComponent* gunMeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* sniperMeshComp;

	// ������޽ø� �߰��ϰ�ʹ�.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USkeletalMeshComponent* ChainsawMeshComp;

	// �Ѿ˰����� �����ʹ�.
	// ���콺 ���� ��ư�� ������ �Ѿ˰��忡�� �Ѿ������� FirePosition���Ͽ� ��ġ�ϰ�ʹ�.

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AYSH_BulletActor> bulletFactory;

	void OnActionFire();

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
	class UUserWidget* sniperUI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float targetFOV=90;

	void Zoom();

	FORCEINLINE void OnActionZoomIn();
	FORCEINLINE void OnActionZoomOut();

	/// <summary> true�� SniperGun, false�� GrenadeGun </summary>
	bool bChooseSniperGun = false; //true�� SniperGun, false�� GrenadeGun

	//���ڰ� Ű��, ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCrossOn = false;

	//���ε� UI ���ε�

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

	void setGreMagazin();
	void ReloadComplete();


	//hp, magazine ���� ui
	UPROPERTY()
	class UUserWidget* playerUI;

	UPROPERTY( EditAnywhere )
	TSubclassOf<class UUserWidget> playerFactory;

};
