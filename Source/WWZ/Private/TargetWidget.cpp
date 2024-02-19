// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetWidget.h"

void UTargetWidget::PlayMissionAnim()
{
	PlayAnimation(CenterMissionAnim);
}

void UTargetWidget::PlayRadioAnim()
{
	PlayAnimation(RadioConversation);
}
