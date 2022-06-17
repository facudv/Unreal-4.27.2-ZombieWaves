// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ZG_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIEGAME_API UZG_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
		int AcumulatedPoints = 0;

	UPROPERTY(BlueprintReadWrite)
		float TimeSurvived = 0.0f;

	UFUNCTION(BlueprintCallable)
		void ResetData();
};
