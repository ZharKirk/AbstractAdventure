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
	UBoxComponent* ItemCollisionComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "ItemParameters")
	USceneComponent* ItemMuzzleLocation;

	//UPROPERTY(EditAnywhere, Category = "ItemParameters")
	//TSoftObjectPtr<AActor> ReferencedItemActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Building")// test
	TSoftObjectPtr<AActor> ReferencedItemActor;// test

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX")
	UNiagaraComponent* ItemFXComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	float Health;
	float DefaultHealth;

	void SetBaseDynamicMaterial();
	UMaterialInstanceDynamic* ItemBaseDynamicMaterial;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemStates")
	bool bBroken;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemStates")
	bool bCharged;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemStates")
	bool bToggledOn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemStates")
	bool bCanBePickedUp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemStates")
	bool bCanBeUsedPickedUp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemStates")
	bool bStationary;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemParameters")
	int32 ChargesAmount;

	bool bHolding;
	bool bGravity;

	void UseItem();

	void ToggleOn();
	void ToggleOff();

	void SwitchMaterial();

	void ContactReferencedItemActor();

	AActor* GetLoadedActor();// test
};
