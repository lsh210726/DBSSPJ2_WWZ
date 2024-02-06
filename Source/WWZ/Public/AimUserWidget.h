// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AimUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class WWZ_API UAimUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetOptional))
	class UImage* WhiteCrossAim;

	UFUNCTION()
	void WhiteAimInvisible();
};
