// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemLoadingUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class WWZ_API UItemLoadingUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidget))
	class UImage* LoadingCircle;

	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* Loading;

	void LoadingAnimation();
};
