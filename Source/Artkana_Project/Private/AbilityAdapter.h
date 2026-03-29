// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AbilityAdapter.generated.h"

/** Fired when an ability input action is triggered. No payload — listeners use their own context. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAbilityInputTriggered);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ARTKANA_PROJECT_API UAbilityAdapter : public UActorComponent
{
	GENERATED_BODY()

public:
	UAbilityAdapter();

	// ── Delegates ─────────────────────────────────────────────────────────────
	/** Broadcast when Ability1 is triggered. C++ uses AddDynamic; Blueprint uses Assign or bind in event graph. */
	UPROPERTY(BlueprintAssignable, Category = "Ability|Input")
	FOnAbilityInputTriggered OnAbility1Triggered;

	/** Broadcast when Ability2 is triggered. */
	UPROPERTY(BlueprintAssignable, Category = "Ability|Input")
	FOnAbilityInputTriggered OnAbility2Triggered;

	// ── Callable Events ───────────────────────────────────────────────────────
	/** Call this from your Blueprint input event (e.g. IA_Ability1 Triggered pin). */
	UFUNCTION(BlueprintCallable, Category = "Ability|Input")
	void TriggerAbility1();

	/** Call this from your Blueprint input event (e.g. IA_Ability2 Triggered pin). */
	UFUNCTION(BlueprintCallable, Category = "Ability|Input")
	void TriggerAbility2();

protected:
	virtual void BeginPlay() override;

};
