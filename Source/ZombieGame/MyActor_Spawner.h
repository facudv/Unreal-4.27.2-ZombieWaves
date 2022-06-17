// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "Zombie.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "MyActor_Spawner.generated.h"

class AZG_GameModeBase;

UCLASS()
class ZOMBIEGAME_API AMyActor_Spawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AMyActor_Spawner();
	UPROPERTY(EditAnyWhere)
		TSubclassOf<AZombie> prefabZombie;
	UPROPERTY(EditAnyWhere)
		float timerToSpawn;
	UPROPERTY(EditAnyWhere)
		bool bucleSpawn;
	UPROPERTY(EditAnywhere)
		TArray<AZombie*> toRespawn;

	FTimerHandle timerSpawn;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	void SpawnZombie();
	void RegisterToRespawn(AZombie* zombie);
};
