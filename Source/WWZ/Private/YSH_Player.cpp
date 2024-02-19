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
#include "Kismet/GameplayStatics.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "../../../../../../../Source/Editor/AudioEditor/Public/AssetTypeActions/AssetDefinition_SoundBase.h"
#include "Components/BoxComponent.h"
#include "LSH_Granade.h"

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

	//총 메시
	gunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("gunMeshComp"));
	gunMeshComp->SetupAttachment(GetMesh());
	gunMeshComp->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("GunSocket"));

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempGunMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/YSH/Models/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));

	if (tempGunMesh.Succeeded())
	{
		gunMeshComp->SetSkeletalMesh(tempGunMesh.Object);
		gunMeshComp->SetRelativeLocation(FVector(0, 50, 130));
	}


	sniperMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("sniperMeshComp"));
	sniperMeshComp->SetupAttachment(GetMesh());
	sniperMeshComp->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("GunSocket"));

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempSniper(TEXT("/Script/Engine.StaticMesh'/Game/YSH/Models/SniperGun/sniper1.sniper1'"));

	if (tempSniper.Succeeded())
	{
		sniperMeshComp->SetStaticMesh(tempSniper.Object);
		sniperMeshComp->SetRelativeLocation(FVector(0, 80, 130));
		sniperMeshComp->SetWorldScale3D(FVector(0.15f));
	}

	rifleMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("rifleMeshComp"));
	rifleMeshComp->SetupAttachment(GetMesh());
	rifleMeshComp->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("GunSocket"));

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempRifle(TEXT("/Script/Engine.StaticMesh'/Game/YSH/Models/Rifle/SM_RifleMerge.SM_RifleMerge'"));

	if (tempRifle.Succeeded())
	{
		rifleMeshComp->SetStaticMesh(tempRifle.Object);
		rifleMeshComp->SetRelativeLocation(FVector(-10, 50, 150));
	}

	ChainsawMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ChainsawMeshComp"));
	ChainsawMeshComp->SetupAttachment(GetMesh());
	ChainsawMeshComp->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("GunSocket"));
	box = CreateDefaultSubobject<UBoxComponent>(TEXT("box"));
	box->SetupAttachment(ChainsawMeshComp);
	//box->SetRelativeLocation(FVector(2166,388,166));
	//box->SetupAttachment(TEXT("ChainsawMeshComp"));

	ConstructorHelpers::FObjectFinder<USkeletalMesh> ChainsawMeshFinder(TEXT("/Script/Engine.SkeletalMesh'/Game/YSH/Models/Chainsaw/source/model.model_Core'"));

	if (ChainsawMeshFinder.Succeeded())
	{
		ChainsawMeshComp->SetSkeletalMesh(ChainsawMeshFinder.Object);
		ChainsawMeshComp->SetRelativeLocationAndRotation(FVector(0, 50, 130), FRotator(0, 0, 90));
		ChainsawMeshComp->SetRelativeScale3D(FVector(0.01));
	}

	gunMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	sniperMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	box->OnComponentBeginOverlap.AddDynamic(this, &AYSH_Player::OnOverlapBegin);
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


}

// Called every frame
void AYSH_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Move();

	Zoom();

	if (bAutoFire)
	{
		//	시간이 흐르다가 
		currentTime += DeltaTime;
		//	만약 발사시간이되면
		if (currentTime >= fireTime)
		{
			// MakeBullet함수를 호출하고싶다.
			MakeRifleBullet();
			currentTime = 0;
		}
	}
	
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

	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Released, this, &AYSH_Player::OnMyActionRifleFireReleased);

	PlayerInputComponent->BindAction(TEXT("ChooseGrenadeGun"), IE_Pressed, this, &AYSH_Player::OnActionChooseGrenadeGun);

	PlayerInputComponent->BindAction(TEXT("ChooseSniperGun"), IE_Pressed, this, &AYSH_Player::OnActionChooseSniperGun);

	PlayerInputComponent->BindAction(TEXT("ChooseChainsaw"), IE_Pressed, this, &AYSH_Player::OnActionChooseChainsaw);

	PlayerInputComponent->BindAction(TEXT("ChooseRifleGun"), IE_Pressed, this, &AYSH_Player::OnActionChooseRifleGun);

	PlayerInputComponent->BindAction(TEXT("Zoom"), IE_Pressed, this, &AYSH_Player::OnActionZoomIn);

	PlayerInputComponent->BindAction(TEXT("Zoom"), IE_Released, this, &AYSH_Player::OnActionZoomOut);

	PlayerInputComponent->BindAction(TEXT("PickUp"), IE_Pressed, this, &AYSH_Player::OnActionPickUp);
}

void AYSH_Player::Move()
{
	FTransform trans = GetActorTransform();
	AddMovementInput(trans.TransformVector(direction));

	// 이동할때 발자국소리를 내고싶다.
	//if (!bIsFootstepSoundPlaying && GetVelocity().SizeSquared() > 0)
	//{
		// 발자국 소리 재생
		//UGameplayStatics::PlaySound2D(GetWorld(), FootstepSound);
		
	//}
	//else if (GetVelocity().SizeSquared() == 0)
	//{
		// 이동이 멈추면 발자국소리를 중지하고싶다
	//	UGameplayStatics::StopSound(GetWorld(), FootstepSound);
	//	bIsFootstepSoundPlaying = false;
	//}
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
	UGameplayStatics::PlaySound2D(GetWorld(), runSound);

	Jump();
}

