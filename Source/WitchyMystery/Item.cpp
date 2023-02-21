// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "WitchyMysteryCharacter.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = ItemMesh;
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void AItem::AddToInventory(class AWitchyMysteryCharacter* Character)
{
	if (Character) 
	{
		Character->AddItemToInventoryWidget(ItemData);
	}
	Destroy();
}
