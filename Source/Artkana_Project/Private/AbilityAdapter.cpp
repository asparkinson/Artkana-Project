// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityAdapter.h"

UAbilityAdapter::UAbilityAdapter()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAbilityAdapter::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("[AbilityAdapter] BeginPlay on owner: %s"), *GetOwner()->GetName());
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan,
			FString::Printf(TEXT("[AbilityAdapter] Initialized on %s"), *GetOwner()->GetName()));
	}
}

void UAbilityAdapter::TriggerAbility1()
{
	UE_LOG(LogTemp, Warning, TEXT("[AbilityAdapter] TriggerAbility1 called on %s | %d listeners"),
		*GetOwner()->GetName(), OnAbility1Triggered.GetAllObjects().Num());

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green,
			FString::Printf(TEXT("[AbilityAdapter] TriggerAbility1 -> %d listeners"), OnAbility1Triggered.GetAllObjects().Num()));
	}

	OnAbility1Triggered.Broadcast();
}

void UAbilityAdapter::TriggerAbility2()
{
	OnAbility2Triggered.Broadcast();
}
