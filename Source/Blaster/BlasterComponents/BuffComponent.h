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
	void Shield(float ShieldAmount, float BuffTime);
	void BuffSpeed(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffTime, ESpeedBuff BuffType);
	void BuffJump(float BuffJumpVelocity, float BuffTime);
	void SetInitialSpeeds(float BaseSpeed, float CrouchSpeed);
	void SetInitialJumpVelocity(float Velocity);

protected:
	virtual void BeginPlay() override;
	void HealRampUp(float DeltaTime);
	void ShieldAdding(float DeltaTime);

	void HealingTimerFinished(float Rate);
	void ShieldBuffTimerFinished(float Rate);

public:	

private:
	UPROPERTY()
	class ABlasterCharacter* Character;
	/**
	* Heal Buff
	*/
	bool bHealing = false;
	float HealingRate = 0.f;
	float AmountToHeal = 0.f;

	/**
	* Shield Buff
	*/
	bool bShieldBuff = false;
	float ShieldAddingRate = 0.f;
	float AmountToAddShield = 0.f;

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
