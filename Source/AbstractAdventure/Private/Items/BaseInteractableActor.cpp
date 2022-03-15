// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/BaseInteractableActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Niagara/Public/NiagaraComponent.h"
#include "Niagara/Public/NiagaraFunctionLibrary.h"
#include "Engine/StreamableManager.h"


// Sets default values
ABaseInteractableActor::ABaseInteractableActor()
{
	ItemMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	ItemMeshComp->SetSimulatePhysics(true);
	ItemMeshComp->SetupAttachment(RootComponent);
	RootComponent = ItemMeshComp;

	ItemCollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("ItemCollisionComponent"));
	ItemCollisionComp->SetCollisionProfileName(TEXT("Trigger"));
	ItemCollisionComp->SetupAttachment(RootComponent);

	ItemMuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("ItemMuzzleLocation"));
	ItemMuzzleLocation->SetupAttachment(ItemMeshComp);
	ItemMuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	ItemFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ItemEffect"));
	ItemFXComponent->SetupAttachment(ItemMuzzleLocation);

	bBroken = false;
	bCharged = false;
	bToggled = false;

	bCanBePickedUp = false;
	bCanBeUsedPickedUp = false;
	bStationary = false;

	bHolding = false;
	bGravity = true;

	ChargesAmount = 5;

	Health = 0.0f;
	DefaultHealth = 100.0f;
	Health = DefaultHealth;
}


// Called when the game starts or when spawned
void ABaseInteractableActor::BeginPlay()
{
	Super::BeginPlay();

	SetBaseDynamicMaterial();

	if (ItemFXComponent) { ItemFXComponent->Deactivate(); } // Maybe there is a better way ...


}


void ABaseInteractableActor::SetBaseDynamicMaterial()
{
	int32 BaseMaterialIndex = 0;
	UMaterialInterface* BaseItemMaterial = ItemMeshComp->GetMaterial(BaseMaterialIndex);
	ItemBaseDynamicMaterial = UMaterialInstanceDynamic::Create(BaseItemMaterial, NULL);
	ItemMeshComp->SetMaterial(BaseMaterialIndex, ItemBaseDynamicMaterial);
}


void ABaseInteractableActor::SwitchMaterial()
{
	float OutValue = 0;
	ItemBaseDynamicMaterial->GetScalarParameterValue(TEXT("Switch"), OutValue);

	if (OutValue == 0.000000f)
	{
		ItemBaseDynamicMaterial->SetScalarParameterValue(TEXT("Switch"), 1);
	}
	else
	{
		ItemBaseDynamicMaterial->SetScalarParameterValue(TEXT("Switch"), 0);
	}
}


float ABaseInteractableActor::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Health -= DefaultHealth;
	UE_LOG(LogTemp, Log, TEXT("Health: %f"), Health);

	if (Health <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Health depleted!"));
	}

	return DamageAmount;
}


void ABaseInteractableActor::UseItem() // TODO Rewrite this
{
	if (bCanBeUsedPickedUp)
	{
		if (ChargesAmount)
		{
			if (ItemFXComponent) { ItemFXComponent->ResetSystem(); } // Maybe there is a better way ...
			ChargesAmount -= 1;
			UE_LOG(LogTemp, Warning, TEXT("%i"), ChargesAmount);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No charges left!"));
		}
	}
}


void ABaseInteractableActor::Toggle()
{
	bToggled = !bToggled; // set to NOT (current state)
	ContactReferencedItemActor();
}


void ABaseInteractableActor::ContactReferencedItemActor()
{
	ABaseInteractableActor* ResultActor = Cast<ABaseInteractableActor>(GetLoadedActor());

	if (ResultActor)
	{
		ResultActor->SwitchMaterial(); // TODO delete
		UE_LOG(LogTemp, Warning, TEXT("%s - respond!"), *ResultActor->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("no Referenced Item Actor!"));
	}
}


AActor* ABaseInteractableActor::GetLoadedActor() // To get refereced actor
{
	FStreamableManager Streamable;
	if (ReferencedItemActor.IsPending())
	{
		const FSoftObjectPath& AssetRef = ReferencedItemActor.ToString();
		ReferencedItemActor = Cast<AActor>(Streamable.LoadSynchronous(AssetRef));
	}
	return ReferencedItemActor.Get();
}