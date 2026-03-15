// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Private/TimeManager.h"
#include "AbilityBase.generated.h"

UCLASS()
class ARTKANA_PROJECT_API AAbilityBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAbilityBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	static bool CheckMana();

	UPROPERTY()
	ACharacter* Player;

	UPROPERTY()
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
	void ResumeTime()	{
		if (TimeManager)
		{
			TimeManager->CustomTimeDilation = 1.0f;
		}
	}
};
