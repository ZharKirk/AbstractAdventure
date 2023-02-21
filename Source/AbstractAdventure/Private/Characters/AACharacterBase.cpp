// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/AACharacterBase.h"
#include "Components/TraceForwardComponent.h"
#include "Items/BaseInteractableActor.h"
#include "PlayerInventory.h"
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
	Inventory = nullptr;
	CurrentInteractableActor = nullptr;
	CurrentHoldingActor = nullptr;

	bPlayerHoldingItem = false;
	bPlayerActionButtonPressed = false;

	bLastCharge = false;

	BaseTurnRate = 45.0F; 
	BaseLookUpAtRate = 45.0F;

	CountdownTime = 5.0f;
}


void AAACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bPlayerActionButtonPressed)
	{
		CountdownTimeButtonPressed();
	}
	AimDownSights();
}


void AAACharacterBase::CountdownTimeButtonPressed()
{
	if ((GetWorld()->GetFirstPlayerController()->GetInputKeyTimeDown(FKey("LeftMouseButton")) >= CountdownTime))
	{
		RepairStationaryActor();
		bPlayerActionButtonPressed = false;
		bLastCharge = false;
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
	PlayerInputComponent->BindAction("Drop", IE_Pressed, this, &AAACharacterBase::DropPressed);
	PlayerInputComponent->BindAction("Action", IE_Pressed, this, &AAACharacterBase::ActionPressed);
	PlayerInputComponent->BindAction("WeaponAim", IE_Pressed, this, &AAACharacterBase::AimDownSights);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AAACharacterBase::ReloadItem);

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

	GetItem();

	if (CurrentInteractableActor && CurrentInteractableActor->bCanBePickedUp)
	{
		if (!bPlayerHoldingItem)
		{
			bPlayerHoldingItem = true;
			CurrentHoldingActor = CurrentInteractableActor;
			SetPickupItemState();
		}
	}

	ToggleStationaryItem();
}


void AAACharacterBase::DropPressed() // "G" - to drop object
{
	if (bPlayerHoldingItem)
	{
		bPlayerHoldingItem = false;
		SetPickupItemState();
		CurrentHoldingActor = NULL;
	}
}


void AAACharacterBase::ActionPressed() // "LMB" - to use what equiped
{
	UsePickupItem();
}

// TODO rewrite this
void AAACharacterBase::AimDownSights() // "RMB" - to use what equiped
{
	float CameraDefaultFieldOfView = 90.0f;
	if (bPlayerHoldingItem)
	{
		FVector StartPos = FVector(ItemHoldingComponent->GetRelativeLocation());

		if (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(FKey("RightMouseButton")))
		{
			FVector EndPos = FVector(StartPos.X, 0.0f, -35.0f);
			FVector Movement = FMath::Lerp(StartPos, EndPos, 0.1f);

			ItemHoldingComponent->SetRelativeLocation(Movement);
			float ZoomFieldOfView = FMath::Lerp(55.0f, CameraDefaultFieldOfView, 0.001f);
			CameraComp->SetFieldOfView(ZoomFieldOfView);
		}
	}
	if (!(GetWorld()->GetFirstPlayerController()->IsInputKeyDown(FKey("RightMouseButton"))))
	{
		FVector StartPos = FVector(ItemHoldingComponent->GetRelativeLocation());
		FVector EndPos = FVector(StartPos.X, 25.0f, -48.0f);
		FVector Movement = FMath::Lerp(StartPos, EndPos, 0.1f);

		ItemHoldingComponent->SetRelativeLocation(Movement);
		float DefaultFieldOfView = FMath::Lerp(CameraDefaultFieldOfView, 55.0f, 0.001f);
		CameraComp->SetFieldOfView(90.0f);
	}
}


// TODO rewrite this
void AAACharacterBase::ReloadItem() // "R" - to reload item charges
{
	Inventory = Cast<UPlayerInventory>(GetComponentByClass(UPlayerInventory::StaticClass()));
	if (Inventory)
	{
		UE_LOG(LogTemp, Warning, TEXT("Reload!"));
		int32 AmmoInInventory = Inventory->AmmoAmount;
		int32 BatteriesInInventory = Inventory->BatteriesAmount;
		if (bPlayerHoldingItem)
		{
			if (CurrentHoldingActor)
			{
				if (!CurrentHoldingActor->bCharged)
				{
					CurrentHoldingActor->ChargesAmount = AmmoInInventory;
					CurrentHoldingActor->bCharged = true;
				}
			}
		}
	}
}


/// Item Interaction ///
void AAACharacterBase::GetItem()
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


void AAACharacterBase::SetPickupItemState()
{
	if (CurrentHoldingActor)
	{
		if (CurrentHoldingActor->bCanBePickedUp)
		{
			TArray<USkeletalMeshComponent*> Components;
			CurrentHoldingActor->GetComponents<USkeletalMeshComponent>(Components);
			AttachItem(Components);
		}
		if (!bPlayerHoldingItem)
		{
			CurrentHoldingActor = NULL;
		}
	}
}


void AAACharacterBase::AttachItem(TArray<USkeletalMeshComponent*>& Components)
{
	bool bItemHolding = CurrentHoldingActor->bHolding; // get state from current object
	bool bItemGravity = CurrentHoldingActor->bGravity; // get state from current object
	bItemHolding = !bItemHolding; // set to NOT (current state)
	CurrentHoldingActor->bHolding = bItemHolding;
	bItemGravity = !bItemGravity; // set to NOT (current state)
	CurrentHoldingActor->bGravity = bItemGravity;

	USkeletalMeshComponent* ItemMeshComponent;

	ForwardVector = CameraComp->GetForwardVector();

	for (USkeletalMeshComponent* MeshComponent : Components)
	{
		if (MeshComponent->GetName() == "ItemMeshComponent")
		{
			ItemMeshComponent = MeshComponent;

			ItemMeshComponent->SetEnableGravity(bItemGravity);
			ItemMeshComponent->SetSimulatePhysics(bItemHolding ? false : true);
			ItemMeshComponent->SetCollisionEnabled(bItemHolding ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryAndPhysics);

			if (bItemHolding)
			{
				ItemMeshComponent->AttachToComponent(ItemHoldingComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
				//ItemMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
				//ItemMeshComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
				//SetActorLocation(ItemHoldingComponent->GetComponentLocation());

			}
			else
			{
				ItemMeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
				ItemMeshComponent->AddForce(ForwardVector * DetachThrowForce * ItemMeshComponent->GetMass());
			}
		}
	}
}


void AAACharacterBase::ToggleStationaryItem()
{
	if (CurrentInteractableActor)
	{
		if (CurrentInteractableActor->bStationary)
		{
			CurrentInteractableActor->Toggle();
		}
	}
}


void AAACharacterBase::UsePickupItem()
{
	if (bPlayerHoldingItem)
	{
		bPlayerActionButtonPressed = true;
		if (CurrentHoldingActor)
		{
			if (CurrentHoldingActor->bCharged)
			{
				bLastCharge = true;
				CurrentHoldingActor->UseItem();
			}
		}
	}
}


void AAACharacterBase::RepairStationaryActor()
{
	TraceForwardComponentInitialization();
	GetItem();

	if (CurrentInteractableActor && CurrentInteractableActor->bStationary && CurrentInteractableActor->bBroken)
	{
		if (bLastCharge)
		{
			CurrentInteractableActor->bBroken = false;
			//CurrentInteractableActor->SetBeginItemCondition();
		}
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