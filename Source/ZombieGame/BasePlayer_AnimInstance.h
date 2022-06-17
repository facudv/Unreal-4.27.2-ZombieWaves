// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BasePlayer_AnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIEGAME_API UBasePlayer_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		bool hasWeapon = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		bool isRunning;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		bool isShooting;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		bool isDead;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		float horizontalAxis;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		float verticalAxis;
};
