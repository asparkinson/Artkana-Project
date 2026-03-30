// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UAbilityBase::UAbilityBase()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void UAbilityBase::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("[AbilityBase] BeginPlay | Class=%s Owner=%s"),
		*GetClass()->GetName(), *GetOwner()->GetName());
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor::Yellow,
		FString::Printf(TEXT("[%s] BeginPlay on %s"), *GetClass()->GetName(), *GetOwner()->GetName()));

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		if (APawn* Pawn = PC->GetPawn())
		{
			ManaComponent = Pawn->FindComponentByClass<UMana>();

			if (UAbilityAdapter* Adapter = Pawn->FindComponentByClass<UAbilityAdapter>())
			{
				Adapter->OnAbility1Triggered.AddDynamic(this, &UAbilityBase::HandleAbility1);
				Adapter->OnAbility2Triggered.AddDynamic(this, &UAbilityBase::HandleAbility2);

				UE_LOG(LogTemp, Warning, TEXT("[%s] Subscribed to AbilityAdapter on %s"),
					*GetClass()->GetName(), *Pawn->GetName());
				if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor::Green,
					FString::Printf(TEXT("[%s] Subscribed to AbilityAdapter on %s"), *GetClass()->GetName(), *Pawn->GetName()));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("[%s] AbilityAdapter NOT FOUND on pawn %s!"),
					*GetClass()->GetName(), *Pawn->GetName());
				if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor::Red,
					FString::Printf(TEXT("[%s] AbilityAdapter NOT FOUND on %s!"), *GetClass()->GetName(), *Pawn->GetName()));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[%s] No pawn found for PlayerController!"), *GetClass()->GetName());
		}
	}

	TimeManager = ATimeManager::GetInstance();
}

bool UAbilityBase::CheckMana()
{
	return ManaComponent && ManaComponent->ManaCurrVal >= ManaCost;
}

void UAbilityBase::ActivateAbility()
{
	if (!CheckMana() || bHasActivated)
	{
		return;
	}

	bHasActivated = true;

	ManaComponent->RemoveMana(ManaCost);

	UE_LOG(LogTemp, Warning, TEXT("[%s] ActivateAbility -> DoAbility()"), *GetClass()->GetName());
	DoAbility();
}

void UAbilityBase::ResetAbility()
{
	bHasActivated = false;
}

void UAbilityBase::HandleAbility1()
{
	UE_LOG(LogTemp, Warning, TEXT("[%s] HandleAbility1 | bIsPrimary=%s"),
		*GetClass()->GetName(),
		bIsPrimary ? TEXT("true") : TEXT("false"));

	if (bIsPrimary)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] HandleAbility1 -> ActivateAbility | Mana=%.1f ManaCost=%.1f bHasActivated=%s"),
			*GetClass()->GetName(),
			ManaComponent ? ManaComponent->ManaCurrVal : -1.f,
			ManaCost,
			bHasActivated ? TEXT("true") : TEXT("false"));

		ActivateAbility();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] HandleAbility1 skipped — bIsPrimary is false"), *GetClass()->GetName());
	}
}

void UAbilityBase::HandleAbility2()
{
	UE_LOG(LogTemp, Warning, TEXT("[%s] HandleAbility2 | bIsSecondary=%s"),
		*GetClass()->GetName(),
		bIsSecondary ? TEXT("true") : TEXT("false"));

	if (bIsSecondary)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] HandleAbility2 -> ActivateAbility | Mana=%.1f ManaCost=%.1f bHasActivated=%s"),
			*GetClass()->GetName(),
			ManaComponent ? ManaComponent->ManaCurrVal : -1.f,
			ManaCost,
			bHasActivated ? TEXT("true") : TEXT("false"));

		ActivateAbility();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] HandleAbility2 skipped — bIsSecondary is false"), *GetClass()->GetName());
	}
}

void UAbilityBase::DoAbility_Implementation()
{
	// This function is meant to be overridden in Blueprints
}

void UAbilityBase::ResumeTime()
{
	TimeManager->TimeLerp(1.0f, 5.0f); // Smoothly return to normal time dilation
}
