// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AttackSpell.generated.h"

UCLASS()
class WITCHYMYSTERY_API AAttackSpell : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAttackSpell();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spell Details")
		float DamageDealt;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spell Details")
		float ChanceOfOccuring;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spell Details")
		float Cooldown;

};
