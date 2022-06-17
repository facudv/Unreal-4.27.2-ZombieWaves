// Fill out your copyright notice in the Description page of Project Settings.


#include "MyActor_Spawner.h"
#include "ZG_GameModeBase.h"

// Sets default values
AMyActor_Spawner::AMyActor_Spawner()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMyActor_Spawner::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimer(timerSpawn, this, &AMyActor_Spawner::SpawnZombie, timerToSpawn, bucleSpawn);
}

void AMyActor_Spawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMyActor_Spawner::SpawnZombie()
{
	AZG_GameModeBase* baseGameMode = Cast<AZG_GameModeBase>(GetWorld()->GetAuthGameMode());
	if(prefabZombie && baseGameMode != nullptr)
	{
		if (baseGameMode->CanSpawnNewZombies())
		{
			AZombie* respawneable = baseGameMode->getRespawnableZombie();
			if (respawneable != nullptr)
			{
				baseGameMode->ZombieSpawned(true);
				respawneable->Respawn(GetActorLocation());
			}
			else
			{
				AZombie * zSpawned = GetWorld()->SpawnActor<AZombie>(prefabZombie, GetActorLocation(), GetActorRotation());
				baseGameMode->ZombieSpawned(false);
			}
		}
	}
}

void AMyActor_Spawner::RegisterToRespawn(AZombie * zombie)
{
	toRespawn.Add(zombie);
}
