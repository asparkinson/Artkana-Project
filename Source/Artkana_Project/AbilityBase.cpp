// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityBase.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"

// Sets default values for this component's properties
UAbilityBase::UAbilityBase()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void UAbilityBase::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		if (APawn* Pawn = PC->GetPawn())
		{
			ManaComponent = Pawn->FindComponentByClass<UMana>();
		}
	}

	TimeManager = ATimeManager::GetInstance();

	// Bind IA_Ability1 on the owning actor's input component
	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(GetOwner()->InputComponent))
	{
		if (AbilityInputAction)
		{
			EIC->BindAction(AbilityInputAction, ETriggerEvent::Triggered,
				this, &UAbilityBase::OnAbility1Input);
		}

		if (AbilityInputAction2)
		{
			EIC->BindAction(AbilityInputAction2, ETriggerEvent::Triggered,
				this, &UAbilityBase::OnAbility2Input);
		}
	}
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

	DoAbility_Implementation();
}

void UAbilityBase::ResetAbility()
{
	bHasActivated = false;
}

void UAbilityBase::OnAbility1Input(const FInputActionValue& Value)
{
	if (bIsPrimary)
	{
		ActivateAbility();
	}
}

void UAbilityBase::OnAbility2Input(const FInputActionValue& Value)
{
	if (bIsSecondary)
	{
		ActivateAbility();
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
