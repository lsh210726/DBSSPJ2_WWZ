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



// Sets default values
AYSH_Player::AYSH_Player()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// springArmComp를 생성해서 Root에 붙이고싶다.
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("springArmComp"));
	springArmComp->SetupAttachment(RootComponent);
	// Loc : (X=0.000000,Y=70.000000,Z=90.000000)
	springArmComp->SetWorldLocation(FVector(0, 70, 90));
	// cameraComp를 생성해서 springArmComp에 붙이고싶다.
	cameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("cameraComp"));
	cameraComp->SetupAttachment(springArmComp);

	// mesh를 로드해서 적용하고싶다.
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Quinn.SKM_Quinn'"));
	// 성공했다면
	if (tempMesh.Succeeded())
	{
		// Mesh에 적용하고싶다.
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}

	bUseControllerRotationYaw = true;
	// Use Pawn Control Rotation을 True로 하고싶다.
	springArmComp->bUsePawnControlRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;


	// gunMeshComp를 생성해서 로딩도하고 배치하고싶다. Mesh에 붙이고싶다.
	gunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("gunMeshComp"));
	gunMeshComp->SetupAttachment(GetMesh());

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempGunMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/YSH/Models/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));

	if (tempGunMesh.Succeeded())
	{
		gunMeshComp->SetSkeletalMesh(tempGunMesh.Object);
		gunMeshComp->SetRelativeLocation(FVector(0, 50, 130));
	}

	// sniperMeshComp(UStaticMeshComponent)를 생성해서 메쉬에 붙이고싶다.
	sniperMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("sniperMeshComp"));
	sniperMeshComp->SetupAttachment(GetMesh());
	// UStaticMesh를 로드해서 적용하고싶다.
	ConstructorHelpers::FObjectFinder<UStaticMesh> tempSniper(TEXT("/Script/Engine.StaticMesh'/Game/YSH/Models/SniperGun/sniper1.sniper1'"));

	if (tempSniper.Succeeded())
	{
		sniperMeshComp->SetStaticMesh(tempSniper.Object);
		sniperMeshComp->SetRelativeLocation(FVector(0, 80, 130));
		sniperMeshComp->SetWorldScale3D(FVector(0.15f));
	}

	// ChainsawMeshComp를 생성해서 로딩도하고 배치하고싶다. Mesh에 붙이고싶다.
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

	auto sniperWidget = CreateWidget(GetWorld(), sniperFactory);
	sniperUI = Cast<USniperUserWidget>(sniperWidget);
	sniperUI->AddToViewport();

	auto reloadWidget = CreateWidget(GetWorld(), reloadFactory);
	reloadUI = Cast<UReloadUserWidget>(reloadWidget);
	reloadUI->AddToViewport();

	auto playerWidget = CreateWidget(GetWorld(), playerFactory);
	playerUI = Cast<UPlayerUserWidget>(playerWidget);
	playerUI->AddToViewport();

	// 태어날 때 기본조준(CrossHair UI)만 보이게하고싶다
	sniperUI->SetVisibility(ESlateVisibility::Hidden);
	reloadUI->SetVisibility(ESlateVisibility::Hidden);
	

	// 총을 교체하면 ZoomOut을 하고싶다.

	OnActionChooseGrenadeGun();

	//탄약 설정
	GreMagazin = 10;
	CurrentGreMagazin = 10;
	totalGreMagazin = 100;

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

	// 가로축, 세로축, 점프에대한 함수를 바인딩 하고싶다.

	// 주어->기능(필요한 값)
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
	crossHairUI->WhiteAimVisible();

	if (bCanFire && CurrentGreMagazin > 0)
	{
		FTransform t = gunMeshComp->GetSocketTransform(TEXT("FirePosition"));
		GetWorld()->SpawnActor<AYSH_BulletActor>(bulletFactory, t);
		CurrentGreMagazin -= 1;
		crossHairUI->WhiteAimInvisible();

		// 총알을 발사한 후에 탄창이 비어있으면 재장전을 시작합니다.
		if (CurrentGreMagazin == 0)
		{
			StartReload();
		}
	}
	else if (bCanFire && CurrentGreMagazin == 1)
	{
		// 탄창이 비어있으면 재장전을 시작합니다.
		StartReload();
	}
}


