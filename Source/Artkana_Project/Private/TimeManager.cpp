// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeManager.h"

// Sets default values
ATimeManager::ATimeManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Static instance definition
ATimeManager* ATimeManager::Instance = nullptr;

// Called when the game starts or when spawned
void ATimeManager::BeginPlay()
{
	Super::BeginPlay();
	Instance = this;
}

// Called when the actor is destroyed or the level ends
void ATimeManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (Instance == this)
	{
		Instance = nullptr;
	}
}

void ATimeManager::TimeLerp(float TargetDilation, float DecayRate)
{
	LerpTargetDilation = TargetDilation;
	LerpDecayRate      = DecayRate;
	bIsLerping         = true;

	// Compensate immediately so this actor keeps ticking at real-time
	float CurrentDilation = UGameplayStatics::GetGlobalTimeDilation(GetWorld());
	if (CurrentDilation > SMALL_NUMBER)
	{
		CustomTimeDilation = 1.0f / CurrentDilation;
	}
}

// Called every frame
void ATimeManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsLerping) return;

	float CurrentDilation = UGameplayStatics::GetGlobalTimeDilation(GetWorld());

	// Exponential approach: fast when far from target, slow when close
	// Formula: Current + (Target - Current) * (1 - e^(-Rate * dt))
	float NewDilation = FMath::Lerp(
		CurrentDilation,
		LerpTargetDilation,
		1.0f - FMath::Exp(-LerpDecayRate * DeltaTime)
	);

	// Snap to target once close enough to avoid infinite crawl
	if (FMath::IsNearlyEqual(NewDilation, LerpTargetDilation, 0.001f))
	{
		NewDilation = LerpTargetDilation;
		bIsLerping  = false;
	}

	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), NewDilation);

	// Keep this actor running at real-time so Tick fires accurately
	if (NewDilation > SMALL_NUMBER)
	{
		CustomTimeDilation = 1.0f / NewDilation;
	}
}
