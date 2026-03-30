// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/Mana.h"

// Sets default values for this component's properties
UMana::UMana()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMana::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UMana::AddToMana (float Amount)
{
	float PrevMana = ManaCurrVal;
	if (ManaCurrVal + Amount >= ManaMaxVal)
	{
		ManaCurrVal = ManaMaxVal;
	}
	else
	{
		ManaCurrVal += Amount;
	}
	bIsManaEmpty = false;

	UE_LOG(LogTemp, Log, TEXT("[Mana] %s | AddToMana: +%.1f | %.1f -> %.1f / %.1f"),
		*GetOwner()->GetName(), Amount, PrevMana, ManaCurrVal, ManaMaxVal);
}

void UMana::RemoveMana(float Amount)
{
	float PrevMana = ManaCurrVal;
	if (ManaCurrVal - Amount <= 0)
	{
		ManaCurrVal = 0;
		bIsManaEmpty = true;
	}
	else
	{
		ManaCurrVal -= Amount;
	}

	UE_LOG(LogTemp, Log, TEXT("[Mana] %s | RemoveMana: -%.1f | %.1f -> %.1f / %.1f%s"),
		*GetOwner()->GetName(), Amount, PrevMana, ManaCurrVal, ManaMaxVal,
		bIsManaEmpty ? TEXT(" [MANA EMPTY]") : TEXT(""));
}


