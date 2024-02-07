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


void UAimUserWidget::OnFireDelayComplete()
{
    // ���� �ð��� ���� �Ŀ� WhiteAimInvisible() �Լ� ȣ��
    WhiteAimInvisible();
}
