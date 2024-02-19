// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TargetWidget.generated.h"

/**
 * 
 */
UCLASS()
class WWZ_API UTargetWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidget))
	class UImage* TargetIcon;

	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidget))
	class UTextBlock* TargetText;

	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* CenterMissionAnim;

	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidget))
	class UTextBlock* RadioText;

	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* RadioConversation;

	void PlayMissionAnim();

	void PlayRadioAnim();

};
