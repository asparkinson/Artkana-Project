// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "TimeManager.generated.h"

UCLASS()
class ATimeManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATimeManager();

	// Returns the singleton instance of TimeManager
	static ATimeManager* GetInstance() { return Instance; }

	// Lerps global time dilation toward TargetDilation using exponential decay/growth.
	// DecayRate controls how fast it approaches: higher = faster convergence.
	UFUNCTION(BlueprintCallable, Category = "Time")
	void TimeLerp(float TargetDilation, float DecayRate);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	static ATimeManager* Instance;

	bool   bIsLerping        = false;
	float  LerpTargetDilation = 1.0f;
	float  LerpDecayRate      = 5.0f;
};
