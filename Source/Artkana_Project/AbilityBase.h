// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Private/TimeManager.h"
#include "Private/AbilityAdapter.h"
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

	/** Set to true if this ability should respond to IA_Ability1 via AbilityAdapter. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	bool bIsPrimary = false;

	/** Set to true if this ability should respond to IA_Ability2 via AbilityAdapter. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	bool bIsSecondary = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	float ManaCost = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Mana")
	UMana* ManaComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Time")
	ATimeManager* TimeManager;

	bool bHasActivated = false;

	/** Called by AbilityAdapter's OnAbility1Triggered delegate. */
	UFUNCTION()
	void HandleAbility1();

	/** Called by AbilityAdapter's OnAbility2Triggered delegate. */
	UFUNCTION()
	void HandleAbility2();

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
};
