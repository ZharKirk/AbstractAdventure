// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AACharacterBase.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UStaticMeshComponent;
class USceneComponent;
class ABaseInteractableActor;


UCLASS()
class ABSTRACTADVENTURE_API AAACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAACharacterBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere)
	class USceneComponent* ItemHoldingComponent;

	UPROPERTY(EditAnywhere)
	ABaseInteractableActor* CurrentInteractableActor;

	UPROPERTY(EditAnywhere)
	ABaseInteractableActor* CurrentStationaryActor;

protected:
	void InteractPressed();
	void ActionPressed();
	void PickupItem();
	void ToggleStationaryItem();
	void UsePickupItem();
	void AttachItem(TArray<UStaticMeshComponent*>& Components, bool bItemGravity, bool bItemHolding);

	bool bPlayerHoldingItem;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void CheckAddMovement(float Value, EAxis::Type Param);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseTurnRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseLookUpAtRate;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float TraceDistance;

	UFUNCTION(BlueprintNativeEvent)
	void TraceForward();
	void TraceForward_Implementation();

	void GetInteractableItem(bool bHitByChannel, FHitResult& Hit);
	void GetStationaryItem(bool bHitByChannel, FHitResult& Hit);

public:
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};