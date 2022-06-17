// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Zombie.h"
#include "SpawnManagerBase.generated.h"

UCLASS()
class ZOMBIEGAME_API ASpawnManagerBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ASpawnManagerBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<AZombie*> respawneables;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	void RegisterReespawneableZombie(AZombie* reespawneable);
	AZombie* getRespawneableZombie();
};
