// Fill out your copyright notice in the Description page of Project Settings.


#include "YSH_Player.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/SkeletalMesh.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "YSH_BulletActor.h"
#include "Engine/World.h"
#include "Blueprint/UserWidget.h"
#include "Animation/WidgetAnimation.h"
#include "ReloadUserWidget.h"
#include "AimUserWidget.h"
#include "SniperUserWidget.h"
#include "PlayerUserWidget.h"
#include "YSH_PlayerAnim.h"
#include "LSH_EnemyFSM.h"
#include "AmmoActor.h"


// Sets default values
AYSH_Player::AYSH_Player()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("springArmComp"));
	springArmComp->SetupAttachment(RootComponent);

	springArmComp->SetWorldLocation(FVector(0, 70, 90));

	cameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("cameraComp"));
	cameraComp->SetupAttachment(springArmComp);

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Quinn.SKM_Quinn'"));

	if (tempMesh.Succeeded())
	{

		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}

	bUseControllerRotationYaw = true;

	springArmComp->bUsePawnControlRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;



	gunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("gunMeshComp"));
	gunMeshComp->SetupAttachment(GetMesh());

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempGunMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/YSH/Models/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));

	if (tempGunMesh.Succeeded())
	{
		gunMeshComp->SetSkeletalMesh(tempGunMesh.Object);
		gunMeshComp->SetRelativeLocation(FVector(0, 50, 130));
	}


	sniperMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("sniperMeshComp"));
	sniperMeshComp->SetupAttachment(GetMesh());

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempSniper(TEXT("/Script/Engine.StaticMesh'/Game/YSH/Models/SniperGun/sniper1.sniper1'"));

	if (tempSniper.Succeeded())
	{
		sniperMeshComp->SetStaticMesh(tempSniper.Object);
		sniperMeshComp->SetRelativeLocation(FVector(0, 80, 130));
		sniperMeshComp->SetWorldScale3D(FVector(0.15f));
	}

	ChainsawMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ChainsawMeshComp"));
	ChainsawMeshComp->SetupAttachment(GetMesh());

	ConstructorHelpers::FObjectFinder<USkeletalMesh> ChainsawMeshFinder(TEXT("/Script/Engine.SkeletalMesh'/Game/YSH/Models/Chainsaw/source/model.model_Core'"));

	if (ChainsawMeshFinder.Succeeded())
	{
		ChainsawMeshComp->SetSkeletalMesh(ChainsawMeshFinder.Object);
		ChainsawMeshComp->SetRelativeLocationAndRotation(FVector(0, 50, 130), FRotator(0, 0, 90));
		ChainsawMeshComp->SetRelativeScale3D(FVector(0.01));
	}


	gunMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	sniperMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

// Called when the game starts or when spawned
void AYSH_Player::BeginPlay()
{
	Super::BeginPlay();

	auto crossHairWidget = CreateWidget(GetWorld(), crossHairFactory);
	crossHairUI = Cast<UAimUserWidget>(crossHairWidget);
	crossHairUI->AddToViewport();
	crossHairUI->WhiteAimInvisible();
	crossHairUI->RedAimInvisible();

	auto sniperWidget = CreateWidget(GetWorld(), sniperFactory);
	sniperUI = Cast<USniperUserWidget>(sniperWidget);
	sniperUI->AddToViewport();

	auto reloadWidget = CreateWidget(GetWorld(), reloadFactory);
	reloadUI = Cast<UReloadUserWidget>(reloadWidget);
	reloadUI->AddToViewport();

	auto playerWidget = CreateWidget(GetWorld(), playerFactory);
	playerUI = Cast<UPlayerUserWidget>(playerWidget);
	playerUI->AddToViewport();

	sniperUI->SetVisibility(ESlateVisibility::Hidden);
	reloadUI->SetVisibility(ESlateVisibility::Hidden);

	OnActionChooseGrenadeGun();

	GreMagazin = 10;
	CurrentGreMagazin = 10;
	totalGreMagazin = 100;

	SnaMagazin = 5;
	CurrentSnaMagazin = 5;
	totalSnaMagazin = 20;

}

// Called every frame
void AYSH_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Move();

	Zoom();
}

// Called to bind functionality to input