void AYSH_Player::StartReload()
{
	if (bCanFire && totalGreMagazin > 1)
	{
		// 리로딩 시작
		BeginReload();
	}
}

void AYSH_Player::ReloadComplete()
{
	// 탄창을 교체하고 재장전 UI를 숨깁니다.
	CurrentGreMagazin = FMath::Min(totalGreMagazin, GreMagazin);
	totalGreMagazin -= CurrentGreMagazin;
	reloadUI->SetVisibility(ESlateVisibility::Hidden);

	// 재장전이 완료되면 다시 총을 쏠 수 있도록 허용합니다.
	bCanFire = true;
}

void AYSH_Player::BeginReload()
{
	// 리로딩 시작
	bCanFire = false;
	crossHairUI->WhiteAimInvisible();

	// 리로딩 UI 표시 및 애니메이션 재생
	reloadUI->SetVisibility(ESlateVisibility::Visible);
	reloadUI->ReloadPlayAnimation();

	// 딜레이 후 재장전 완료
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AYSH_Player::ReloadComplete, ReloadTime, false);

	// 탄창 재충전
	CurrentGreMagazin = GreMagazin;
	totalGreMagazin -= GreMagazin;
}


void AYSH_Player::OnActionChooseGrenadeGun()
{
	bChooseSniperGun = false;
	// 유탄총을 보이게, 스나이퍼를 안보이게
	gunMeshComp->SetVisibility(true);
	sniperMeshComp->SetVisibility(false);
	ChainsawMeshComp->SetVisibility(false);
	//총을 교체하면 ZoomOut을 하고싶다.
	OnActionZoomOut();
}

void AYSH_Player::OnActionChooseSniperGun()
{
	bChooseSniperGun = true;
	// 유탄총을 안보이게, 스나이퍼를 보이게
	gunMeshComp->SetVisibility(false);
	sniperMeshComp->SetVisibility(true);
	ChainsawMeshComp->SetVisibility(false);
}

void AYSH_Player::OnActionChooseChainsaw()
{
	bChooseSniperGun = false;
	gunMeshComp->SetVisibility(false);
	sniperMeshComp->SetVisibility(false);
	ChainsawMeshComp->SetVisibility(true);
}

void AYSH_Player::Zoom()
{
	//선형보간을 이용해서 현재 FOV를 targetFOV값에 근접하게 하고싶다.
	cameraComp->FieldOfView = FMath::Lerp<float>(cameraComp->FieldOfView, targetFOV, GetWorld()->GetDeltaSeconds() * 10);
}

FORCEINLINE void AYSH_Player::OnActionZoomIn()
{
	//만약 손에 쥔 총이 스나이퍼가 아니라면 함수를 바로 종료하고싶다.
	if (false == bChooseSniperGun)
	{
		return;
	}//return을 만나면 바로 함수를 종료함 (어?너 아니네? 끝)

	// ZoomIn을 하면 SniperUI를 보이게 하고싶다.(CrossHairUI 안보이게)
	crossHairUI->SetVisibility(ESlateVisibility::Hidden);
	sniperUI->SetVisibility(ESlateVisibility::Visible);
	targetFOV = 30;
}

FORCEINLINE void AYSH_Player::OnActionZoomOut()
{
	// ZoomOut을 하면 CossHair를 보이게 하고싶다.(SniperUI 안보이게)
	crossHairUI->SetVisibility(ESlateVisibility::Visible);
	sniperUI->SetVisibility(ESlateVisibility::Hidden);
	targetFOV = 90;
}