void AYSH_Player::OnActionFire()
{
	// 카메라 쉐이크 재생

	switch (CurrentWeaponType)
	{
	case EWeapon::GrenadeGun:
		// 수류탄 총알을 발사하는 동작을 수행합니다.
		if (bCanFire && CurrentGreMagazin > 0)
		{
			auto controller = GetWorld()->GetFirstPlayerController();
			controller->PlayerCameraManager->StartCameraShake(cameraShake);
			this->PlayAnimMontage(fireMontage, 1);
			FTransform t = gunMeshComp->GetSocketTransform(TEXT("FirePosition"));
			GetWorld()->SpawnActor<ALSH_Granade>(bulletFactory, t);
			CurrentGreMagazin -= 1;
			crossHairUI->WhiteAimInvisible();

			if (CurrentGreMagazin == 0)
			{
				if (totalGreMagazin > 0)
				{
					StartReload();
				}
			}
		}
		else if (CurrentGreMagazin == 0)
		{
			if (totalGreMagazin > 0)
			{
				StartReload();
			}
		}
		break;
	case EWeapon::RifleGun:
		OnMyActionRiflePressed();
		break;
	case EWeapon::SniperGun:
		// 저격총 총알을 발사하는 동작을 수행합니다.
		if (bCanFire && CurrentSnaMagazin > 0)
		{
			auto controller = GetWorld()->GetFirstPlayerController();
			controller->PlayerCameraManager->StartCameraShake(cameraShake);
			this->PlayAnimMontage(fireMontage, 1);

			UGameplayStatics::PlaySound2D(GetWorld(), sniperSound);

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
					enemyFSM->OnDamageProcess(4);
					enemyFSM->ShootForce(outHit.Normal * -10000);
					crossHairUI->RedAimVisible();

				}
			}

			CurrentSnaMagazin -= 1;
			crossHairUI->WhiteAimInvisible();

			if (CurrentSnaMagazin == 0)
			{
				if (totalSnaMagazin > 0)
				{
					StartReload();
				}
			}

		}
		else if (CurrentSnaMagazin == 0)
		{
			if (totalSnaMagazin > 0)
			{
				StartReload();
			}
		}
		break;
	case EWeapon::Chainsaw:
		//전기톱 콜리전을 활성화 하고싶다.
		box->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		break;
	default:
		// 기본 동작을 수행합니다.
		break;
	}
}

void AYSH_Player::OnActionChooseGrenadeGun()
{
	CurrentWeaponType = EWeapon::GrenadeGun;

	gunMeshComp->SetVisibility(true);
	sniperMeshComp->SetVisibility(false);
	ChainsawMeshComp->SetVisibility(false);
	rifleMeshComp->SetVisibility(false);

	OnActionZoomOut();
}

void AYSH_Player::OnActionChooseSniperGun()
{
	CurrentWeaponType = EWeapon::SniperGun;

	gunMeshComp->SetVisibility(false);
	sniperMeshComp->SetVisibility(true);
	ChainsawMeshComp->SetVisibility(false);
	rifleMeshComp->SetVisibility(false);
}

void AYSH_Player::OnActionChooseChainsaw()
{
	CurrentWeaponType = EWeapon::Chainsaw;

	gunMeshComp->SetVisibility(false);
	sniperMeshComp->SetVisibility(false);
	ChainsawMeshComp->SetVisibility(true);
	rifleMeshComp->SetVisibility(false);

	//UGameplayStatics::PlaySound2D(GetWorld(), chainsawSound);

}

void AYSH_Player::OnActionChooseRifleGun()
{
	CurrentWeaponType = EWeapon::RifleGun;
	gunMeshComp->SetVisibility(false);
	sniperMeshComp->SetVisibility(false);
	ChainsawMeshComp->SetVisibility(false);
	rifleMeshComp->SetVisibility(true);

	OnActionZoomOut();
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
	}else if(CurrentWeaponType == EWeapon::GrenadeGun || CurrentWeaponType == EWeapon::RifleGun)
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

void AYSH_Player::OnMyActionRiflePressed()
{
	bAutoFire = true;
	MakeRifleBullet();
}

