// Fill out your copyright notice in the Description page of Project Settings.


#include "ZG_GameModeBase.h"
#include "myPlayer.h"

void AZG_GameModeBase::StartPlay()
{
	Super::StartPlay();
}

void AZG_GameModeBase::UpdateTrackedGameTime(float time)
{
	if (!lockTimeTrack)
		GameTime += time;
}

void AZG_GameModeBase::AddPoints(int points)
{
	this->Score += points;
}

void AZG_GameModeBase::AddPlayerLifes(int ExtraLifes)
{
	this->RemainingLifes += ExtraLifes;
}

void AZG_GameModeBase::PlayerDied()
{
	this->RemainingLifes--;

	if (this->RemainingLifes <= 0)
	{
		//Load data to GameInstance
		UZG_GameInstance* gameInstance = Cast<UZG_GameInstance>(GetWorld()->GetGameInstance());
		gameInstance->AcumulatedPoints = Score;
		gameInstance->TimeSurvived = GameTime;

		if (_playerHUDWigetInstance && _playerHUDWigetInstance->IsInViewport())//Remove wg from player
			_playerHUDWigetInstance->RemoveFromViewport();

		APlayerController* controller = GetWorld()->GetFirstPlayerController();
		controller->bShowMouseCursor = true;//show cursor
		controller->SetInputMode(FInputModeUIOnly());//Set Input to UI only

		if (_playerDefeatWigetInstance) //Show defeat wg
		{
			_playerDefeatWigetInstance->AddToViewport();
			if (_playerDefeatWigetInstance->playerDefeatWidgetAnimation)
				_playerDefeatWigetInstance->PlayAnimation(_playerDefeatWigetInstance->playerDefeatWidgetAnimation);
		}
	}
	else //respawn if die
	{
		if (_respawnWidgetInstance && !_respawnWidgetInstance->IsInViewport())
			_respawnWidgetInstance->AddToViewport();//Change hud to respawn timer
		GetWorld()->GetTimerManager().SetTimer(respawnFeedBack, this, &AZG_GameModeBase::RespawnCallBack, 1.0f, true, 1.0f);
	}
}

void AZG_GameModeBase::RespawnCallBack()
{
	//Rest one to preview;
	RespawnTimeRemaining--;
	
	if (RespawnTimeRemaining == 0)
	{
		GetWorld()->GetTimerManager().PauseTimer(respawnFeedBack);
		RespawnTimeRemaining = 5;
		RespawnPlayer();
	}
}

void AZG_GameModeBase::RespawnPlayer()
{
	//Respawn player.
	if (DefaultPlayerStart != nullptr)
	{
		AmyPlayer* player = Cast<AmyPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());

		if (player != nullptr)
		{
			player->SetActorLocation(DefaultPlayerStart->GetActorTransform().GetLocation());
			player->RespawnPlayer();
		}
	}
	if (_respawnWidgetInstance)
		_respawnWidgetInstance->RemoveFromParent();
}

void AZG_GameModeBase::AddPlayerHudToScene()
{
	if (_playerHUDWigetInstance == nullptr)
	{
		_playerHUDWigetInstance = CreateWidget(GetWorld(), PlayerHUDWidget);
		_playerHUDWigetInstance->AddToViewport();
	}
}

void AZG_GameModeBase::ZombieSpawned(bool isReespawning)
{
	_activeZombies++;
	TotalZombiesSpawnedInCurrentWave++;
	lockSpawning = !(TotalZombiesSpawnedInCurrentWave < zombiesToKill[(WaveNumber - 1)]);
}

void AZG_GameModeBase::ZombieDied(bool willRespawn)
{
	_activeZombies--;
	RemainingZombiesToEndCurrentWave--;
	if (RemainingZombiesToEndCurrentWave <= 0)
		WaveEnded();
}

bool AZG_GameModeBase::CanSpawnNewZombies()
{
	int waveIndex = WaveNumber - 1;
	bool canSpawn = _activeZombies < SceneSpawnLimit && RemainingZombiesToEndCurrentWave > 0 && !lockSpawning;
	
	return canSpawn;
}

void AZG_GameModeBase::WaveEnded()
{
	if (_playerHUDWigetInstance && _playerHUDWigetInstance->IsInViewport())
		_playerHUDWigetInstance->RemoveFromViewport();
	if (_waveCompleteWidgetInstance && !_waveCompleteWidgetInstance->IsInViewport())
		_waveCompleteWidgetInstance->AddToViewport();

	if (zombiesToKill.Num() - WaveNumber <= 0)
		LevelCompleted(); //Level completed
	else //Start count next wave
	{
		lockTimeTrack = true;
		TimeRemainingForNextWave = TimeBetweenWaves;
		GetWorld()->GetTimerManager().SetTimer(WaveStartCounter, this, &AZG_GameModeBase::WaveEndCallback, 1.0f, true, 1.0f);
	}
}

void AZG_GameModeBase::WaveEndCallback()
{
	TimeRemainingForNextWave--;
	if (TimeRemainingForNextWave == 0)
	{
		TimeRemainingForNextWave = TimeBetweenWaves;
		GetWorld()->GetTimerManager().ClearTimer(WaveStartCounter);
		WaveStart();
	}
}

