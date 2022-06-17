// Fill out your copyright notice in the Description page of Project Settings.


#include "ZG_GameInstance.h"

void UZG_GameInstance::ResetData()
{
	UE_LOG(LogTemp, Warning, TEXT("Data Reseted"));

	AcumulatedPoints = 0;
	TimeSurvived = 0;
}

