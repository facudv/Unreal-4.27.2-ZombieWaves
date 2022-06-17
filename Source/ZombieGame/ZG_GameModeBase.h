// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include <Kismet/GameplayStatics.h>
#include "GameFramework/GameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "ZG_GameInstance.h"
#include "PlayerDefeatWidget.h"
#include "SpawnManagerBase.h"
#include "ZG_GameModeBase.generated.h"

class AmyPlayer;

/**
 * 
 */
UCLASS()
class ZOMBIEGAME_API AZG_GameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int RemainingLifes = 3;

	UPROPERTY(BlueprintReadOnly)
		int Score = 0;

	UPROPERTY(BlueprintReadOnly)
		float GameTime = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		AActor* DefaultPlayerStart;

	UPROPERTY(BlueprintReadOnly)
		int RespawnTimeRemaining = 5;  //CountDown to respawn player.

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class UUserWidget> LevelStartWidget; //HUD Start lvl.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class UUserWidget> LevelEndWidget; //HUD Final lvl.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class UUserWidget> RespawnWidget;  //HUD player die.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class UPlayerDefeatWidget> PlayerDefeatWidget;  //HUD player die 3 times
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class UUserWidget> WaveCompleteWidget; //HUD player complete wave
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class UUserWidget> PlayerHUDWidget; //Hud player.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class ASpawnManagerBase> _spawnManagerClass;
	UPROPERTY(EditAnywhere)
		ASpawnManagerBase* _spawnManager;

	UPROPERTY(BlueprintReadOnly, category = "GameFlow")
		int WaveNumber = 0;                       //Num wave.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "GameFlow")
		int RemainingZombiesToEndCurrentWave = 0; //Zombies left to actual wave ends.
	UPROPERTY(BlueprintReadOnly, category = "GameFlow")
		int TotalZombiesSpawnedInCurrentWave = 0; //zombies spawned since wave started.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "GameFlow")
		int SceneSpawnLimit = 200;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TArray<int> zombiesToKill; //zombie amount to complete wave. Use .Num() to get count!.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TArray<int> maxActiveZombiesInScenePerWave; //Max limit zombies active

	//Wave Flow.
	UPROPERTY(EditAnywhere)
		float TimeBetweenWaves = 10.0f; 
	UPROPERTY(BlueprintReadOnly)
		int StartingWaveDelay = 5; 
	UPROPERTY(BlueprintReadOnly)
		int TimeRemainingForNextWave = 0;

	//Level Flow.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName nextLevelPath = "";
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int LevelStartDisplayTime = 10;//Time to start game(sec)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int LevelEndDisplayTime = 10;  //Timer show result before exit lvl(sec)
	UPROPERTY(BlueprintReadOnly)
		int TimeRemainingToExitLevel = 0;//CountDown before exit lvl

private:

	UPROPERTY()
		UWorld* _currentWorld;
	UPROPERTY()
		UUserWidget* _levelStartWidgetInstance;
	UPROPERTY()
		UUserWidget* _levelEndWidgetInstance;
	UPROPERTY()
		UUserWidget* _respawnWidgetInstance;
	UPROPERTY()
		UUserWidget* _playerHUDWigetInstance;
	UPROPERTY()
		UPlayerDefeatWidget* _playerDefeatWigetInstance;
	UPROPERTY()
		UUserWidget* _waveCompleteWidgetInstance;

	int _activeZombies = 0; //Actives zombies on lvl
	bool lockTimeTrack = true; 
	UPROPERTY(EditAnywhere)
		bool lockSpawning = true; //Block spawn.

	FString _currentLevel;
	FTimerHandle respawnFeedBack;
	FTimerHandle WaveStartCounter;
	FTimerHandle WaveFinishCounter;

public:
	void StartPlay() override;

	UFUNCTION(BlueprintCallable)
		void AddPlayerHudToScene();
	UFUNCTION(BlueprintCallable)
		void AddPoints(int points);

	void AddPlayerLifes(int ExtraLifes);
	void UpdateTrackedGameTime(float time);

	void PlayerDied();
	void RespawnCallBack();
	void RespawnPlayer();

	UFUNCTION(BlueprintCallable)
		void WaveStart();//Called when lvl start and when wave ends successfull
	void WaveEndCallback(); //count before start new wave
	void WaveEnded(); //Called when last zombie of actual wave dies
	
	void ZombieDied(bool willRespawn);     //Register action die
	void ZombieSpawned(bool isReespawning);//Register action spawn
	bool CanSpawnNewZombies();
	AZombie* getRespawnableZombie();   //Return nullptr if respawnManager doesn't has zomb
	void registerZombieToRespawnList(AZombie* reespawneable);//Register in reespawn manag one zombie to respawn

	UFUNCTION(BlueprintCallable)
		void SetLevelFlow(TArray<int> _ammountOfZombiesToKill, TArray<int> _maxActiveZombiesInScene); //When level start can set this params
	UFUNCTION(BlueprintCallable)
		void StartLevel(); //Level BP calls when start lvl 
	UFUNCTION(BlueprintCallable)
		void LoadPreviousData(int acumulatedScore, float survivedTime); //Load in the next lvl 

	void LevelCompleted(); //Call wen ends all waves
	void EndGameCallback(); //Call for 1 sec after ends wave
	
};
