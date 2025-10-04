// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Pickup.h"

void ACPP_Character::StaminaLoss(float DeltaTime)
{
	IsMoving = abs(Capsule->GetComponentVelocity().Length()) > 1.0f;
	if (ForceStaminaCharge || !(IsSprinting && IsMoving)) return;
	StaminaLossCounter += DeltaTime;
	if (StaminaLossCounter < 0.25f) return;
	StaminaLossCounter = 0.0f;
	DepleteStamina(0.5f);
}

void ACPP_Character::StaminaHealPlayer()
{
	if (Health >= 20.0f) return;
	if (Stamina >= 2.0f) {
		DepleteHealth(-1.0f);
		DepleteStamina(2.0f);
	}
	else {
		if (Hunger >= 1.0f)
			DepleteHunger(1.0f);
	}
}

void ACPP_Character::EatFood()
{
	ConsumeFood(3.0f);
}

void ACPP_Character::StaminaRegain(float DeltaTime)
{
	if (ForceStaminaCharge) return;
	if (IsSprinting && IsMoving) return;
	if (CanRegenerateStamina && IsMoving && Stamina <= 20.0f) {
		StaminaRegainCounter += DeltaTime;
		if (StaminaRegainCounter < 0.5f) return;
	}
	else {
		StaminaRegainCounter += DeltaTime;
		if (StaminaRegainCounter < 0.25f) return;
	}
	StaminaRegainCounter = 0.0f;
	if (Hunger <= 0.0f || Stamina >= 20) return;
	float regainedStamina = -1.0f;
	if (Stamina > 19.0f) regainedStamina = Stamina - 20.0f;
	DepleteStamina(regainedStamina);
	DepleteHunger(-regainedStamina / 4.0f);
}

void ACPP_Character::DepleteStamina(float StaminaCost)
{
	Stamina -= StaminaCost;
	if (StaminaCost <= 0)
		CheckStamina();
	else
		CanRegenerateStamina = true;
	StaminaChanged.Broadcast(Stamina);
}

void ACPP_Character::ForceStaminaChargeFunc()
{
	if (ForceStaminaCharge) return;
	CanRegenerateStamina = false;
	Stamina = 0.0f;
	ForceStaminaCharge = true;
	CanSprint = false;
	CheckStamina();
}

void ACPP_Character::CheckStamina()
{
	if (Stamina <= 0.0f && !ForceStaminaCharge)
		ForceStaminaChargeFunc();
}

void ACPP_Character::CheckHunger()
{
	if (Hunger <= 0.0f) {
		Hunger = 0.0f;
		DepleteHealth(1.0f);
		isHungry = true;
	}
	else if (Hunger >= 20.0f) {
		Hunger = 20.0f;
		isHungry = false;
	}
	else {
		isHungry = false;
	}
	HungerChanged.Broadcast(Hunger);
}

void ACPP_Character::StartJumping()
{
	JumpPressed = true;
}

void ACPP_Character::EndJumping()
{
	JumpPressed = false;
}

void ACPP_Character::MoveForward(float AxisValue)
{
	GetCharacterMovement()->MaxWalkSpeed = IsSprinting && CanSprint ? 1200.0f : 600.0f;
	FVector Direction = FRotationMatrix(GetArrowComponent()->GetComponentRotation()).GetScaledAxis(EAxis::X);
	AddMovementInput(Direction, AxisValue);
}

void ACPP_Character::MoveRight(float AxisValue)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	AddMovementInput(Direction, AxisValue);
}

void ACPP_Character::FindObject()
{
	FVector startLocation = GetMesh()->GetBoneLocation("head");
	FVector endLocation = startLocation + PlayerCam->GetForwardVector() * 400.0f;
	
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;

	if (!isBuilding) {
		bool hit = GetWorld()->LineTraceSingleByChannel(
			HitResult,
			startLocation,
			endLocation,
			ECC_WorldDynamic,
			QueryParams
		);

		/*
		DrawDebugLine(
			GetWorld(),            // The UWorld instance
			startLocation,         // FVector for the line's start point
			endLocation,           // FVector for the line's end point
			hit ? FColor::Green : FColor::Red,                 // FColor for the line's color
			true,      // bool: true for persistent lines, false for temporary
			-1,              // float: duration in seconds if not persistent (-1 for indefinite)
			0,         // uint8: rendering priority (0 for default)
			2.0f              // float: line thickness
		);
		*/

		if (hit) {
			AActor* hitActor = HitResult.GetActor();
			UClass* uClass = hitActor->GetClass();
			bool isImplemented = uClass->ImplementsInterface(UPickup::StaticClass());
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, isImplemented ? FColor::Cyan : FColor::Red,
			///	uClass->GetAuthoredName() + FString(" " + isImplemented ? "true" : "false"));
			if (isImplemented) {
				IPickup* pickup = Cast<IPickup, AActor>(hitActor);
				FPickupReturn pickupReturn = pickup->Pickup(hitActor);
				switch (pickupReturn.Type) {
				case EPickupType::Berry:
					Inventory.Berries += pickupReturn.Quantity;
					break;
				case EPickupType::Stone:
					Inventory.Stone += pickupReturn.Quantity;
					break;
				case EPickupType::Wood:
					Inventory.Wood += pickupReturn.Quantity;
					break;
				}
				matsCollected += pickupReturn.Quantity;
				objWidget->UpdatematOBJ(matsCollected);
				InventoryChanged.Broadcast(Inventory);
			}
		}
	}
	else {
		isBuilding = false;

		objectsBuilt++;
		objWidget->UpdatebuildOBJ(objectsBuilt);
	}
}

