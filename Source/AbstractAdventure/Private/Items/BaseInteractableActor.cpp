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

	BrokenMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BrokenMeshComponent"));
	BrokenMeshComp->SetupAttachment(RootComponent);

	ItemCollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("ItemCollisionComponent"));
	ItemCollisionComp->SetCollisionProfileName(TEXT("Trigger"));
	ItemCollisionComp->SetupAttachment(RootComponent);

	ItemMuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("ItemMuzzleLocation"));
	ItemMuzzleLocation->SetupAttachment(RootComponent);
	ItemMuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	ItemFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ItemEffect"));
	ItemFXComponent->SetupAttachment(ItemMuzzleLocation);

	bToggled = false;

	bBroken = false;
	bCanBeCharged = false;
	bCharged = false;

	bCanBePickedUp = false;
	bCanBeUsedPickedUp = false;

	bStationary = false;
	bRepairItem = false;
	bCharger = false;

	bHolding = false;
	bGravity = true;

	ChargesAmount = 0;
}


// Called when the game starts or when spawned
void ABaseInteractableActor::BeginPlay()
{
	Super::BeginPlay();

	SetBeginItemCondition();
	SetBaseDynamicMaterial();

	if (ItemFXComponent) { ItemFXComponent->Deactivate(); } // Maybe there is a better way ...
	ItemCollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ABaseInteractableActor::OnHit);
}


void ABaseInteractableActor::SetBaseDynamicMaterial()
{
	int32 BaseMaterialIndex = 0;
	UMaterialInterface* BaseItemMaterial = ItemMeshComp->GetMaterial(BaseMaterialIndex);
	ItemBaseDynamicMaterial = UMaterialInstanceDynamic::Create(BaseItemMaterial, NULL);
	ItemMeshComp->SetMaterial(BaseMaterialIndex, ItemBaseDynamicMaterial);
}


void ABaseInteractableActor::SetBeginItemCondition()
{
	if (bBroken)
	{
		BrokenMeshComp->SetHiddenInGame(false);
		ItemMeshComp->SetHiddenInGame(true);
		bCharged = false;
		bCanBePickedUp = false;
		bCanBeUsedPickedUp = false;
		bStationary = true;
		bRepairItem = false;
		bCharger = false;
		//UE_LOG(LogTemp, Warning, TEXT("bBroken state switched!"));
	}
	else
	{
		PlayFX();
		ItemMeshComp->SetHiddenInGame(false);
		BrokenMeshComp->SetHiddenInGame(true);
	}
	if (bCanBeCharged)
	{
		bCharger = false;
	}
	if (bCharged)
	{
		ChargesAmount = 5;
	}
	if (bStationary)
	{
		bCharger = false;
		bCanBePickedUp = false;
		bCanBeUsedPickedUp = false;
		//UE_LOG(LogTemp, Warning, TEXT("bStationary state switched!"));
	}
	if (bCanBePickedUp)
	{
		bStationary = false;
		//UE_LOG(LogTemp, Warning, TEXT("bCanBePickedUp state switched!"));
	}
	if (bCanBeUsedPickedUp)
	{
		bCharger = false;
		bStationary = false;
		bCanBePickedUp = true;
		//UE_LOG(LogTemp, Warning, TEXT("bCanBeUsedPickedUp state switched!"));
	}
	if (bCharger)
	{
		bStationary = false;
		bCanBePickedUp = true;
		bCanBeUsedPickedUp = false;
		//UE_LOG(LogTemp, Warning, TEXT("bCharger state switched!"));
	}
	if (bRepairItem)
	{
		bCharger = false;
		bStationary = false;
		bCanBePickedUp = true;
		bCanBeUsedPickedUp = true;
		//UE_LOG(LogTemp, Warning, TEXT("bRepairItem state switched!"));
	}
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


void ABaseInteractableActor::UseItem() // TODO Rewrite this
{
	if (bCanBeUsedPickedUp)
	{
		if (ChargesAmount)
		{
			bCharged = true;

			PlayFX();

			ChargesAmount--;

			if (!ChargesAmount) 
			{
				bCharged = false;

				UE_LOG(LogTemp, Warning, TEXT("No charges left!"));
			}

			UE_LOG(LogTemp, Warning, TEXT("charges: %i"), ChargesAmount);
		}
	}
}


void ABaseInteractableActor::Toggle()
{
	if(!bBroken)
	{
		if(bCanBeCharged)
		{
			if (bCharged)
			{
				bToggled = !bToggled; // set to NOT (current state)
				ContactReferencedItemActor();
			}
		}
		else
		{
			bToggled = !bToggled; // set to NOT (current state)
			ContactReferencedItemActor();
		}
	}
}


void ABaseInteractableActor::OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *OtherActor->GetName());
	Charge(OtherActor);
}


void ABaseInteractableActor::Charge(AActor* OtherActor)
{
	if (bStationary && !bBroken && !bCharged)
	{
		if (OtherActor)
		{
			ABaseInteractableActor* ChargerItem = Cast<ABaseInteractableActor>(OtherActor);

			if (ChargerItem->bCharger)
			{
				this->bCharged = true;
				this->ChargesAmount = 10;
				ChargerItem->Destroy();
				UE_LOG(LogTemp, Warning, TEXT(" %s charged %s !"), *ChargerItem->GetName(), *this->GetName());
			}
		}
	}
}


void ABaseInteractableActor::PlayFX()
{
	if (ItemFXComponent) { ItemFXComponent->ResetSystem(); } // Maybe there is a better way ...
}


void ABaseInteractableActor::ContactReferencedItemActor()
{
	ABaseInteractableActor* ResultActor = Cast<ABaseInteractableActor>(GetLoadedActor());

	if (ResultActor)
	{
		// As an example ResultActor->SwitchMaterial();
		UE_LOG(LogTemp, Warning, TEXT("%s - respond!"), *ResultActor->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("no Referenced Item Actor!"));
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