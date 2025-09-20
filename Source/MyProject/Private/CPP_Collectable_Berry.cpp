// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_Collectable_Berry.h"
#include <string>

ACPP_Collectable_Berry::ACPP_Collectable_Berry()
{
	PrimaryActorTick.bCanEverTick = true;
}

FPickupReturn ACPP_Collectable_Berry::Pickup(AActor* pickupActor) {
	int32 berriesCollected = 0;
	for (const TPair<UStaticMeshComponent*, bool>& Pair : Berries) {
		if (Pair.Value) {
			Berries[Pair.Key] = false;
			Pair.Key->SetMaterial(0, CollectedBerryMat);
			berriesCollected = 1;
			regrowing = true;
			break;
		}
	}

	return FPickupReturn(EPickupType::Berry, berriesCollected);
}

void ACPP_Collectable_Berry::GrowBerries()
{
	for (const TPair<UStaticMeshComponent*, bool>& Pair : Berries) {
		UStaticMeshComponent* staticMesh = Pair.Key;
		staticMesh->SetMaterial(0, GoodBerryMat);
		Berries[Pair.Key] = true;
	}
}

void ACPP_Collectable_Berry::RegrowTimeline(float value)
{
	regrowCounter += value;
	if (regrowCounter >= regrowTime) {
		regrowing = false;
		regrowCounter = 0;
		GrowBerries();
	}
}

void ACPP_Collectable_Berry::Tick(float DeltaTime)
{
	if (regrowing)
		RegrowTimeline(DeltaTime);
}
