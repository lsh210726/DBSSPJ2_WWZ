// Fill out your copyright notice in the Description page of Project Settings.


#include "SniperUserWidget.h"

void USniperUserWidget::SniperAimInvisible()
{
	SetVisibility(ESlateVisibility::Hidden);
}

void USniperUserWidget::SniperAimVisible()
{
	SetVisibility(ESlateVisibility::Visible);
}
