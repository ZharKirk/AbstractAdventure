// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseInteractableActor.generated.h"

class UBoxComponent;
class USceneComponent;
class UNiagaraComponent;
class UMaterialInterface;
class UPlayerInventory;

UCLASS()
class ABSTRACTADVENTURE_API ABaseInteractableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseInteractableActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ItemGameplay")
	USkeletalMeshComponent* ItemMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ItemGameplay")
	USkeletalMeshComponent* BrokenMeshComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ItemGameplayParameters")
	UBoxComponent* ItemCollisionComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "GameplayParameters")
	USceneComponent* ItemMuzzleLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemBuilding")
	TSoftObjectPtr<AActor> ReferencedItemActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemFX")
	UNiagaraComponent* ItemFXComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemGameplay")
	UAnimMontage* UseAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemGameplay")
	UAnimMontage* ReloadAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemGameplay")
	UAnimMontage* EmptyAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemGameplay")
	USoundBase* UseSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemGameplay")
	USoundBase* ReloadSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemGameplay")
	USoundBase* EmptySound;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void SetBaseDynamicMaterial();

	UMaterialInstanceDynamic* ItemBaseDynamicMaterial;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemGameplayParameters")
	bool bBroken;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemGameplayParameters")
	bool bCharged;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemGameplayParameters")
	bool bToggled;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemGameplayParameters")
	int32 ChargesAmount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemGameplayParameters")
	bool bCanBeCharged;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemGameplayParameters")
	bool bCanBePickedUp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemGameplayParameters")
	bool bCanBeUsedPickedUp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemGameplayParameters")
	bool bStationary;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemGameplayParameters")
	bool bCharger;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemGameplayParameters")
	bool bRepairItem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemGameplayParameters")
	bool bAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemGameplayParameters")
	bool bBattaries;

	void UseItem();

	void PlayMontage(UAnimMontage* Montage);

	void PlaySound(USoundBase* Sound);

	void Charge(AActor* OtherActor);

	void PlayFX();

	void Toggle();

	void SwitchMaterial();

	//void SetBeginItemCondition();

	void ContactReferencedItemActor();

	AActor* GetLoadedActor();

	bool bHolding;

	bool bGravity;

};
