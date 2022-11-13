// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseInteractableActor.generated.h"

class UBoxComponent;
class USceneComponent;
class UNiagaraComponent;
class UMaterialInterface;

UCLASS()
class ABSTRACTADVENTURE_API ABaseInteractableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseInteractableActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ItemParameters")
	UStaticMeshComponent* ItemMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ItemParameters")
	UStaticMeshComponent* BrokenMeshComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ItemParameters")
	UBoxComponent* ItemCollisionComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "ItemParameters")
	USceneComponent* ItemMuzzleLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	TSoftObjectPtr<AActor> ReferencedItemActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX")
	UNiagaraComponent* ItemFXComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void SetBaseDynamicMaterial();

	UMaterialInstanceDynamic* ItemBaseDynamicMaterial;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemStates")
	bool bBroken;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemStates")
	bool bCharged;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemStates")
	bool bToggled;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemProperties")
	int32 ChargesAmount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemProperties")
	bool bCanBeCharged;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemProperties")
	bool bCanBePickedUp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemProperties")
	bool bCanBeUsedPickedUp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemProperties")
	bool bStationary;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemProperties")
	bool bCharger;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemProperties")
	bool bRepairItem;

	void UseItem();

	void Charge(AActor* OtherActor);

	void PlayFX();

	void Toggle();

	void SwitchMaterial();

	void SetBeginItemCondition();

	void ContactReferencedItemActor();

	AActor* GetLoadedActor();

	bool bHolding;

	bool bGravity;
};