void AYSH_Player::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("Move Forward / Backward"), this, &AYSH_Player::OnAxisVertical);

	PlayerInputComponent->BindAxis(TEXT("Move Right / Left"), this, &AYSH_Player::OnAxisHorizontal);

	PlayerInputComponent->BindAxis(TEXT("Turn Right / Left Mouse"), this, &AYSH_Player::OnAxisTurnYaw);

	PlayerInputComponent->BindAxis(TEXT("Look Up / Down Mouse"), this, &AYSH_Player::OnAxisLookupPitch);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &AYSH_Player::OnActionJump);

	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &AYSH_Player::OnActionFire );

	PlayerInputComponent->BindAction(TEXT("ChooseGrenadeGun"), IE_Pressed, this, &AYSH_Player::OnActionChooseGrenadeGun);

	PlayerInputComponent->BindAction(TEXT("ChooseSniperGun"), IE_Pressed, this, &AYSH_Player::OnActionChooseSniperGun);

	PlayerInputComponent->BindAction(TEXT("ChooseChainsaw"), IE_Pressed, this, &AYSH_Player::OnActionChooseChainsaw);

	PlayerInputComponent->BindAction(TEXT("Zoom"), IE_Pressed, this, &AYSH_Player::OnActionZoomIn);

	PlayerInputComponent->BindAction(TEXT("Zoom"), IE_Released, this, &AYSH_Player::OnActionZoomOut);

	PlayerInputComponent->BindAction(TEXT("PickUp"), IE_Pressed, this, &AYSH_Player::OnActionPickUp);
}

void AYSH_Player::Move()
{
	FTransform trans = GetActorTransform();
	AddMovementInput(trans.TransformVector(direction));
}

void AYSH_Player::OnAxisVertical(float value)
{
	direction.X = value;
}

void AYSH_Player::OnAxisHorizontal(float value)
{
	direction.Y = value;
}

void AYSH_Player::OnAxisTurnYaw(float value)
{
	AddControllerYawInput(value);
}

void AYSH_Player::OnAxisLookupPitch(float value)
{
	AddControllerPitchInput(value);
}

void AYSH_Player::OnActionJump()
{
	Jump();
}

void AYSH_Player::OnActionFire()
{
	//auto anim = Cast<UYSH_PlayerAnim>(GetMesh()->GetAnimInstance());
	//anim->PlayerAttackAnim();
	this->PlayAnimMontage(fireMontage, 1);

	switch (CurrentWeaponType)
	{
	case EWeapon::GrenadeGun:
		// 수류탄 총알을 발사하는 동작을 수행합니다.
		if (bCanFire && CurrentGreMagazin > 0)
		{
			FTransform t = gunMeshComp->GetSocketTransform(TEXT("FirePosition"));
			GetWorld()->SpawnActor<AYSH_BulletActor>(bulletFactory, t);
			CurrentGreMagazin -= 1;
			crossHairUI->WhiteAimInvisible();

			if (CurrentGreMagazin == 0)
			{
				if (totalGreMagazin > 0)
				{
					StartReloadGre();
				}
			}
		}
		else if (CurrentGreMagazin == 0)
		{
			if (totalGreMagazin > 0)
			{
				StartReloadGre();
			}
		}
		break;
	case EWeapon::SniperGun:
		// 저격총 총알을 발사하는 동작을 수행합니다.
		if (bCanFire && CurrentSnaMagazin > 0)
		{
			FHitResult outHit;
			FVector start = cameraComp->GetComponentLocation();
			FVector end = start + cameraComp->GetForwardVector() * 100000;
			FCollisionQueryParams params;
			params.AddIgnoredActor(this);

			bool bReturnValue = GetWorld()->LineTraceSingleByChannel(outHit, start, end, ECollisionChannel::ECC_Visibility, params);
			if (bReturnValue)
			{
				//DrawDebugLine(GetWorld(), outHit.TraceStart, outHit.ImpactPoint, FColor::Red, false, 5);

				UPrimitiveComponent* hitComp = outHit.GetComponent();

				if (hitComp && hitComp->IsSimulatingPhysics())
				{

					FVector dir = end - start;
					hitComp->AddForce(dir.GetSafeNormal() * 500000 * hitComp->GetMass());

				}
				//outHit.ImpactPoint
				//zombie damage
				auto enemy = outHit.GetActor()->GetDefaultSubobjectByName(TEXT("FSM"));
				if (enemy)
				{
					UE_LOG(LogTemp, Log, TEXT("hit"));
					auto enemyFSM = Cast<ULSH_EnemyFSM>(enemy);
					enemyFSM->OnDamageProcess();
					crossHairUI->RedAimVisible();

				}
			}

			CurrentSnaMagazin -= 1;
			crossHairUI->WhiteAimInvisible();

			if (CurrentSnaMagazin == 0)
			{
				if (totalSnaMagazin > 0)
				{
					StartReloadSna();
				}
			}

		}
		else if (CurrentSnaMagazin == 0)
		{
			if (totalSnaMagazin > 0)
			{
				StartReloadSna();
			}
		}
		break;
	case EWeapon::Chainsaw:
		// 연쇄톱 동작을 수행합니다.
		break;
	default:
		// 기본 동작을 수행합니다.
		break;
	}
}


