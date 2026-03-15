// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityBase.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
AAbilityBase::AAbilityBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAbilityBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		Player = Cast<ACharacter>(PC->GetPawn());
	}

	TimeManager = ATimeManager::GetInstance();
}

bool AAbilityBase::CheckMana()
{
	return true;
	//return Player->ManaCurrentVal >= ManaCost;
}

void AAbilityBase::ActivateAbility()
{
	if (!CheckMana() || bHasActivated)
	{
		return;
	}

	bHasActivated = true;

	//Player->ManaCurrentVal -= ManaCost;

	DoAbility_Implementation();
}

void AAbilityBase::ResetAbility()
{
	bHasActivated = false;
}

void AAbilityBase::DoAbility_Implementation()
{
	// This function is meant to be overridden in Blueprints
}

