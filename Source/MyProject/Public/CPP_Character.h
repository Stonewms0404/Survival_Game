// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/CapsuleComponent.h"
#include "Components/TimelineComponent.h"
#include <Camera/CameraComponent.h>
#include "Components/ArrowComponent.h"
#include "CPP_PlayerInventory.h"
#include "BuildingType.h"
#include "BuildingPart.h"
#include "ObjectiveWidget.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CPP_Character.generated.h"

class UCurveFloat;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHealthChangedSignature, float, _health);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHungerChangedSignature, float, _hunger);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStaminaChangedSignature, float, _stamina);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryChangedSignature, FPlayerInventory, _inventory);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MYPROJECT_API ACPP_Character : public ACharacter
{
	GENERATED_BODY()

public:
	// ~~~~~~~~~~~~~~~~~ //
	// ~~~ Functions ~~~ //
	// ~~~~~~~~~~~~~~~~~ //

#pragma region Public Functions
	/** Any Action that will cost stamina and lower it's value. */
	UFUNCTION(BlueprintCallable)
	void DepleteStamina(float StaminaCost);

	/** Adds or subracts hunger and broadcasts when the hunger value changes */
	UFUNCTION(BlueprintCallable)
	void DepleteHunger(float HungerCost);

	/** Adds or subracts health and broadcasts when the health value changes */
	UFUNCTION(BlueprintCallable)
	void DepleteHealth(float Damage);

	/** Adds hunger through the DepleteHunger function */
	UFUNCTION(BlueprintCallable)
	void ConsumeFood(float HungerAmount);

	/** Plays if the player is hungry and depletes health every hungerTime seconds */
	UFUNCTION()
	void HungerDamageTimeline(float Value);

	/** Plays if the player has ran out of stamina and updates visual stamina every staminaRechargeTime seconds */
	UFUNCTION()
	void StaminaRegainTimeline(float Value);

	UFUNCTION(BlueprintCallable)
	void UpdateResources(FPlayerInventory inventory, EBuildingType buildingType);

	UFUNCTION(BlueprintCallable)
	void SpawnBuilding(int buildingID, bool& isSuccess);

	UFUNCTION(BlueprintCallable)
	void RotateBuilding();
#pragma endregion

	// ~~~~~~~~~~~~~~~~~ //
	// ~~~ Variables ~~~ //
	// ~~~~~~~~~~~~~~~~~ //

#pragma region Public Variables
	float Health = 20.0f;
	float Hunger = 20.0f;
	float Stamina = 20.0f;
	float StaminaLossCounter = 0.0f;
	float StaminaRegainCounter = 0.0f;
	bool CanRegenerateStamina = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	bool isInventoryOpen = false;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	bool ForceStaminaCharge = false;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Movement")
	UCapsuleComponent* Capsule;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FPlayerInventory Inventory;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	TArray<int> BuildingArray;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	TSubclassOf<ABuildingPart> BuildPartClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UObjectiveWidget* objWidget;

	UPROPERTY()
	float objectsBuilt;

	UPROPERTY()
	float matsCollected;

#pragma endregion

	// ~~~~~~~~~~~~~~~~~ //
	// ~~~~ Events ~~~~~ //
	// ~~~~~~~~~~~~~~~~~ //

#pragma region Public Delegates
	UPROPERTY(BlueprintAssignable)
	FHealthChangedSignature HealthChanged;
	UPROPERTY(BlueprintAssignable)
	FHungerChangedSignature HungerChanged;
	UPROPERTY(BlueprintAssignable)
	FStaminaChangedSignature StaminaChanged;
	UPROPERTY(BlueprintAssignable)
	FInventoryChangedSignature InventoryChanged;
#pragma endregion

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	// Sets default values for this character's properties
	ACPP_Character();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


private:
	// ~~~~~~~~~~~~~~~~~ //
	// ~~~ Functions ~~~ //
	// ~~~~~~~~~~~~~~~~~ //

#pragma region Private Functions
	/** Function to represent sprinting costing stamina. */
	UFUNCTION()
	void StaminaLoss(float DeltaTime);

	/** Function to represent sprinting costing stamina. */
	UFUNCTION()
	void StaminaHealPlayer();

	UFUNCTION()
	void EatFood();

	/** Regaining Stamina by not sprinting or using any action that will cost stamina. */
	UFUNCTION()
	void StaminaRegain(float DeltaTime);

	/** If Stamina runs out this will blank out the stamina bar and any stamina action cannot be used. */
	UFUNCTION()
	void ForceStaminaChargeFunc();

	/** Please add a function description */
	UFUNCTION()
	void CheckStamina();

	/** Please add a function description */
	UFUNCTION()
	void CheckHunger();

	/** Please add a function description */
	UFUNCTION()
	void StartJumping();

	/** Please add a function description */
	UFUNCTION()
	void EndJumping();

	/** Please add a function description */
	UFUNCTION()
	void MoveForward(float AxisValue);

	/** Please add a function description */
	UFUNCTION()
	void MoveRight(float AxisValue);

	/** Please add a function description */
	UFUNCTION()
	void FindObject();

	/** Please add a function description */
	UFUNCTION()
	void StartSprinting();

	/** Please add a function description */
	UFUNCTION()
	void EndSprinting();
#pragma endregion

	// ~~~~~~~~~~~~~~~~~ //
	// ~~~ Variables ~~~ //
	// ~~~~~~~~~~~~~~~~~ //

#pragma region Private Variables

	bool JumpPressed;
	bool isHungry = false;
	bool CanSprint = true;
	bool IsMoving = false;
	bool IsSprinting = false;
	bool isBuilding = false;
	ABuildingPart* spawnedPart;

	float staminaRechargeTime = 6.0f;
	float staminaRechargeCounter = 0.0f;
	float hungerTime = 4.0f;
	float hungerCounter = 0.0f;

	FVector cameraOffset = FVector(0, 20, 0);

	UCameraComponent* PlayerCam;

#pragma endregion

};
