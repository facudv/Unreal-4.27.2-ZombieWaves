// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "Engine/World.h"
#include "AnimI_Zombie.h"
#include "myPlayer.h"
#include "ZombieSpit.h"
#include "GameFramework/Actor.h"
#include "Zombie.generated.h"


class AZG_GameModeBase;

UENUM(BlueprintType)
enum class EBehaviours : uint8
{
	Follow UMETA(DisplayName = "FollowTarget"),
	LookTarget UMETA(DisplayName = "Look at Target"),
	Avoidance UMETA(DisplayName = "Avoid Obstacles"),
	Attack UMETA(DisplayName = "Attacking"),
	Spit UMETA(DisplayName = "Spitting")
};

UCLASS()
class ZOMBIEGAME_API AZombie : public AActor
{
	GENERATED_BODY()
	
public:	
	AZombie();

	UPROPERTY()
		UAudioComponent* myAudioComp;

	UPROPERTY(EditAnywhere)
		UParticleSystem * HitFeedback;
	UPROPERTY(EditAnywhere)
		UMaterialInterface * refMaterial;
	UPROPERTY(VisibleAnywhere)
		UMaterialInstanceDynamic * DynMaterial;
	UPROPERTY()
		bool fadeBody = false;
	UPROPERTY()
		bool bDie = false;
	UPROPERTY(BlueprintReadWrite)
		UUserWidget * lifeBarWidget;

	UPROPERTY(EditAnywhere)
		USoundCue* dieCue;
	UPROPERTY(EditAnywhere)
		USoundCue * attackCue;
	UPROPERTY(EditAnywhere)
		USoundCue* chaseCue;

	UPROPERTY(VisibleAnywhere)
		AActor * target;

	UPROPERTY(visibleAnywhere)
		EBehaviours currentBehaviour;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float AttackRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float SpitRangeMax;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float SpitRangeMin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float SpitCoolDownDuration = 6.0f;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float WeightAvoid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float attackDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float SpitDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float speedRot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		int Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Stats")
		float HeathPercent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "BodyCollision")
		float DamageForHitTorso = 15;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "BodyCollision")
		float DamageForHitLeftArm = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "BodyCollision")
		float DamageForHitRightArm = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "BodyCollision")
		int pointsForHitTorso = 15;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "BodyCollision")
		int pointsForHitLeftArm = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "BodyCollision")
		int pointsForHitRightArm = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Controllers")
		bool IsAttacking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Controllers")
		bool IsWalking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Controllers")
		bool IsDead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		AActor* closestObstacle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* SpitSpawner;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class AZombieSpit> prefabSpit;

	UPROPERTY(EditDefaultsOnly, Category = "Prefabs")
		TSubclassOf<AActor> prefab_healPack;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
		UCapsuleComponent* myCapsuleCollider;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
		UBoxComponent* myBoxCollider1;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
		UBoxComponent* myBoxCollider2;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
		USphereComponent* mySphereCollider;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
		TArray<USceneComponent*> myCollidersZombie;


	UPROPERTY()
		UAnimI_Zombie* _anim;
	UPROPERTY()
		AZG_GameModeBase* _gamemode;

	FTimerHandle timerDead;
	FTimerHandle timerSpit;
	FTimerHandle timerSpitCooldown;
	FTimerHandle timerStartDisolve;
	void SetDisolveOn();

protected:
	virtual void BeginPlay() override;

	int respawns = 1;
	float _initialHealth;

	FVector dir;

	float dist = 0;

	float timeAttack = 0;

	bool AttackPerform = false;

	bool spitting = false;
	float spitCooldDown = 0;

	float distanceToTarget = 0;

public:	
	virtual void Tick(float DeltaTime) override;

	void FollowMyTarget(float deltaTime);
	void LookTowardsTarget();
	void AvoidanceObstacles(float deltaTime);
	void Attack(float deltaTime);
	void Spit();
	void spitEnded();
	void resetSpitCoolDown();
	void GetHit(int Damage);
	
	UFUNCTION(BlueprintCallable)
		void Die();
	UFUNCTION(BlueprintCallable)
		void Respawn(FVector respawnPosition);

	void DestroyDead();
	void raycastAttack();

	UFUNCTION(BlueprintCallable)
		void myBeginOverlap(AActor * ActorOverlap);
	
	UFUNCTION(BlueprintCallable)
		void myEndOverlap(AActor* ActorOverlap);

	UFUNCTION(BlueprintCallable)
		void BodyCollisionHandler(AActor* other, int bodyPart);

};
