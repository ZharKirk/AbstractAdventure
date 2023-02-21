// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerInventory.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ABSTRACTADVENTURE_API UPlayerInventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerInventory();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	int32 AmmoAmount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	int32 BatteriesAmount;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void AmmoPool();

	void BatteriesPool();
};
