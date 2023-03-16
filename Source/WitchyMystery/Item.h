// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableInterface.h"
#include "WitchyMystery.h"
#include "Public/Structs.h"
#include "Item.generated.h"

UCLASS()
class WITCHYMYSTERY_API AItem : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Item Data")
		class UStaticMeshComponent* ItemMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Item Data")
		FPickUpItem ItemData;

	UPROPERTY(EditDefaultsOnly, Category = "Item Data")
		int PuzzleNumber;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void AddToInventory(class AWitchyMysteryCharacter* Character) override;

	FPickUpItem GetItemData() const { return ItemData; }

	FString IsUsed() const { return FString("The item was used..."); }
};
