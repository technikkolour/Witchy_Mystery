// Fill out your copyright notice in the Description page of Project Settings.


#include "WizardAIController.h"

void AWizardAIController::TakeDamage(float Damage)
{
	if (Health >= Damage)
		Health -= Damage;
	else
		Health = 0;
}