void AZG_GameModeBase::WaveStart()
{
	WaveNumber++;			//Update wave
	lockSpawning = false;   //Unlock zombie spawn
	lockTimeTrack = false;	//Unlock timer track
	_activeZombies = 0;		//Update zombies
	TotalZombiesSpawnedInCurrentWave = 0;

	int currentWaveIndex = WaveNumber - 1;
	
	if (maxActiveZombiesInScenePerWave.Num() > 0)
		SceneSpawnLimit = maxActiveZombiesInScenePerWave[currentWaveIndex];	
	if (zombiesToKill.Num() > 0) //Update zombie amount to end the wave.
		RemainingZombiesToEndCurrentWave = zombiesToKill[currentWaveIndex];

	if (_levelStartWidgetInstance && _levelStartWidgetInstance->IsInViewport())
		_levelStartWidgetInstance->RemoveFromViewport();
	if (_playerHUDWigetInstance && !_playerHUDWigetInstance->IsInViewport())
		_playerHUDWigetInstance->AddToViewport();
	if (_waveCompleteWidgetInstance && _waveCompleteWidgetInstance->IsInViewport())
		_waveCompleteWidgetInstance->RemoveFromViewport();
}

void AZG_GameModeBase::LevelCompleted()
{
	lockTimeTrack = true;
	lockSpawning = true; //block spawn
	if (_levelEndWidgetInstance && !_levelEndWidgetInstance->IsInViewport()) //active wg end lvl
		_levelEndWidgetInstance->AddToViewport();
	if (_waveCompleteWidgetInstance && _waveCompleteWidgetInstance->IsInViewport())
		_waveCompleteWidgetInstance->RemoveFromViewport();

	TimeRemainingToExitLevel = LevelEndDisplayTime;
	GetWorld()->GetTimerManager().SetTimer(WaveFinishCounter, this, &AZG_GameModeBase::EndGameCallback, 1.0f, true, 0.0f);
}

void AZG_GameModeBase::EndGameCallback()
{
	TimeRemainingToExitLevel--;
	if (TimeRemainingToExitLevel <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(WaveFinishCounter);
		if (_levelEndWidgetInstance && _levelEndWidgetInstance->IsInViewport()) //desactive wg end lvl
			_levelEndWidgetInstance->RemoveFromViewport();
		if (_currentWorld)
		{
			//load data result in game instance
			UZG_GameInstance* gameInstance = Cast<UZG_GameInstance>(GetWorld()->GetGameInstance());
			gameInstance->AcumulatedPoints += Score;
			gameInstance->TimeSurvived += GameTime;
			UGameplayStatics::OpenLevel(GetWorld(), nextLevelPath, false);//Load next scene
		}
	}
}

void AZG_GameModeBase::SetLevelFlow(TArray<int> _ammountOfZombiesPerWave, TArray<int> _activeZombiesInScenePerWave)
{
	WaveNumber = 0;
	zombiesToKill = _ammountOfZombiesPerWave;
	maxActiveZombiesInScenePerWave = _activeZombiesInScenePerWave;

	_spawnManager = Cast<ASpawnManagerBase>(GetWorld()->SpawnActor(_spawnManagerClass, &FVector::ZeroVector, &FRotator::ZeroRotator));
}

void AZG_GameModeBase::StartLevel()
{
	//UE_LOG(LogTemp, Warning, TEXT("AZG_GameModeBase::StartLevel()"));
	_currentWorld = GetWorld();
	_currentLevel = _currentWorld->GetMapName();
	lockTimeTrack = false;
	lockSpawning = false;

	if (RespawnWidget && _respawnWidgetInstance == nullptr)
		_respawnWidgetInstance = CreateWidget(GetWorld(), RespawnWidget);
	if (PlayerHUDWidget && _playerHUDWigetInstance == nullptr)
		_playerHUDWigetInstance = CreateWidget(GetWorld(), PlayerHUDWidget);
	if (PlayerDefeatWidget && _playerDefeatWigetInstance == nullptr)
		_playerDefeatWigetInstance =  Cast<UPlayerDefeatWidget>(CreateWidget(GetWorld(), PlayerDefeatWidget));
	if (WaveCompleteWidget && _waveCompleteWidgetInstance == nullptr)
		_waveCompleteWidgetInstance = CreateWidget(GetWorld(), WaveCompleteWidget);
	if (LevelStartWidget && _levelStartWidgetInstance == nullptr)
		_levelStartWidgetInstance = CreateWidget(GetWorld(), LevelStartWidget);
	if (LevelEndWidget && _levelEndWidgetInstance == nullptr)
		_levelEndWidgetInstance = CreateWidget(GetWorld(), LevelEndWidget);

	if (_levelStartWidgetInstance)
	{
		_levelStartWidgetInstance->AddToViewport();
		TimeRemainingForNextWave = StartingWaveDelay;
		GetWorld()->GetTimerManager().SetTimer(WaveStartCounter, this, &AZG_GameModeBase::WaveEndCallback, 1.0f, true, 0.0f);
	}
}

void AZG_GameModeBase::LoadPreviousData(int acumulatedScore, float survivedTime)
{
	Score = acumulatedScore;
	GameTime = survivedTime;
}

AZombie* AZG_GameModeBase::getRespawnableZombie()
{
	if (_spawnManager)
		return _spawnManager->getRespawneableZombie();
	else
	{
		return nullptr;
	}
}

void AZG_GameModeBase::registerZombieToRespawnList(AZombie* reespawneable)
{
	if (_spawnManager)
		_spawnManager->RegisterReespawneableZombie(reespawneable);
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnManager no esta seteado"));
	}
}