void ACPP_Character::StartSprinting()
{
	IsSprinting = true;
}

void ACPP_Character::EndSprinting()
{
	IsSprinting = false;
}

void ACPP_Character::DepleteHunger(float HungerCost)
{
	Hunger -= HungerCost;
	CanRegenerateStamina = Hunger < 0.0f;
	CanSprint = Hunger > 5.0f;
	CheckHunger();
}

void ACPP_Character::DepleteHealth(float Damage)
{
	Health -= Damage;
	HealthChanged.Broadcast(Health);
}

void ACPP_Character::ConsumeFood(float HungerAmount)
{
	if (Hunger > 20.0f) return;
	Inventory.Berries -= 1;
	DepleteHunger(-HungerAmount);
}

void ACPP_Character::HungerDamageTimeline(float Value)
{
	hungerCounter += Value;
	if (hungerCounter >= hungerTime) {
		isHungry = false;
		hungerCounter = 0.0f;
	}

	CheckHunger();
}

void ACPP_Character::StaminaRegainTimeline(float Value)
{
	staminaRechargeCounter += Value;
	if (staminaRechargeCounter >= staminaRechargeTime) {
		ForceStaminaCharge = false;
		staminaRechargeCounter = 0;
		Stamina = 20.0f;
	}
	StaminaChanged.Broadcast(FMath::Lerp(0.0f, 20.0f, staminaRechargeCounter / staminaRechargeTime));
}

void ACPP_Character::UpdateResources(FPlayerInventory inventory, EBuildingType buildingType)
{
	if (Inventory.Wood < inventory.Wood || Inventory.Stone < inventory.Stone) return;
	Inventory.Wood -= inventory.Wood;
	Inventory.Stone -= inventory.Stone;
	switch (buildingType)
	{
	case EBuildingType::Wall:
		BuildingArray[0]++;
		break;
	case EBuildingType::Floor:
		BuildingArray[1]++;
		break;
	case EBuildingType::Ceiling:
		BuildingArray[2]++;
		break;
	}
	InventoryChanged.Broadcast(Inventory);
}

void ACPP_Character::SpawnBuilding(int buildingID, bool& isSuccess)
{
	if (!isBuilding) {
		if (BuildingArray[buildingID] >= 1) {
			isBuilding = true;
			FActorSpawnParameters SpawnParam;
			FVector startLocation = GetMesh()->GetBoneLocation("head");
			FVector endLocation = startLocation + PlayerCam->GetForwardVector() * 600.0f;
			FRotator myRot(0, 0, 0);

			BuildingArray[buildingID]--;

			spawnedPart = GetWorld()->SpawnActor<ABuildingPart>(BuildPartClass, endLocation, myRot, SpawnParam);

			isSuccess = true;
		}
		isSuccess = false;
	}
}

void ACPP_Character::RotateBuilding()
{
	if (isBuilding) {
		spawnedPart->AddActorWorldRotation(FRotator(0, 45, 0));
	}
}

// Sets default values
ACPP_Character::ACPP_Character()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Capsule = GetCapsuleComponent();

	PlayerCam = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	PlayerCam->SetupAttachment(GetMesh(), "head");
	PlayerCam->bUsePawnControlRotation = true;
	PlayerCam->SetRelativeLocation(cameraOffset);

	BuildingArray.SetNum(3);
}

// Called when the game starts or when spawned
void ACPP_Character::BeginPlay()
{
	Super::BeginPlay();

	HealthChanged.Broadcast(Health);
	HungerChanged.Broadcast(Hunger);
	StaminaChanged.Broadcast(Stamina);
	InventoryChanged.Broadcast(Inventory);

	if (objWidget) {
		objWidget->UpdatebuildOBJ(0.0f);
		objWidget->UpdatematOBJ(0.0f);
	}
}

// Called every frame
void ACPP_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	bPressedJump = JumpPressed && GetCharacterMovement()->IsMovingOnGround();
	if (bPressedJump && Stamina >= 3.0f)
		DepleteStamina(3.0f);
	StaminaLoss(DeltaTime);
	StaminaHealPlayer();
	StaminaRegain(DeltaTime);

	if (ForceStaminaCharge)
		StaminaRegainTimeline(DeltaTime);
	if (isHungry)
		HungerDamageTimeline(DeltaTime);

	if (isBuilding) {
		if (spawnedPart) {
			FVector startLocation = GetMesh()->GetBoneLocation("head");
			FVector endLocation = startLocation + PlayerCam->GetForwardVector() * 600.0f;

			spawnedPart->SetActorLocation(endLocation);
		}
	}
}

// Called to bind functionality to input
void ACPP_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &ACPP_Character::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &ACPP_Character::MoveRight);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &ACPP_Character::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(FName("LookRight"), this, &ACPP_Character::AddControllerYawInput);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ACPP_Character::FindObject);
	PlayerInputComponent->BindAction("Eat", IE_Pressed, this, &ACPP_Character::EatFood);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACPP_Character::StartJumping);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACPP_Character::EndJumping);
	PlayerInputComponent->BindAction("Rotate", IE_Pressed, this, &ACPP_Character::RotateBuilding);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ACPP_Character::StartSprinting);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ACPP_Character::EndSprinting);
}
