// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AnimI_Zombie.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIEGAME_API UAnimI_Zombie : public UAnimInstance
{
	GENERATED_BODY()

public: 
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		bool Attaking;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		bool Die;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		bool GetHited;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		bool Spitting;
public:
	
	UFUNCTION(BlueprintCallable)
		void ChangeAttackValue(bool value);
	UFUNCTION(BlueprintCallable)
		void ChangeHitValue(bool value);
	UFUNCTION(BlueprintCallable)
		void ChangeLifeValue(bool value);
	UFUNCTION(BlueprintCallable)
		void ChangeSpitValue(bool value);
	
};
