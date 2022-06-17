// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnManagerBase.h"

// Sets default values
ASpawnManagerBase::ASpawnManagerBase()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ASpawnManagerBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASpawnManagerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpawnManagerBase::RegisterReespawneableZombie(AZombie* reespawneable)
{
	respawneables.Add(reespawneable);
}

//Return null if not available
AZombie* ASpawnManagerBase::getRespawneableZombie()
{
	if (respawneables.Num() > 0)
	{
		AZombie* toRespawnZ = respawneables[0];
		respawneables.RemoveAt(0);
		toRespawnZ->myBoxCollider1->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		toRespawnZ->myBoxCollider2->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		toRespawnZ->myCapsuleCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		toRespawnZ->mySphereCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		return toRespawnZ;
	}

	return nullptr;
}

