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

	// springArmComp�� �����ؼ� Root�� ���̰�ʹ�.
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("springArmComp"));
	springArmComp->SetupAttachment(RootComponent);
	// Loc : (X=0.000000,Y=70.000000,Z=90.000000)
	springArmComp->SetWorldLocation(FVector(0, 70, 90));
	// cameraComp�� �����ؼ� springArmComp�� ���̰�ʹ�.
	cameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("cameraComp"));
	cameraComp->SetupAttachment(springArmComp);

	// mesh�� �ε��ؼ� �����ϰ�ʹ�.
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Quinn.SKM_Quinn'"));
	// �����ߴٸ�
	if (tempMesh.Succeeded())
	{
		// Mesh�� �����ϰ�ʹ�.
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}

	bUseControllerRotationYaw = true;
	// Use Pawn Control Rotation�� True�� �ϰ�ʹ�.
	springArmComp->bUsePawnControlRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;


	// gunMeshComp�� �����ؼ� �ε����ϰ� ��ġ�ϰ�ʹ�. Mesh�� ���̰�ʹ�.
	gunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("gunMeshComp"));
	gunMeshComp->SetupAttachment(GetMesh());

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempGunMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/YSH/Models/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));

	if (tempGunMesh.Succeeded())
	{
		gunMeshComp->SetSkeletalMesh(tempGunMesh.Object);
		gunMeshComp->SetRelativeLocation(FVector(0, 50, 130));
	}

	// sniperMeshComp(UStaticMeshComponent)�� �����ؼ� �޽��� ���̰�ʹ�.
	sniperMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("sniperMeshComp"));
	sniperMeshComp->SetupAttachment(GetMesh());
	// UStaticMesh�� �ε��ؼ� �����ϰ�ʹ�.
	ConstructorHelpers::FObjectFinder<UStaticMesh> tempSniper(TEXT("/Script/Engine.StaticMesh'/Game/YSH/Models/SniperGun/sniper1.sniper1'"));

	if (tempSniper.Succeeded())
	{
		sniperMeshComp->SetStaticMesh(tempSniper.Object);
		sniperMeshComp->SetRelativeLocation(FVector(0, 80, 130));
		sniperMeshComp->SetWorldScale3D(FVector(0.15f));
	}

	// ChainsawMeshComp�� �����ؼ� �ε����ϰ� ��ġ�ϰ�ʹ�. Mesh�� ���̰�ʹ�.
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

	// �¾ �� �⺻����(CrossHair UI)�� ���̰��ϰ�ʹ�
	sniperUI->SetVisibility(ESlateVisibility::Hidden);
	reloadUI->SetVisibility(ESlateVisibility::Hidden);
	

	// ���� ��ü�ϸ� ZoomOut�� �ϰ�ʹ�.

	OnActionChooseGrenadeGun();

	//ź�� ����
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

	// ������, ������, ���������� �Լ��� ���ε� �ϰ�ʹ�.

	// �־�->���(�ʿ��� ��)
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

		// �Ѿ��� �߻��� �Ŀ� źâ�� ��������� �������� �����մϴ�.
		if (CurrentGreMagazin == 0)
		{
			StartReload();
		}
	}
	else if (bCanFire && CurrentGreMagazin == 1)
	{
		// źâ�� ��������� �������� �����մϴ�.
		StartReload();
	}
}


void AYSH_Player::StartReload()
{
	if (bCanFire && totalGreMagazin > 1)
	{
		// ���ε� ����
		BeginReload();
	}
}

void AYSH_Player::ReloadComplete()
{
	// źâ�� ��ü�ϰ� ������ UI�� ����ϴ�.
	CurrentGreMagazin = FMath::Min(totalGreMagazin, GreMagazin);
	totalGreMagazin -= CurrentGreMagazin;
	reloadUI->SetVisibility(ESlateVisibility::Hidden);

	// �������� �Ϸ�Ǹ� �ٽ� ���� �� �� �ֵ��� ����մϴ�.
	bCanFire = true;
}

void AYSH_Player::BeginReload()
{
	// ���ε� ����
	bCanFire = false;
	crossHairUI->WhiteAimInvisible();

	// ���ε� UI ǥ�� �� �ִϸ��̼� ���
	reloadUI->SetVisibility(ESlateVisibility::Visible);
	reloadUI->ReloadPlayAnimation();

	// ������ �� ������ �Ϸ�
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AYSH_Player::ReloadComplete, ReloadTime, false);

	// źâ ������
	CurrentGreMagazin = GreMagazin;
	totalGreMagazin -= GreMagazin;
}


void AYSH_Player::OnActionChooseGrenadeGun()
{
	bChooseSniperGun = false;
	// ��ź���� ���̰�, �������۸� �Ⱥ��̰�
	gunMeshComp->SetVisibility(true);
	sniperMeshComp->SetVisibility(false);
	ChainsawMeshComp->SetVisibility(false);
	//���� ��ü�ϸ� ZoomOut�� �ϰ�ʹ�.
	OnActionZoomOut();
}

void AYSH_Player::OnActionChooseSniperGun()
{
	bChooseSniperGun = true;
	// ��ź���� �Ⱥ��̰�, �������۸� ���̰�
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
	//���������� �̿��ؼ� ���� FOV�� targetFOV���� �����ϰ� �ϰ�ʹ�.
	cameraComp->FieldOfView = FMath::Lerp<float>(cameraComp->FieldOfView, targetFOV, GetWorld()->GetDeltaSeconds() * 10);
}

FORCEINLINE void AYSH_Player::OnActionZoomIn()
{
	//���� �տ� �� ���� �������۰� �ƴ϶�� �Լ��� �ٷ� �����ϰ�ʹ�.
	if (false == bChooseSniperGun)
	{
		return;
	}//return�� ������ �ٷ� �Լ��� ������ (��?�� �ƴϳ�? ��)

	// ZoomIn�� �ϸ� SniperUI�� ���̰� �ϰ�ʹ�.(CrossHairUI �Ⱥ��̰�)
	crossHairUI->SetVisibility(ESlateVisibility::Hidden);
	sniperUI->SetVisibility(ESlateVisibility::Visible);
	targetFOV = 30;
}

FORCEINLINE void AYSH_Player::OnActionZoomOut()
{
	// ZoomOut�� �ϸ� CossHair�� ���̰� �ϰ�ʹ�.(SniperUI �Ⱥ��̰�)
	crossHairUI->SetVisibility(ESlateVisibility::Visible);
	sniperUI->SetVisibility(ESlateVisibility::Hidden);
	targetFOV = 90;
}