void AYSH_Player::StartReloadGre()
{
	if (bCanFire && CurrentGreMagazin == 0)
	{
		BeginReloadGre();
	}
}

void AYSH_Player::ReloadCompleteGre()
{
	CurrentGreMagazin = FMath::Min(totalGreMagazin, GreMagazin);
	totalGreMagazin -= CurrentGreMagazin;
	reloadUI->SetVisibility(ESlateVisibility::Hidden);

	bCanFire = true;
}

void AYSH_Player::BeginReloadGre()
{

	bCanFire = false;

	reloadUI->SetVisibility(ESlateVisibility::Visible);
	reloadUI->ReloadPlayAnimation();


	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AYSH_Player::ReloadCompleteGre, ReloadTime, false);


	CurrentGreMagazin = GreMagazin;
	totalGreMagazin -= GreMagazin;
}

void AYSH_Player::StartReloadSna()
{
	if (bCanFire && CurrentSnaMagazin == 0 )
	{
		BeginReloadSna();
	}
}

void AYSH_Player::ReloadCompleteSna()
{
	CurrentSnaMagazin = FMath::Min(totalSnaMagazin, SnaMagazin);
	totalSnaMagazin -= CurrentSnaMagazin;
	reloadUI->SetVisibility(ESlateVisibility::Hidden);

	bCanFire = true;
}

void AYSH_Player::BeginReloadSna()
{

	bCanFire = false;

	reloadUI->SetVisibility(ESlateVisibility::Visible);
	reloadUI->ReloadPlayAnimation();


	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AYSH_Player::ReloadCompleteSna, ReloadTime, false);


	CurrentSnaMagazin = SnaMagazin;
	totalSnaMagazin -= SnaMagazin;
}


void AYSH_Player::OnActionChooseGrenadeGun()
{
	CurrentWeaponType = EWeapon::GrenadeGun;

	gunMeshComp->SetVisibility(true);
	sniperMeshComp->SetVisibility(false);
	ChainsawMeshComp->SetVisibility(false);

	OnActionZoomOut();
}

void AYSH_Player::OnActionChooseSniperGun()
{
	CurrentWeaponType = EWeapon::SniperGun;

	gunMeshComp->SetVisibility(false);
	sniperMeshComp->SetVisibility(true);
	ChainsawMeshComp->SetVisibility(false);
}

void AYSH_Player::OnActionChooseChainsaw()
{
	CurrentWeaponType = EWeapon::Chainsaw;
	gunMeshComp->SetVisibility(false);
	sniperMeshComp->SetVisibility(false);
	ChainsawMeshComp->SetVisibility(true);
}

void AYSH_Player::Zoom()
{
	cameraComp->FieldOfView = FMath::Lerp<float>(cameraComp->FieldOfView, targetFOV, GetWorld()->GetDeltaSeconds() * 10);
}

void AYSH_Player::OnActionZoomIn()
{

	if (CurrentWeaponType == EWeapon::Chainsaw)
	{
		return;
	}

	if(CurrentWeaponType == EWeapon::SniperGun)
	{
		crossHairUI->SetVisibility(ESlateVisibility::Hidden);
		sniperUI->SetVisibility(ESlateVisibility::Visible);
		targetFOV = 30;
	}else if(CurrentWeaponType == EWeapon::GrenadeGun)
	{
		crossHairUI->WhiteAimVisible();
		targetFOV = 50;
	}
}

void AYSH_Player::OnActionZoomOut()
{
	if (CurrentWeaponType == EWeapon::Chainsaw)
	{
		return;
	}

	// Crosshair UI를 표시하고 Sniper UI를 숨깁니다.
	crossHairUI->SetVisibility(ESlateVisibility::Visible);
	crossHairUI->WhiteAimInvisible();
	sniperUI->SetVisibility(ESlateVisibility::Hidden);
	targetFOV = 90;
}

void AYSH_Player::OnActionPickUp()
{
	// 플레이어 주변에 물체 감지 (충돌 체크 등)
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, AAmmoActor::StaticClass());

	// 상호작용 가능한 물체 감지 후 처리
	for (AActor* Actor : OverlappingActors)
	{
		AAmmoActor* AmmoActor = Cast<AAmmoActor>(Actor);
		if (AmmoActor) // 상호작용 가능한 물체인지 확인
		{
			// 플레이어의 총알 수에 탄창의 수량을 추가
			totalSnaMagazin += 50;
			// 탄창을 비활성화하거나 제거
			AmmoActor->AutoDestroy(); // 이 예시에서는 비활성화 처리하도록 가정

		}
	}
}
