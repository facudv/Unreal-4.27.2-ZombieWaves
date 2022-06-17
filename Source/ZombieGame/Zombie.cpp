// Fill out your copyright notice in the Description page of Project Settings.

#include "Zombie.h"
#include "ZG_GameModeBase.h"
#include "Math/UnrealMathUtility.h"

AZombie::AZombie()
{
	PrimaryActorTick.bCanEverTick = true;
}


void AZombie::BeginPlay()
{
	Super::BeginPlay();

	target = GetWorld()->GetFirstPlayerController()->GetPawn();
	currentBehaviour = EBehaviours::Avoidance;
	timeAttack = attackDuration;
	_anim = Cast<UAnimI_Zombie>(FindComponentByClass<USkeletalMeshComponent>()->GetAnimInstance());
	myAudioComp = FindComponentByClass<UAudioComponent>();
	_gamemode = Cast<AZG_GameModeBase>(GetWorld()->GetAuthGameMode());
	spitCooldDown = 0;

	_initialHealth = Health;

	if (refMaterial)
	{
		DynMaterial = UMaterialInstanceDynamic::Create(refMaterial, this);
		USkeletalMeshComponent* mySkeleton = FindComponentByClass<USkeletalMeshComponent>();
		if (mySkeleton && DynMaterial)
		{
			mySkeleton->SetMaterial(0, DynMaterial);
			mySkeleton->SetMaterial(1, DynMaterial);
			mySkeleton->SetMaterial(2, DynMaterial);
		}
	}
}


void AZombie::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (fadeBody && DynMaterial)
	{
		float currentValue = 0;
		DynMaterial->GetScalarParameterValue(FName(TEXT("Dissolve")), currentValue);
		currentValue -= DeltaTime;
		DynMaterial->SetScalarParameterValue("Dissolve", currentValue);
	}

	dir = target->GetActorLocation() - GetActorLocation();
	if (!IsDead && !_anim->GetHited)
	{
		switch (currentBehaviour)
		{
		case EBehaviours::Follow:
			FollowMyTarget(DeltaTime);
			break;
		case EBehaviours::LookTarget:
			LookTowardsTarget();
			break;
		case EBehaviours::Avoidance:
			AvoidanceObstacles(DeltaTime);
			break;
		case EBehaviours::Attack:
			Attack(DeltaTime);
			break;
		case EBehaviours::Spit:
			Spit();
			break;
		}

		if (currentBehaviour == EBehaviours::Follow || currentBehaviour == EBehaviours::Avoidance)
		{
			if (myAudioComp && chaseCue && myAudioComp->Sound != chaseCue)
			{
				myAudioComp->Stop();
				myAudioComp->SetSound(chaseCue);
				myAudioComp->Play();
			}
		}

		distanceToTarget = dir.Size();

		if (spitCooldDown <= 0 && !spitting)
		{
			if (distanceToTarget < SpitRangeMax && distanceToTarget > SpitRangeMin)
			{
				int result = rand() % 5 + 1;
				if (result == 2)
					Spit();
			}
		}
	}
}

void AZombie::FollowMyTarget(float deltaTime)
{
	LookTowardsTarget();

	if (closestObstacle)
	{
		currentBehaviour = EBehaviours::Avoidance;
	}

	if (distanceToTarget <= AttackRange)
	{
		AttackPerform = false;
		timeAttack = attackDuration;
		currentBehaviour = EBehaviours::Attack;
	}
	else
	{
		SetActorLocation(GetActorLocation() + GetActorForwardVector() * MovementSpeed * deltaTime);
	}
}

void AZombie::LookTowardsTarget()
{
	SetActorRotation(dir.Rotation());
}

void AZombie::Spit()
{
	currentBehaviour = EBehaviours::Spit;
	spitCooldDown = SpitCoolDownDuration;
	LookTowardsTarget();

	if (!spitting)
	{
		spitting = true;

		if (_anim)
			_anim->Spitting = true;

		if (SpitSpawner)
		{
			FTransform position = SpitSpawner->GetComponentTransform();
			GetWorld()->SpawnActor<AZombieSpit>(prefabSpit, position);
		}
		else UE_LOG(LogTemp, Warning, TEXT("El componente no esta seteado"));

		GetWorld()->GetTimerManager().SetTimer(timerSpit, this, &AZombie::spitEnded, SpitDuration, false);
	}
}

void AZombie::spitEnded()
{
	spitting = false;

	if (_anim)
		_anim->Spitting = false;

	currentBehaviour = EBehaviours::Follow;
	GetWorld()->GetTimerManager().SetTimer(timerSpitCooldown, this, &AZombie::resetSpitCoolDown, spitCooldDown, false);
}

void AZombie::resetSpitCoolDown()
{
	spitCooldDown = 0;
}

void AZombie::AvoidanceObstacles(float deltaTime)
{
	if (!closestObstacle)
	{
		currentBehaviour = EBehaviours::Follow;
		return;
	}
	FVector direction = GetActorLocation() - closestObstacle->GetActorLocation();
	direction = (target->GetActorLocation() - GetActorLocation()).GetSafeNormal() + (direction.GetSafeNormal() * WeightAvoid);

	FVector Rot = FMath::Lerp(GetActorForwardVector(), direction, deltaTime * speedRot);

	Rot.Z = 0;

	SetActorRotation(Rot.Rotation());
	SetActorLocation(GetActorLocation() + GetActorForwardVector() * MovementSpeed * deltaTime);

	if (distanceToTarget <= AttackRange)
	{
		AttackPerform = false;
		timeAttack = attackDuration;
		currentBehaviour = EBehaviours::Attack;
	}
}