void AYSH_Player::OnMyActionRifleFireReleased()
{
	bAutoFire = false;
	currentTime = 0;
	box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AYSH_Player::MakeRifleBullet()
{
	//총알을 발사합니다
	if (bCanFire && CurrentRifleMagazin > 0)
	{
		auto controller = GetWorld()->GetFirstPlayerController();
		controller->PlayerCameraManager->StartCameraShake(cameraShake);
		this->PlayAnimMontage(fireMontage, 1);

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

			UGameplayStatics::PlaySound2D(GetWorld(), riflefireSound);

			//부딪힌 곳에 VFX를 생성해서 배치하고싶다.
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), expVFX, outHit.ImpactPoint);

			//outHit.ImpactPoint
			//zombie damage
			auto enemy = outHit.GetActor()->GetDefaultSubobjectByName(TEXT("FSM"));
			if (enemy)
			{
				UE_LOG(LogTemp, Log, TEXT("hit"));
				auto enemyFSM = Cast<ULSH_EnemyFSM>(enemy);
				enemyFSM->OnDamageProcess(2);
				enemyFSM->ShootForce(outHit.Normal * -5000);
				crossHairUI->RedAimVisible();
			}
		}

		CurrentRifleMagazin -= 1;
		crossHairUI->WhiteAimInvisible();

		if (CurrentRifleMagazin == 0)
		{
			if (totalRifleMagazin > 0)
			{
				StartReload();
			}
		}

	}
	else if (CurrentRifleMagazin == 0)
	{
		if (totalRifleMagazin > 0)
		{
			StartReload();
		}
	}
}

void AYSH_Player::StartReload()
{
	if (bCanFire)
	{
		// 현재 무기에 따라 리로드 함수를 호출합니다.
		switch (CurrentWeaponType)
		{
		case EWeapon::SniperGun:
			ReloadWeapon(CurrentSnaMagazin, SnaMagazin, totalSnaMagazin);
			break;
		case EWeapon::RifleGun:
			// Rifle 총의 Reload 시작하는 부분을 여기에 구현
			ReloadWeapon(CurrentRifleMagazin, RifleMagazin, totalRifleMagazin);
			break;
		case EWeapon::GrenadeGun:
			ReloadWeapon(CurrentGreMagazin, GreMagazin, totalGreMagazin);
			break;
		default:
			// 다른 무기에 대한 처리 (추가적인 무기가 있다면 추가)
			break;
		}
	}
}

void AYSH_Player::ReloadWeapon(int& CurrentMagazin, int MaxMagazin, int& TotalMagazin)
{
	bCanFire = false;
	reloadUI->SetVisibility(ESlateVisibility::Visible);
	reloadUI->ReloadPlayAnimation();

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AYSH_Player::ReloadComplete, ReloadTime, false);

	if(TotalMagazin >0)
	{
		CurrentMagazin = MaxMagazin;
		TotalMagazin -= MaxMagazin;
	}
}

void AYSH_Player::ReloadComplete()
{
	// 현재 무기에 따라 리로드 완료 함수를 호출합니다.
	switch (CurrentWeaponType)
	{
	case EWeapon::SniperGun:
		ReloadCompleteWeapon(CurrentSnaMagazin, totalSnaMagazin);
		break;
	case EWeapon::RifleGun:
		// Rifle 총의 Reload가 완료되었을 때 처리하는 부분을 여기에 구현
		ReloadCompleteWeapon(CurrentRifleMagazin, totalRifleMagazin);
		break;
	case EWeapon::GrenadeGun:
		// Rifle 총의 Reload가 완료되었을 때 처리하는 부분을 여기에 구현
		ReloadCompleteWeapon(CurrentGreMagazin, totalGreMagazin);
		break;
	default:
		// 다른 무기에 대한 처리 (추가적인 무기가 있다면 추가)
		break;
	}
}

void AYSH_Player::ReloadCompleteWeapon(int& CurrentMagazin, int& TotalMagazin)
{
	CurrentMagazin = FMath::Min(TotalMagazin, CurrentMagazin);
	TotalMagazin -= CurrentMagazin;
	reloadUI->SetVisibility(ESlateVisibility::Hidden);
	bCanFire = true;
}

void AYSH_Player::OnMyActionChainsawPressed()
{
	bAutoFire = true;
	box->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
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
		if (AmmoActor && bCanPickUp) // 상호작용 가능한 물체인지 확인
		{
			// 플레이어의 총알 수에 탄창의 수량을 추가
			totalGreMagazin = 12;
			totalSnaMagazin = 60;
			totalRifleMagazin = 300;

			// 이 예시에서는 물체를 다시 활성화시키도록 가정
			AmmoActor->SetActorHiddenInGame(false);

			// OnActionPickUp 함수를 30초 후에 다시 호출할 수 있도록 타이머 설정
			bCanPickUp = false;
			FTimerHandle UnusedHandle;
			GetWorldTimerManager().SetTimer(UnusedHandle, this, &AYSH_Player::EnablePickUpAction, 5.0f, false);
		}
	}
}

void AYSH_Player::EnablePickUpAction()
{
	// 30초 후에 다시 픽업을 활성화
	bCanPickUp = true;
}

void AYSH_Player::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& outHit)
{
	auto enemy = outHit.GetActor()->GetDefaultSubobjectByName(TEXT("FSM"));
	if (enemy)
	{
		UE_LOG(LogTemp, Log, TEXT("hit"));
		auto enemyFSM = Cast<ULSH_EnemyFSM>(enemy);
		enemyFSM->OnDamageProcess(4);
		//enemyFSM->ShootForce(outHit.Normal * -10000);
		crossHairUI->RedAimVisible();

	}

}