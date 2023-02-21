// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/TraceForwardComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UTraceForwardComponent::UTraceForwardComponent()
{
	TraceDistance = 250;
}


// Called when the game starts
void UTraceForwardComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UTraceForwardComponent::TraceForward(FVector Loc, FRotator Rot, FHitResult& Hit, bool& bHitByChannel)
{
	FVector Start = Loc;
	FVector End = Start + (Rot.Vector() * TraceDistance);

	FCollisionQueryParams TraceParams;
	FComponentQueryParams DefaultComponentQueryParams;
	FCollisionResponseParams DefaultResponseParams;
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, TraceParams);

	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 2.0f);

	if (bHit)
	{
		DrawDebugBox(GetWorld(), Hit.ImpactPoint, FVector(5, 5, 5), FColor::Emerald, false, 2.0f);
		bHitByChannel = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, DefaultComponentQueryParams, DefaultResponseParams);
	}
}