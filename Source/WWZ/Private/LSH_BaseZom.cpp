// Fill out your copyright notice in the Description page of Project Settings.


#include "LSH_BaseZom.h"
#include "LSH_EnemyFSM.h"


// Sets default values
ALSH_BaseZom::ALSH_BaseZom()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Essets/Character/Zombie/Model/Ch20_nonPBR.Ch20_nonPBR'"));

	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -88), FRotator(0, -90, 0));
	}

	//AI FSM
	fsm = CreateDefaultSubobject<ULSH_EnemyFSM>(TEXT("FSM"));

	ConstructorHelpers::FClassFinder<UAnimInstance> tempClass(TEXT("/Script/Engine.AnimBlueprint'/Game/Essets/Character/Zombie/Anim/ABP_Enemy.ABP_Enemy_C'"));
	if (tempClass.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(tempClass.Class);
	}
}

// Called when the game starts or when spawned
void ALSH_BaseZom::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALSH_BaseZom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ALSH_BaseZom::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

