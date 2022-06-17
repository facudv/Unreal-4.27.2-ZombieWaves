// Fill out your copyright notice in the Description page of Project Settings.

#include "ZombieSpit.h"
#include "myPlayer.h"

AZombieSpit::AZombieSpit()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AZombieSpit::BeginPlay()
{
	Super::BeginPlay();
	
}

void AZombieSpit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetActorLocation(GetActorLocation() + GetActorForwardVector() * speed * DeltaTime);
}

void AZombieSpit::SelfDestruction()
{
}

void AZombieSpit::CheckCollision(AActor* otherActor, UPrimitiveComponent* compCollision)
{
	auto P = Cast<AmyPlayer>(otherActor);
	if (P != nullptr)
	{
		P->GetSlowed();
		P->GetHit(5);
		Destroy();
	}
}

