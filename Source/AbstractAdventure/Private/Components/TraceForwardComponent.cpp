// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/TraceForwardComponent.h"
#include "Characters/AACharacterBase.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UTraceForwardComponent::UTraceForwardComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	TraceDistance = 500;
}


// Called when the game starts
void UTraceForwardComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UTraceForwardComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


void UTraceForwardComponent::TraceForward()
{
	Player = Cast<AAACharacterBase>(GetOwner());

	if (Player)
	{
		FVector Loc;
		FRotator Rot;
		FHitResult Hit;

		Player->GetController()->GetPlayerViewPoint(Loc, Rot);

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
			Player->GetInteractableItem(bHitByChannel, Hit);
			Player->GetStationaryItem(bHitByChannel, Hit);
		}
		else
		{
			Player->CurrentStationaryActor = NULL;
		}
	}
}