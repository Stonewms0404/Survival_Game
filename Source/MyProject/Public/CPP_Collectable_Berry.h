// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Pickup.h"
#include "Components/TimelineComponent.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPP_Collectable_Berry.generated.h"

class UCurveFloat;

/**
 * 
 */
UCLASS()
class MYPROJECT_API ACPP_Collectable_Berry : public AActor, public IPickup
{
	GENERATED_BODY()
	
public:
	ACPP_Collectable_Berry();
	FPickupReturn Pickup(AActor* pickupActor);
	void GrowBerries();
	void RegrowTimeline(float value);
	virtual void Tick(float DeltaTime) override;
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	TMap<UStaticMeshComponent*, bool> Berries;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	UMaterialInterface* GoodBerryMat;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	UMaterialInterface* CollectedBerryMat;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	UStaticMesh* BerryMesh;

	bool regrowing = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	float regrowTime = 10.0f;
	float regrowCounter = 0;
};
