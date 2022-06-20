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
class UTraceForwardComponent;
class AItemInteraction;


UCLASS()
class ABSTRACTADVENTURE_API AAACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAACharacterBase();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	ABaseInteractableActor* CurrentInteractableActor;

	UPROPERTY(EditAnywhere)
	ABaseInteractableActor* CurrentStationaryActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere)
	USceneComponent* ItemHoldingComponent;

	AItemInteraction* ItemInteractionComponent;

	UTraceForwardComponent* TraceForwardComponent;

	bool bPlayerHoldingItem;
	bool bPlayerButtonPressed;

	void TraceForwardComponentInitialization();

	void GetItemType();
	void SetPickupItemState();
	void AttachItem(TArray<UStaticMeshComponent*>& Components, bool bItemGravity, bool bItemHolding);
	void UsePickupItem();
	void ToggleStationaryItem();
	void RepairStationaryActor();

	FVector Loc;
	FRotator Rot;
	FHitResult Hit;
	bool bHitByChannel;

protected:
	void InteractPressed();
	void ActionPressed();

	void MoveForward(float Value);
	void MoveRight(float Value);
	void CheckAddMovement(float Value, EAxis::Type Param);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseTurnRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseLookUpAtRate;

	int32 DetachThrowForce = 40000;

public:
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FVector ForwardVector;

};