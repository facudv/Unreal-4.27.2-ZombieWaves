// Fill out your copyright notice in the Description page of Project Settings.

#include "Bullet.h"
#include "Zombie.h"

// Sets default values
ABullet::ABullet()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABullet::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimer(timerDestruction, this, &ABullet::SelfDestruction, 3.0f, false);
}

void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetActorLocation(GetActorLocation() + GetActorForwardVector() * speed * DeltaTime);
	if (toDestroy && setedOut)
	{
		GetWorld()->GetTimerManager().ClearTimer(timerDestruction);
		SelfDestruction();
	}
}

void ABullet::AddToBaseDamage(float damage)
{
	setedOut = true;
	dmg += damage;
}

void ABullet::MarkAsToDestroy()
{
	toDestroy = true;
}

void ABullet::SelfDestruction()
{
	Destroy();
}
