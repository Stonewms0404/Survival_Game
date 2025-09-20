// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_Collectable_Tree.h"


FPickupReturn ACPP_Collectable_Tree::Pickup(AActor* pickupActor)
{
	pickupActor->Destroy();
	FRandomStream* rand = new FRandomStream();
	return FPickupReturn(EPickupType::Wood, rand->RandRange(15, 50));
}
