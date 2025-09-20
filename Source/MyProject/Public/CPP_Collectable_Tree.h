// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Pickup.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPP_Collectable_Tree.generated.h"

UCLASS()
class MYPROJECT_API ACPP_Collectable_Tree : public AActor , public IPickup
{
	GENERATED_BODY()
public:
	FPickupReturn Pickup(AActor* pickupActor);
};
