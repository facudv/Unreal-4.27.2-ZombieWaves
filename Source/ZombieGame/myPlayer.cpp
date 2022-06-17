// Fill out your copyright notice in the Description page of Project Settings.

#include "myPlayer.h"
#include "ZG_GameModeBase.h"

// Sets default values
AmyPlayer::AmyPlayer()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AmyPlayer::BeginPlay()
{
	Super::BeginPlay();

	Slowed = false;
	CurrentSpeed = MovementSpeed;

	USkeletalMeshComponent* mySkeletalMesh = FindComponentByClass<USkeletalMeshComponent>();
	if (mySkeletalMesh)
	{
		this->animController = Cast<UBasePlayer_AnimInstance>(mySkeletalMesh->GetAnimInstance());
		if (animController)
		{
			animController->hasWeapon = true;
		}
	}

	auto gm = Cast<AZG_GameModeBase>(GetWorld()->GetAuthGameMode());
	if (gm != nullptr)
	{
		_gameMode = gm;
	}

	myAudioComp = FindComponentByClass<UAudioComponent>();
}

void AmyPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (health > 0)
		_gameMode->UpdateTrackedGameTime(DeltaTime);
}

// Called to bind functionality to input
void AmyPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AmyPlayer::Jump);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AmyPlayer::Reload);
	PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &AmyPlayer::StartShooting);
	PlayerInputComponent->BindAction("Shoot", IE_Released, this, &AmyPlayer::StopShooting);

	PlayerInputComponent->BindAxis("Horizontal", this, &AmyPlayer::MoveHorizontal);
	PlayerInputComponent->BindAxis("Vertical", this, &AmyPlayer::MoveVertical);
	PlayerInputComponent->BindAxis("RotatePlayer", this, &AmyPlayer::RotatePlayer);
}

void AmyPlayer::MoveHorizontal(float HAxis)
{
	if (health > 0)
	{
		this->animController->horizontalAxis = HAxis;
		AddMovementInput(GetActorRightVector(), HAxis* CurrentSpeed);
	}
}

void AmyPlayer::DoCameraShake()
{
	FOutputDeviceNull ar;
	this->CallFunctionByNameWithArguments(TEXT("myCameraShake"), ar, NULL, true);
}

void AmyPlayer::MoveVertical(float VAxis)
{
	if (health > 0)
	{
		this->animController->verticalAxis = VAxis;
		AddMovementInput(GetActorForwardVector(), VAxis* CurrentSpeed);
	}
}

void AmyPlayer::RotatePlayer(float rot)
{
	if (health > 0)
		AddControllerYawInput(rot);
}

void AmyPlayer::StartShooting()
{
	GetWorld()->GetTimerManager().SetTimer(_ShootCooldown,this, &AmyPlayer::Shoot, ShootCadency, true, 0.0f);
}

void AmyPlayer::Shoot()
{
	if (this->TestLocator != nullptr && health > 0)
	{
		FTransform position = this->TestLocator->GetComponentTransform();

		if (AmmoInMagazine > 0)
		{
			UWorld* const World = GetWorld();
			if (World != NULL && prefabBullet != NULL)
			{
				FActorSpawnParameters spawnParams;
				spawnParams.Owner = GetOwner();

				ABullet* bullet = World->SpawnActor<ABullet>(prefabBullet, position);
				bullet->AddToBaseDamage(BaseDamage);

				if (this->animController != NULL)
				{
					this->animController->isShooting = true;
				}
				if (myAudioComp)
				{
					myAudioComp->Stop();

					if (myShootCue)
						myAudioComp->SetSound(myShootCue);
					myAudioComp->Play();
				}
			}
		}
	}
}

void AmyPlayer::StopShooting()
{
	GetWorld()->GetTimerManager().ClearTimer(_ShootCooldown);
	if (this->animController != NULL)
	{
		this->animController->isShooting = false;
	}
}

void AmyPlayer::ShootWithTransform(FTransform transform)
{
	if (AmmoInMagazine > 0)
	{
		UWorld* const World = GetWorld();
		if (World != NULL && prefabBullet != NULL)
		{
			FActorSpawnParameters spawnParams;
			spawnParams.Owner = GetOwner();

			ABullet * bullet = World->SpawnActor<ABullet>(prefabBullet, transform);
			
			if (this->animController != NULL)
			{
				this->animController->isShooting = true;
			}
		}
	}
}

void AmyPlayer::ShootWithPositionAndRotation(FVector position, FRotator rotation)
{
	if (AmmoInMagazine > 0)
	{
		UWorld* const World = GetWorld();
		if (World != NULL && prefabBullet != NULL)
		{
			FActorSpawnParameters spawnParams;
			spawnParams.Owner = GetOwner();

			ABullet * bullet = World->SpawnActor<ABullet>(prefabBullet, position, rotation, spawnParams);
		}
	}
}

void AmyPlayer::Jump()
{
	//UE_LOG(LogTemp, Warning, TEXT("Jump"));
}

void AmyPlayer::Reload()
{
	if (Ammo <= 0) 
	{
		return;
	}

	if (AmmoInMagazine < MaxAmmoInMagazine)
	{
		int faltantes = MaxAmmoInMagazine - AmmoInMagazine;
		int añadibles = 0;
		
		if (Ammo > 0 && Ammo > faltantes) 
			añadibles += faltantes;

		if (Ammo > 0 && Ammo <= faltantes)
			añadibles = Ammo;

		Ammo -= añadibles;
	}
}

void AmyPlayer::AddBulletsToBackPack(int Bullets)
{
	Ammo += Bullets;
}

void AmyPlayer::Heal(int Ammount)
{
	health += Ammount;
	if (health > MaxHealth)
		health = MaxHealth;
}

void AmyPlayer::AddExtraMagazineSlots(int extraBullets)
{
	MaxAmmoInMagazine += extraBullets;
}

void AmyPlayer::Die() 
{
	if (animController != nullptr)
	{
		animController->isDead = true;
	}

	if (_gameMode)
	{
		_gameMode->PlayerDied();
	}
}

void AmyPlayer::RespawnPlayer()
{
	if (animController != nullptr)
	{
		animController->isDead = false;
		health = MaxHealth;
	}
}

void AmyPlayer::GetAndLoadWeapon()
{
	if (this->animController)
	{
		this->animController->hasWeapon = true;
	}
}

void AmyPlayer::GetHit(int Damage)
{
	if (health > 0)
	{
		this->AmyPlayer::DoCameraShake();
		health -= Damage;

		if (health <= 0)
		{
			health = 0;
			Die();
		}
	}
}

void AmyPlayer::GetSlowed()
{
	if (!Slowed)
	{
		CurrentSpeed = MovementSpeed / 10;
		Slowed = true;
		GetWorld()->GetTimerManager().SetTimer(_SlowCooldown, this, &AmyPlayer::GetRecovered, 4.0f, false);
	}
}

void AmyPlayer::GetRecovered()
{
	CurrentSpeed = MovementSpeed;
	Slowed = false;
}

void AmyPlayer::AddExtraDamage(float extraDamage)
{
	BaseDamage += extraDamage;
}