void AZombie::Attack(float deltaTime)
{
	LookTowardsTarget();
	timeAttack -= deltaTime;

	if (timeAttack <= 0.854f && AttackPerform == false)
	{
		AttackPerform = true;
		raycastAttack();
	}

	if (_anim != nullptr)
	{
		if (_anim->Attaking == false && timeAttack <= 0)
		{
			currentBehaviour = EBehaviours::Follow;
			return;
		}
	}

	_anim->ChangeAttackValue(true);
}

void AZombie::GetHit(int damage)
{
	if (Health > 0)
	{
		Health -= damage;
		_anim->ChangeHitValue(true);
		HeathPercent = Health / 100.0f;

		if (Health <= 0)
		{
			Health = 0;
			Die();
		}
	}
}

void AZombie::Die()
{
	FActorSpawnParameters SpawnParams;
	myBoxCollider1->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	myBoxCollider2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	myCapsuleCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	mySphereCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	int myRandNum = FMath::RandRange(1, 9);
	switch (myRandNum)
	{
		case 2 :
		{
			AActor * SpawnedActorRef = GetWorld()->SpawnActor<AActor>(prefab_healPack, GetActorTransform(), SpawnParams);
			break;
		}
		case 4 :
		{
			AActor* SpawnedActorRef = GetWorld()->SpawnActor<AActor>(prefab_healPack, GetActorTransform(), SpawnParams);
			break;
		}
		case 8 :
		{
			AActor* SpawnedActorRef = GetWorld()->SpawnActor<AActor>(prefab_healPack, GetActorTransform(), SpawnParams);
			break;
		}
		default:
			UE_LOG(LogTemp, Warning, TEXT("no spawn heal"));
			break;
	}

	bDie = true;
	if (myAudioComp)
	{
		myAudioComp->Stop();

		if (dieCue)
		{
			myAudioComp->SetSound(dieCue);
			myAudioComp->Play();
		}

		if (lifeBarWidget)
			lifeBarWidget->SetVisibility(ESlateVisibility::Hidden);
		if (_gamemode != nullptr) _gamemode->ZombieDied(respawns > 0);
	}

	_anim->ChangeLifeValue(true);
	IsDead = true;
	_gamemode->AddPoints(100);
	GetWorld()->GetTimerManager().SetTimer(timerStartDisolve, this, &AZombie::SetDisolveOn, 7.0, false);
	GetWorld()->GetTimerManager().SetTimer(timerDead, this, &AZombie::DestroyDead, 10.0f, false);
}
void AZombie::Respawn(FVector respawnPosition)
{
	SetActorLocation(respawnPosition);
	Health = _initialHealth;
	HeathPercent = 1.0f;
	IsDead = false;
	fadeBody = false;
	currentBehaviour = EBehaviours::Follow;

	if (lifeBarWidget)
		lifeBarWidget->SetVisibility(ESlateVisibility::Visible);

	if (_anim)
	{
		_anim->Die = false;
		_anim->Spitting = false;
		_anim->GetHited = false;
		_anim->Attaking = false;
	}

	if (DynMaterial)
		DynMaterial->SetScalarParameterValue("Dissolve", 1.6f);
}

void AZombie::SetDisolveOn()
{
	fadeBody = true;
}

void AZombie::DestroyDead()
{
	if (respawns > 0)
	{
		respawns--;
		_gamemode->registerZombieToRespawnList(this);
	}
	else Destroy();
}


void AZombie::raycastAttack()
{
	if (distanceToTarget <= AttackRange + 100)
	{
		if (myAudioComp && attackCue)
		{
			myAudioComp->Stop();
			myAudioComp->SetSound(attackCue);
			myAudioComp->Play();
		}

		AmyPlayer* CharacterHit = Cast<AmyPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
		if (CharacterHit != nullptr)
		{
			CharacterHit->GetHit(Damage);
		}
	}
}


void AZombie::myBeginOverlap(AActor* ActorOverlap)
{
	if (ActorOverlap == this || ActorOverlap == target)
		return;

	if (closestObstacle)
	{
		FVector DistA = closestObstacle->GetActorLocation() - GetActorLocation();
		FVector DistB = ActorOverlap->GetActorLocation() - GetActorLocation();
		if (DistA.Size() > DistB.Size())
			closestObstacle = ActorOverlap;
	}
	else
		closestObstacle = ActorOverlap;
}

void AZombie::myEndOverlap(AActor* ActorOverlap)
{
	if (ActorOverlap == closestObstacle)
	{
		closestObstacle = NULL;
	}
}

void AZombie::BodyCollisionHandler(AActor* other, int bodyPart)
{
	ABullet* bullet = Cast<ABullet>(other);
	if (bullet == nullptr) return;

	FString Part = "";
	if (Health > 0)
	{
		switch (bodyPart)
		{
		case 0:
			Part = "Torso";
			if (_gamemode)
				_gamemode->AddPoints(pointsForHitTorso);
			GetHit(DamageForHitTorso + bullet->dmg);
			break;

		case  1:
			Part = "Left Arm";
			if (_gamemode)
				_gamemode->AddPoints(pointsForHitLeftArm);
			GetHit(DamageForHitLeftArm + bullet->dmg);
			break;

		case 2:
			Part = "Right Arm";
			if (_gamemode)
				_gamemode->AddPoints(pointsForHitRightArm);
			GetHit(DamageForHitRightArm + bullet->dmg);
			break;

		default:
			Part = "Invalid!";
			break;
		}
	}
	
	FVector playerLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	FVector dirToPlayer = (playerLocation - this->GetActorLocation());
	
	FRotator rotation = dirToPlayer.Rotation();
	FVector position = bullet->GetTransform().GetLocation();

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitFeedback, position, rotation, true);

	bullet->MarkAsToDestroy();
}

