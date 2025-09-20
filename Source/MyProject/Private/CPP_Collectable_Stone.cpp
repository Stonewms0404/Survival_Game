// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_Collectable_Stone.h"


FPickupReturn ACPP_Collectable_Stone::Pickup(AActor* pickupActor)
{
	pickupActor->Destroy();
	FRandomStream* rand = new FRandomStream();
	return FPickupReturn(EPickupType::Stone, rand->RandRange(5, 20));
}
