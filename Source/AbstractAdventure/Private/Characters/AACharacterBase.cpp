// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/AACharacterBase.h"
#include "Components/TraceForwardComponent.h"
#include "Characters/ItemInteraction.h"
#include "Items/BaseInteractableActor.h"
//
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"


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

	ItemInteractionComponent = Cast<AItemInteraction>(GetComponentByClass(AItemInteraction::StaticClass()));

	TraceForwardComponent = nullptr;

	BaseTurnRate = 45.0F;
	BaseLookUpAtRate = 45.0F;
	ForwardVector = CameraComp->GetForwardVector();
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
	TraceForwardComponent = Cast<UTraceForwardComponent>(GetComponentByClass(UTraceForwardComponent::StaticClass()));

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Cast<APlayerController>(*Iterator);

		if (PlayerController && TraceForwardComponent)
		{
			ItemInteractionComponent->GetInteractableTypeItem(PlayerController, TraceForwardComponent);
			//ItemInteractionComponent->SetPickupItemState(this, ForwardVector);
			//ItemInteractionComponent->ToggleStationaryItem();
		}
	}	
}


void AAACharacterBase::ActionPressed() // "LMB" - to use what equiped
{
	//ItemInteractionComponent->UsePickupItem();
}