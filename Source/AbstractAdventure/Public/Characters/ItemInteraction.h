// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemInteraction.generated.h"

class ABaseInteractableActor;
class UTraceForwardComponent;
class AAACharacterBase;


UCLASS()
class ABSTRACTADVENTURE_API AItemInteraction : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemInteraction();

};
