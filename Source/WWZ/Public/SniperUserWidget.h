// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SniperUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class WWZ_API USniperUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetOptional))
	class UImage* SniperAim;

	UFUNCTION()
	void SniperAimInvisible();

	UFUNCTION()
	void SniperAimVisible();
};