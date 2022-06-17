// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimI_Zombie.h"

void UAnimI_Zombie::ChangeAttackValue(bool value)
{
	Attaking = value;
}
void UAnimI_Zombie::ChangeHitValue(bool value)
{
	GetHited = value;
}
void UAnimI_Zombie::ChangeLifeValue(bool value)
{
	Die = value;
}

void UAnimI_Zombie::ChangeSpitValue(bool value)
{
	Spitting = value;
}

