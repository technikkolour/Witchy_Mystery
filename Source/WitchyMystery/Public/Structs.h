#pragma once

#include "Structs.generated.h"

USTRUCT(BlueprintType)
struct FPickUpItem {
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		TSubclassOf<class AItem> ItemClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		class UTexture2D* ItemImage;
};