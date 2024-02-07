// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class WWZ_API UPlayerUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetOptional))
	class UImage* CurrentWeapon;

	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetOptional))
	class UTextBlock* CurrentMag;
	
	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetOptional))
	class UTextBlock* TotalMag;

	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetOptional))
	class UProgressBar* PlayerHP;
};
