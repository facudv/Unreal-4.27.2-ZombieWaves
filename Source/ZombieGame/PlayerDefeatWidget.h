// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerDefeatWidget.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIEGAME_API UPlayerDefeatWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UWidgetAnimation* playerDefeatWidgetAnimation;

};
