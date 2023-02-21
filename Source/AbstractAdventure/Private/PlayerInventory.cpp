// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInventory.h"

// Sets default values for this component's properties
UPlayerInventory::UPlayerInventory()
{
	PrimaryComponentTick.bCanEverTick = false;
	AmmoAmount = 4;
	BatteriesAmount = 0;
}


// Called when the game starts
void UPlayerInventory::BeginPlay()
{
	Super::BeginPlay();
}


void UPlayerInventory::AmmoPool()
{
	//
}


void UPlayerInventory::BatteriesPool()
{
	//
}