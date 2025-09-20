// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CPP_PickupType.h"

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Pickup.generated.h"

USTRUCT(BlueprintType)
struct FPickupReturn {
	GENERATED_BODY()
public:
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Health")
	EPickupType Type = EPickupType::Berry;
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Health")
	int32 Quantity = 0;
};

/**
 *
 */
UINTERFACE(MinimalAPI, Blueprintable)
class UPickup : public UInterface
{
	GENERATED_BODY()
};

class IPickup {
	GENERATED_BODY()
public:
	virtual FPickupReturn Pickup(AActor* pickupActor) = 0;
};
