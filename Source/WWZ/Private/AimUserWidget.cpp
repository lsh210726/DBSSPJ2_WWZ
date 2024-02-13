// Fill out your copyright notice in the Description page of Project Settings.


#include "AimUserWidget.h"
#include "Components/Image.h"


void UAimUserWidget::WhiteAimInvisible()
{
	WhiteCrossAim->SetVisibility(ESlateVisibility::Hidden);
}

void UAimUserWidget::WhiteAimVisible()
{
	WhiteCrossAim->SetVisibility(ESlateVisibility::Visible);
}

void UAimUserWidget::RedAimVisible()
{
	RedCrossAim->SetVisibility(ESlateVisibility::Visible);

	FTimerHandle CrosshairTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(CrosshairTimerHandle, this, &UAimUserWidget::RedAimInvisible, 0.2f, false);
}

void UAimUserWidget::RedAimInvisible()
{
	RedCrossAim->SetVisibility(ESlateVisibility::Hidden);
}


void UAimUserWidget::OnFireDelayComplete()
{
    // 일정 시간이 지난 후에 WhiteAimInvisible() 함수 호출
    WhiteAimInvisible();
}
