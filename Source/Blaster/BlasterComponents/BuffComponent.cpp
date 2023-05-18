// Fill out your copyright notice in the Description page of Project Settings.


#include "BuffComponent.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"

UBuffComponent::UBuffComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UBuffComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UBuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	HealRampUp(DeltaTime);
	ShieldAdding(DeltaTime);
}

void UBuffComponent::Heal(float HealAmount, float HealingTime)
{
	bHealing = true;
	float Rate = HealAmount / HealingTime;
	HealingRate += Rate;
	AmountToHeal += HealAmount;

	FTimerHandle TimerHandle;

	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle, 
		FTimerDelegate::CreateUObject(this, &UBuffComponent::HealingTimerFinished, Rate),
		HealingTime,
		false
	);
}

void UBuffComponent::Shield(float ShieldAmount, float BuffTime)
{
	bShieldBuff = true;
	float Rate = ShieldAmount / BuffTime;
	ShieldAddingRate += Rate;
	AmountToAddShield += ShieldAmount;

	FTimerHandle TimerHandle;

	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		FTimerDelegate::CreateUObject(this, &UBuffComponent::ShieldBuffTimerFinished, Rate),
		BuffTime,
		false
	);
}

void UBuffComponent::BuffSpeed(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffTime, ESpeedBuff BuffType)
{
	if (Character == nullptr)
	{
		return;
	}

	Character->GetWorldTimerManager().SetTimer(
		SpeedBuffTimer,
		this,
		&UBuffComponent::SpeedBuffFinished,
		BuffTime
	);

	if (Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BuffBaseSpeed;
		Character->GetCharacterMovement()->MaxWalkSpeedCrouched = BuffCrouchSpeed;
	}
	MulticastSpeedBuff(BuffBaseSpeed, BuffCrouchSpeed);
}

void UBuffComponent::SpeedBuffFinished()
{
	if (Character == nullptr || Character->GetCharacterMovement() == nullptr)
	{
		return;
	}
	Character->GetCharacterMovement()->MaxWalkSpeed = InitialBaseSpeed;
	Character->GetCharacterMovement()->MaxWalkSpeedCrouched = InitialCrouchSpeed;
	MulticastSpeedBuff(InitialBaseSpeed, InitialCrouchSpeed);
}

void UBuffComponent::MulticastSpeedBuff_Implementation(float BaseSpeed, float CrouchSpeed)
{
	if (Character == nullptr || Character->GetCharacterMovement() == nullptr)
	{
		return;
	}
	Character->GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
	Character->GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeed;
}

void UBuffComponent::BuffJump(float BuffJumpVelocity, float BuffTime)
{

	if (Character == nullptr)
	{
		return;
	}

	Character->GetWorldTimerManager().SetTimer(
		JumpBuffTimer,
		this,
		&UBuffComponent::JumpBuffFinished,
		BuffTime
	);

	if (Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->JumpZVelocity = BuffJumpVelocity;
	}
	MulticastJumpBuff(BuffJumpVelocity);
}

void UBuffComponent::JumpBuffFinished()
{
	if (Character == nullptr || Character->GetCharacterMovement() == nullptr)
	{
		return;
	}
	Character->GetCharacterMovement()->JumpZVelocity = InitialJumpVelocity;
	MulticastJumpBuff(InitialJumpVelocity);
}

void UBuffComponent::MulticastJumpBuff_Implementation(float Velocity)
{
	if (Character == nullptr || Character->GetCharacterMovement() == nullptr)
	{
		return;
	}
	Character->GetCharacterMovement()->JumpZVelocity = Velocity;
}

void UBuffComponent::SetInitialSpeeds(float BaseSpeed, float CrouchSpeed)
{
	InitialBaseSpeed = BaseSpeed;
	InitialCrouchSpeed = CrouchSpeed;
}

void UBuffComponent::SetInitialJumpVelocity(float Velocity)
{
	InitialJumpVelocity = Velocity;
}

void UBuffComponent::HealRampUp(float DeltaTime)
{
	if (!bHealing || Character == nullptr || Character->IsElimmed())
	{
		return;
	}

	const float HealThisFrame = FMath::Min(HealingRate * DeltaTime, AmountToHeal);
	Character->SetHealth(FMath::Clamp(Character->GetHealth() + HealThisFrame, 0.f, Character->GetMaxHealth()));
	Character->UpdateHUDHealth();
	AmountToHeal -= HealThisFrame;

	if (AmountToHeal <= 0.f)
	{
		bHealing = false;
		AmountToHeal = 0.f;
	}
}

void UBuffComponent::ShieldAdding(float DeltaTime)
{
	if (!bShieldBuff || Character == nullptr || Character->IsElimmed())
	{
		return;
	}

	const float AddingThisFrame = FMath::Min(ShieldAddingRate * DeltaTime, AmountToAddShield);
	Character->SetShield(FMath::Clamp(Character->GetShield() + AddingThisFrame, 0.f, Character->GetMaxShield()));
	Character->UpdateHUDShield();
	AmountToAddShield -= AddingThisFrame;

	if (AmountToAddShield <= 0.f)
	{
		bShieldBuff = false;
		AmountToAddShield = 0.f;
	}
}

void UBuffComponent::HealingTimerFinished(float Rate)
{
	HealingRate = HealingRate - Rate;
}

void UBuffComponent::ShieldBuffTimerFinished(float Rate)
{
	ShieldAddingRate = ShieldAddingRate - Rate;
}

