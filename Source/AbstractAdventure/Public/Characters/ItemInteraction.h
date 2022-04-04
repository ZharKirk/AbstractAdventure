// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemInteraction.generated.h"

class ABaseInteractableActor;
class UTraceForwardComponent;
class AAACharacterBase;


UCLASS()
class ABSTRACTADVENTURE_API AItemInteraction : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemInteraction();

	UPROPERTY(EditAnywhere)
	ABaseInteractableActor* CurrentInteractableActor;

	UPROPERTY(EditAnywhere)
	ABaseInteractableActor* CurrentStationaryActor;

	AAACharacterBase* Player;
	APlayerController* PlayerController;

	UTraceForwardComponent* TraceForwardComponent;

	void SetPickupItemState();
	void ToggleStationaryItem();
	void UsePickupItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool bPlayerHoldingItem;

	void AttachItem(TArray<UStaticMeshComponent*>& Components, bool bItemGravity, bool bItemHolding);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void GetInteractableTypeItem();
};