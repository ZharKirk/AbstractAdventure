// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/AACharacterBase.h"
#include "Components/TraceForwardComponent.h"
#include "Characters/ItemInteraction.h"
#include "Items/BaseInteractableActor.h"
//
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"


// Sets default values
AAACharacterBase::AAACharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(RootComponent);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CharacterMesh"));
	MeshComp->SetupAttachment(RootComponent);

	ItemHoldingComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ItemHoldingComponent"));
	ItemHoldingComponent->SetupAttachment(CameraComp);

	TraceForwardComponent = nullptr;
	CurrentStationaryActor = nullptr;
	CurrentInteractableActor = nullptr;

	bPlayerHoldingItem = false;
	bPlayerButtonPressed = false;

	BaseTurnRate = 45.0F; 
	BaseLookUpAtRate = 45.0F;
}


void AAACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetWorld()->GetFirstPlayerController()->GetInputKeyTimeDown(FKey("LeftMouseButton")) >= 5.0f && bPlayerButtonPressed)
	{
		UE_LOG(LogTemp, Warning, TEXT("%f"), GetWorld()->GetFirstPlayerController()->GetInputKeyTimeDown(FKey("LeftMouseButton")));
		RepairStationaryActor();
		bPlayerButtonPressed = false;
	}
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


void AAACharacterBase::InteractPressed() // "E" - to interact with object
{
	TraceForwardComponentInitialization();
	GetItemType();
	SetPickupItemState();
	ToggleStationaryItem();
}


void AAACharacterBase::ActionPressed() // "LMB" - to use what equiped
{
	UsePickupItem();
}


/// Item Interaction ///
void AAACharacterBase::GetItemType()
{
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


void AAACharacterBase::SetPickupItemState()
{
	if (CurrentInteractableActor)
	{
		if (CurrentInteractableActor->bCanBePickedUp)
		{
			bPlayerHoldingItem = !bPlayerHoldingItem; // set to NOT (current state)

			TArray<UStaticMeshComponent*> Components;
			CurrentInteractableActor->GetComponents<UStaticMeshComponent>(Components);

			// TODO Probably should move this inside AttachItem function
			bool bItemHolding = CurrentInteractableActor->bHolding; // get state from current object
			bool bItemGravity = CurrentInteractableActor->bGravity; // get state from current object
			bItemHolding = !bItemHolding; // set to NOT (current state)
			CurrentInteractableActor->bHolding = bItemHolding;
			bItemGravity = !bItemGravity; // set to NOT (current state)
			CurrentInteractableActor->bGravity = bItemGravity;
			// TODO Probably should move this inside AttachItem function

			AttachItem(Components, bItemGravity, bItemHolding);
		}
		if (!bPlayerHoldingItem)
		{
			CurrentInteractableActor = NULL;
		}
	}
}


void AAACharacterBase::ToggleStationaryItem()
{
	if (CurrentStationaryActor)
	{
		if (CurrentStationaryActor->bStationary)
		{
			CurrentStationaryActor->Toggle();
		}
	}
}


void AAACharacterBase::AttachItem(TArray<UStaticMeshComponent*>& Components, bool bItemGravity, bool bItemHolding)
{
	UStaticMeshComponent* ItemMeshComponent;

	ForwardVector = CameraComp->GetForwardVector();

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
				ItemMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
				ItemMeshComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
				SetActorLocation(ItemHoldingComponent->GetComponentLocation());
			}
			else
			{
				ItemMeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
				ItemMeshComponent->AddForce(ForwardVector * DetachThrowForce * ItemMeshComponent->GetMass());
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("No components in item match 'ItemMeshComponent' name to attach!"));
		}
	}
}


void AAACharacterBase::UsePickupItem()
{
	if (bPlayerHoldingItem)
	{
		if (CurrentInteractableActor->bRepairItem)
		{
			CurrentInteractableActor->UseItem();
			bPlayerButtonPressed = true;
		}
		else
		{
			CurrentInteractableActor->UseItem();
		}		
	}
}


void AAACharacterBase::RepairStationaryActor()
{
	TraceForwardComponentInitialization();
	GetItemType();

	if (CurrentStationaryActor && CurrentStationaryActor->bStationary && CurrentStationaryActor->bBroken)
	{
		UE_LOG(LogTemp, Warning, TEXT("RepairItem! %s"), *CurrentStationaryActor->GetName());
		CurrentStationaryActor->bBroken = false;
		CurrentStationaryActor->SetItemCondition();
	}
}


void AAACharacterBase::TraceForwardComponentInitialization()
{
	TraceForwardComponent = Cast<UTraceForwardComponent>(GetComponentByClass(UTraceForwardComponent::StaticClass()));
	GetController()->GetPlayerViewPoint(Loc, Rot);
	bHitByChannel = false;
	Hit.Init();

	if (TraceForwardComponent)
	{
		TraceForwardComponent->TraceForward(Loc, Rot, Hit, bHitByChannel);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("TraceForward Component Initialization failed!"));
		return;
	}
}