// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InputAction.h"
#include "Private/TimeManager.h"
#include "Public/Mana.h"
#include "AbilityBase.generated.h"

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ARTKANA_PROJECT_API UAbilityBase : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this component's properties
	UAbilityBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	bool CheckMana();

	// Set to true if this is the primary ability slot; only then will IA_Ability1 trigger it
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	bool bIsPrimary = false;

	// Assign IA_Ability1 here in the Blueprint details panel
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* AbilityInputAction;

	void OnAbility1Input(const FInputActionValue& Value);

	// Assign IA_Ability2 here in the Blueprint details panel
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* AbilityInputAction2;

	void OnAbility2Input(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	float ManaCost = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Mana")
	UMana* ManaComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Time")
	ATimeManager* TimeManager;

	bool bHasActivated = false;

public:
	UFUNCTION(BlueprintCallable)
	virtual void ActivateAbility();

	UFUNCTION(BlueprintCallable)
	virtual void ResetAbility();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void DoAbility();
	virtual void DoAbility_Implementation();
	
	UFUNCTION(BlueprintCallable)
	void ResumeTime();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsSecondary;
};
