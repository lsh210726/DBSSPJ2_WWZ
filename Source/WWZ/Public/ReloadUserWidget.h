// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReloadUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class WWZ_API UReloadUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* reloadAnim;

	//FWidgetAnimationDynamicEvent StartDelegate;

	UFUNCTION()
	void ReloadPlayAnimation();

};
