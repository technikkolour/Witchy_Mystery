// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "WizardAIController.generated.h"

/**
 * 
 */
UCLASS()
class WITCHYMYSTERY_API AWizardAIController : public AAIController
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float Health;

	UFUNCTION(BlueprintCallable, Category = "Health")
		void TakeDamage(float Damage);

private:
	UPROPERTY(EditAnywhere)
		class UBehaviorTree* AIBehaviour;
};
