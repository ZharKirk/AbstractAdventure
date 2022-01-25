// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/AACharacterBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "DrawDebugHelpers.h"
#include "Items/BaseInteractableActor.h"

// Sets default values
AAACharacterBase::AAACharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(RootComponent);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CharacterMesh"));
	MeshComp->SetupAttachment(RootComponent);

	ItemHoldingComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ItemHoldingComponent"));
	ItemHoldingComponent->SetupAttachment(CameraComp);

	CurrentStationaryActor = nullptr;
	CurrentInteractableActor = nullptr;
	bPlayerHoldingItem = false;

	BaseTurnRate = 45.0F;
	BaseLookUpAtRate = 45.0F;

	TraceDistance = 500;
}


void AAACharacterBase::CheckAddMovement(float Value, EAxis::Type Param)
{
	if ((Controller) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(Param);
		AddMovementInput(Direction, Value);
	}
}


void AAACharacterBase::MoveForward(float Value)
{
	CheckAddMovement(Value, EAxis::X);
}


void AAACharacterBase::MoveRight(float Value)
{
	CheckAddMovement(Value, EAxis::Y);
}


void AAACharacterBase::InteractPressed()
{
	TraceForward();
	PickupItem();
	ToggleStationaryItem();
}


void AAACharacterBase::ActionPressed()
{
	UsePickupItem();
}


// Called to bind functionality to input
void AAACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AAACharacterBase::InteractPressed);
	PlayerInputComponent->BindAction("Action", IE_Pressed, this, &AAACharacterBase::ActionPressed);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AAACharacterBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AAACharacterBase::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
}


void AAACharacterBase::TraceForward_Implementation()
{
	FVector Loc;
	FRotator Rot;
	FHitResult Hit;

	GetController()->GetPlayerViewPoint(Loc, Rot);

	FVector Start = Loc;
	FVector End = Start + (Rot.Vector() * TraceDistance);

	FCollisionQueryParams TraceParams;
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, TraceParams);

	FComponentQueryParams DefaultComponentQueryParams;
	FCollisionResponseParams DefaultResponseParams;
	bool bHitByChannel = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, DefaultComponentQueryParams, DefaultResponseParams);

	//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 2.0f);

	if (bHit)
	{
		//DrawDebugBox(GetWorld(), Hit.ImpactPoint, FVector(5, 5, 5), FColor::Emerald, false, 2.0f);		
		GetInteractableItem(bHitByChannel, Hit);
		GetStationaryItem(bHitByChannel, Hit);
	}
	else
	{
		CurrentStationaryActor = NULL;
	}
}

void AAACharacterBase::GetInteractableItem(bool bHitByChannel, FHitResult& Hit)
{
	if (!bPlayerHoldingItem)
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
}


void AAACharacterBase::GetStationaryItem(bool bHitByChannel, FHitResult& Hit)
{
	if (bHitByChannel)
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


void AAACharacterBase::PickupItem()
{
	if (CurrentInteractableActor)
	{
		if (CurrentInteractableActor->bCanBePickedUp)
		{
			bPlayerHoldingItem = !bPlayerHoldingItem; // set to NOT (current state)

			// Pickup Start
			TArray<UStaticMeshComponent*> Components;
			CurrentInteractableActor->GetComponents<UStaticMeshComponent>(Components);

			bool bItemHolding = CurrentInteractableActor->bHolding; // get state from current object
			bool bItemGravity = CurrentInteractableActor->bGravity; // get state from current object

			bItemHolding = !bItemHolding; // set to NOT (current state)
			CurrentInteractableActor->bHolding = bItemHolding;

			bItemGravity = !bItemGravity; // set to NOT (current state)
			CurrentInteractableActor->bGravity = bItemGravity;

			AttachItem(Components, bItemGravity, bItemHolding);
		}
		if (!bPlayerHoldingItem)
		{
			CurrentInteractableActor = NULL;
		}
	}
}


void AAACharacterBase::AttachItem(TArray<UStaticMeshComponent*>& Components, bool bItemGravity, bool bItemHolding)
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

			if (bItemHolding)
			{
				ItemMeshComponent->AttachToComponent(ItemHoldingComponent, FAttachmentTransformRules::KeepWorldTransform);

				// Holding Item Orientation
				if (CurrentInteractableActor->bCanBePickedUp && CurrentInteractableActor->bCanBeUsedPickedUp)
				{
					ItemMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
					ItemMeshComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
				}
				else
				{
					SetActorLocation(ItemHoldingComponent->GetComponentLocation());
				}
			}
			else
			{
				ItemMeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
				FVector ForwardVector = CameraComp->GetForwardVector();
				ItemMeshComponent->AddForce(ForwardVector * 100000 * ItemMeshComponent->GetMass());
			}
		}
	}
}


void AAACharacterBase::UsePickupItem()
{
	if (bPlayerHoldingItem)
	{
		CurrentInteractableActor->UseItem();
	}
}


void AAACharacterBase::ToggleStationaryItem()
{
	if (CurrentStationaryActor)
	{
		if (CurrentStationaryActor->bStationary)
		{
			if (!CurrentStationaryActor->bToggledOn)
			{
				CurrentStationaryActor->ToggleOn();
			}
			else
			{
				CurrentStationaryActor->ToggleOff();
			}
		}
	}
}