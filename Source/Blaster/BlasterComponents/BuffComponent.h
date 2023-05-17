// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Blaster/BlasterTypes/BuffTypes.h"

#include "BuffComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBuffComponent();
	friend class ABlasterCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void Heal(float HealAmount, float HealingTime);
	void BuffSpeed(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffTime, ESpeedBuff BuffType);
	void BuffJump(float BuffJumpVelocity, float BuffTime);
	void SetInitialSpeeds(float BaseSpeed, float CrouchSpeed);
	void SetInitialJumpVelocity(float Velocity);

protected:
	virtual void BeginPlay() override;
	void HealRampUp(float DeltaTime);

	UFUNCTION()
	void HealingTimerFinished(float Rate);

public:	

private:
	UPROPERTY()
	class ABlasterCharacter* Character;
	/**
	* Heal Buff
	*/
	bool bHealing = false;
	float HealingRate = 0;
	float AmountToHeal = 0.f;

	/**
	* Speed Buff
	*/
	FTimerHandle SpeedBuffTimer;
	void SpeedBuffFinished();
	float InitialBaseSpeed = 0.f;
	float InitialCrouchSpeed = 0.f;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpeedBuff(float BaseSpeed, float CrouchSpeed);

	/**
	* Jump Buff
	*/
	FTimerHandle JumpBuffTimer;
	void JumpBuffFinished();
	float InitialJumpVelocity = 0.f;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastJumpBuff(float Velocity);
};
