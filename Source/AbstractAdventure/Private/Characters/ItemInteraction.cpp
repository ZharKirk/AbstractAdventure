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

	CurrentStationaryActor = nullptr;
	CurrentInteractableActor = nullptr;

	bPlayerHoldingItem = false;
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


void AItemInteraction::GetInteractableTypeItem(APlayerController* PController, UTraceForwardComponent* TraceForwardComponent)
{
	FVector Loc;
	FRotator Rot;
	FHitResult Hit;
	bool bHitByChannel = false;

	PController->GetPlayerViewPoint(Loc, Rot);
	TraceForwardComponent->TraceForward(Loc, Rot, bHitByChannel, Hit);
	//UE_LOG(LogTemp, Warning, TEXT("%s 44"), *TraceForwardComponent->GetName());
	if (!bPlayerHoldingItem) // if Interactable Actor
	{
		if (bHitByChannel)
		{
			if (Hit.GetActor()->GetClass()->IsChildOf(ABaseInteractableActor::StaticClass()))
			{
				CurrentInteractableActor = Cast<ABaseInteractableActor>(Hit.GetActor());
			}
			else
			{
				CurrentInteractableActor = NULL;
			}
		}
		else
		{
			CurrentInteractableActor = NULL;
		}
	}

	if (bHitByChannel) // if Stationary Actor
	{
		if (Hit.GetActor()->GetClass()->IsChildOf(ABaseInteractableActor::StaticClass()))
		{
			CurrentStationaryActor = Cast<ABaseInteractableActor>(Hit.GetActor());
		}
		else
		{
			CurrentStationaryActor = NULL;
		}
	}
	else
	{
		CurrentStationaryActor = NULL;
	}
}


void AItemInteraction::SetPickupItemState(AAACharacterBase* Player, FVector ForwardVector)
{
	if (CurrentInteractableActor)
	{
		if (CurrentInteractableActor->bCanBePickedUp)
		{
			bPlayerHoldingItem = !bPlayerHoldingItem; // set to NOT (current state)

			TArray<UStaticMeshComponent*> Components;
			CurrentInteractableActor->GetComponents<UStaticMeshComponent>(Components);

			bool bItemHolding = CurrentInteractableActor->bHolding; // get state from current object
			bool bItemGravity = CurrentInteractableActor->bGravity; // get state from current object

			bItemHolding = !bItemHolding; // set to NOT (current state)
			CurrentInteractableActor->bHolding = bItemHolding;

			bItemGravity = !bItemGravity; // set to NOT (current state)
			CurrentInteractableActor->bGravity = bItemGravity;

			AttachItem(Player, ForwardVector, Components, bItemGravity, bItemHolding);
		}
		if (!bPlayerHoldingItem)
		{
			CurrentInteractableActor = NULL;
		}
	}
}


void AItemInteraction::AttachItem(AAACharacterBase* Player, FVector ForwardVector, TArray<UStaticMeshComponent*>& Components, bool bItemGravity, bool bItemHolding)
{
	UStaticMeshComponent* ItemMeshComponent;

	for (UStaticMeshComponent* MeshComponent : Components)
	{
		if (MeshComponent->GetName() == "ItemMeshComponent")
		{
			ItemMeshComponent = MeshComponent;

			ItemMeshComponent->SetEnableGravity(bItemGravity);
			ItemMeshComponent->SetSimulatePhysics(bItemHolding ? false : true);
			ItemMeshComponent->SetCollisionEnabled(bItemHolding ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryAndPhysics);

			if (bItemHolding) // TODO fix AAACharacterBase dependency
			{
				ItemMeshComponent->AttachToComponent(Player->ItemHoldingComponent, FAttachmentTransformRules::KeepWorldTransform); // TODO fix AAACharacterBase dependency

				// Holding Item Orientation
				if (CurrentInteractableActor->bCanBePickedUp && CurrentInteractableActor->bCanBeUsedPickedUp)
				{
					ItemMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
					ItemMeshComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
				}
				else
				{
					SetActorLocation(Player->ItemHoldingComponent->GetComponentLocation()); // TODO fix AAACharacterBase dependency
				}
			}
			else
			{
				ItemMeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
				ItemMeshComponent->AddForce(ForwardVector * 100000 * ItemMeshComponent->GetMass());  // TODO fix magic value  // TODO fix AAACharacterBase dependency
			}
		}
	}
}


void AItemInteraction::UsePickupItem()
{
	if (bPlayerHoldingItem)
	{
		CurrentInteractableActor->UseItem();
	}
}


void AItemInteraction::ToggleStationaryItem()
{
	if (CurrentStationaryActor)
	{
		if (CurrentStationaryActor->bStationary)
		{
			CurrentStationaryActor->Toggle();
		}
	}
}