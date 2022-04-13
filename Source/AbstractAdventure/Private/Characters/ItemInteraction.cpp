// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/ItemInteraction.h"
#include "Characters/AACharacterBase.h"
#include "Items/BaseInteractableActor.h"
#include "Components/TraceForwardComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AItemInteraction::AItemInteraction()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}


// Called when the game starts or when spawned
void AItemInteraction::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void AItemInteraction::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AItemInteraction::SetPickupItemState(AAACharacterBase* Player, FVector ForwardVector)
{
	//if (CurrentInteractableActor)
	//{
	//	if (CurrentInteractableActor->bCanBePickedUp)
	//	{
	//		bPlayerHoldingItem = !bPlayerHoldingItem; // set to NOT (current state)

	//		TArray<UStaticMeshComponent*> Components;
	//		CurrentInteractableActor->GetComponents<UStaticMeshComponent>(Components);

	//		bool bItemHolding = CurrentInteractableActor->bHolding; // get state from current object
	//		bool bItemGravity = CurrentInteractableActor->bGravity; // get state from current object

	//		bItemHolding = !bItemHolding; // set to NOT (current state)
	//		CurrentInteractableActor->bHolding = bItemHolding;

	//		bItemGravity = !bItemGravity; // set to NOT (current state)
	//		CurrentInteractableActor->bGravity = bItemGravity;

	//		AttachItem(Player, ForwardVector, Components, bItemGravity, bItemHolding);
	//	}
	//	if (!bPlayerHoldingItem)
	//	{
	//		CurrentInteractableActor = NULL;
	//	}
	//}
}


void AItemInteraction::AttachItem(AAACharacterBase* Player, FVector ForwardVector, TArray<UStaticMeshComponent*>& Components, bool bItemGravity, bool bItemHolding)
{
	//UStaticMeshComponent* ItemMeshComponent;

	//for (UStaticMeshComponent* MeshComponent : Components)
	//{
	//	if (MeshComponent->GetName() == "ItemMeshComponent")
	//	{
	//		ItemMeshComponent = MeshComponent;

	//		ItemMeshComponent->SetEnableGravity(bItemGravity);
	//		ItemMeshComponent->SetSimulatePhysics(bItemHolding ? false : true);
	//		ItemMeshComponent->SetCollisionEnabled(bItemHolding ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryAndPhysics);

	//		if (bItemHolding)
	//		{
	//			ItemMeshComponent->AttachToComponent(ItemHoldingComponent, FAttachmentTransformRules::KeepWorldTransform);

	//			// Holding Item Orientation
	//			if (CurrentInteractableActor->bCanBePickedUp && CurrentInteractableActor->bCanBeUsedPickedUp)
	//			{
	//				ItemMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	//				ItemMeshComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	//			}
	//			else
	//			{
	//				SetActorLocation(ItemHoldingComponent->GetComponentLocation());
	//			}
	//		}
	//		else
	//		{
	//			ItemMeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	//			ItemMeshComponent->AddForce(ForwardVector * 100000 * ItemMeshComponent->GetMass());  // TODO fix magic value
	//		}
	//	}
	//}
}


void AItemInteraction::UsePickupItem()
{
	//if (bPlayerHoldingItem)
	//{
	//	CurrentInteractableActor->UseItem();
	//}
}


void AItemInteraction::ToggleStationaryItem()
{
	//if (CurrentStationaryActor)
	//{
	//	if (CurrentStationaryActor->bStationary)
	//	{
	//		CurrentStationaryActor->Toggle();
	//	}
	//}
}