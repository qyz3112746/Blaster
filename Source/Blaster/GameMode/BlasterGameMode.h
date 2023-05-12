// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BlasterGameMode.generated.h"

namespace MatchState
{
	extern BLASTER_API const FName Cooldown; // Match duration has been reached. Display winnerand begin cooldown timer.
}

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	ABlasterGameMode();
	virtual void PlayerEliminated(class ABlasterCharacter* ElimmedCharacter, class ABlasterPlayerController* VictimComtroller, ABlasterPlayerController* AttackerController);
	virtual void RequestRespawn(class ACharacter* ElimmedCharacter, AController* ElimmedController);
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 10.f;

	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 120.f;
	float LevelStartingTime = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float CooldownTime = 10.f;

protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;

private:
	float CountdownTime = 0.f;
public:
	FORCEINLINE float GetCountdownTime() const { return CountdownTime; }
};
