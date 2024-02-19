// Fill out your copyright notice in the Description page of Project Settings.


#include "MissionActor.h"

#include "TargetWidget.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Components/BoxComponent.h"

// Sets default values
AMissionActor::AMissionActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("boxComp"));
	SetRootComponent(boxComp);

	boxComp->OnComponentBeginOverlap.AddDynamic(this, &AMissionActor::OnOverlapBegin);
	boxComp->OnComponentEndOverlap.AddDynamic(this, &AMissionActor::OnOverlapEnd);

	bHasOverlapped = false; // �ʱ�ȭ
}

// Called when the game starts or when spawned
void AMissionActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMissionActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMissionActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    //if (!bHasOverlapped)
    //{
        // Ÿ�� ���� ����
       // TargetWidget = CreateWidget<UTargetWidget>(GetWorld(), UTargetWidget::StaticClass());

        //if (TargetWidget)
       // {
            //TargetWidget->AddToViewport(); // ȭ�鿡 �߰�
           // TargetWidget->PlayMissionAnim(); // �ִϸ��̼� ����
        //}

       // bHasOverlapped = true; // �������� �߻������� ǥ��
   // }
}

void AMissionActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//bHasOverlapped = false; // ������ ����
    //TargetWidget->SetVisibility(ESlateVisibility::Hidden);
}

