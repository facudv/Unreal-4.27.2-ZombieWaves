// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Engine.h"
#include "Bullet.h"
#include "BasePlayer_AnimInstance.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "Math/UnrealMathUtility.h"
#include "myPlayer.generated.h"

class AZG_GameModeBase;

UCLASS()
class ZOMBIEGAME_API AmyPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	AmyPlayer();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Variables:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player References", meta = (DisplayPriority=1))
		UBasePlayer_AnimInstance* animController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player References", meta = (DisplayPriority = 1))
		USceneComponent* TestLocator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (DisplayPriority = 2))
		int health = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (DisplayPriority = 2))
		int MaxHealth = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (DisplayPriority = 2))
		int Ammo = 15;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (DisplayPriority = 2))
		int AmmoInMagazine = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (DisplayPriority = 2))
		float BaseDamage = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (DisplayPriority = 2))
		int MaxAmmoInMagazine = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (DisplayPriority = 2))
		float ShootCadency = 0.1f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (DisplayPriority = 2))
		float MovementSpeed = 1.0f;
	
		

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class ABullet> prefabBullet;

	UPROPERTY()
		UAudioComponent* myAudioComp;
	UPROPERTY(EditAnyWhere)
		USoundCue* myShootCue;


private:

	UPROPERTY()
		AZG_GameModeBase* _gameMode;
	
	FTimerHandle _ShootCooldown;
	FTimerHandle _SlowCooldown;

	float CurrentSpeed;
	
	bool Slowed;

	void Shoot();


public:
	//Functions:

	void MoveHorizontal(float HAxis); 
	void MoveVertical(float VAxis);   
	void RotatePlayer(float rot);     
	void StartShooting();             
	void StopShooting();              
	void Jump();                      
	void Reload();                    
	void Die();                       
	void RespawnPlayer();             
	void GetAndLoadWeapon();          //Call when get weapon
	void GetHit(int Damage);
	void GetSlowed();
	void GetRecovered();
	
	UFUNCTION(BlueprintCallable)
		void DoCameraShake();

	UFUNCTION(BlueprintCallable)
		void Heal(int Ammount);

	UFUNCTION(BlueprintCallable)
		void AddBulletsToBackPack(int Bullets);

	UFUNCTION(BlueprintCallable)
		void AddExtraMagazineSlots(int extraBullets);

	UFUNCTION(BlueprintCallable)
		void AddExtraDamage(float extraDamage);

	UFUNCTION(BlueprintCallable)
		void ShootWithTransform(FTransform transform);

	UFUNCTION(BlueprintCallable)
		void ShootWithPositionAndRotation(FVector position, FRotator rotation);
};
