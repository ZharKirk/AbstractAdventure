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
	//virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void SetBaseDynamicMaterial();

	void SetItemCondition();

	UMaterialInstanceDynamic* ItemBaseDynamicMaterial;

	float Health;

	float DefaultHealth;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemStates")
	bool bBroken;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemStates")
	bool bCharged;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemStates")
	bool bToggled;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemParameters")
	int32 ChargesAmount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemProperties")
	bool bCanBePickedUp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemProperties")
	bool bCanBeUsedPickedUp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemProperties")
	bool bStationary;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemProperties")
	bool bRepairItem;

	void UseItem();

	//void RepairItem();

	void Toggle();

	void SwitchMaterial();

	void ContactReferencedItemActor();

	AActor* GetLoadedActor();

	bool bHolding;

	bool bGravity;
};
