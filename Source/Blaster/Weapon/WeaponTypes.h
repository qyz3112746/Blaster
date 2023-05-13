#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_AssaultRifle UMETA(DisplauName = "Assault Rifle"),
	EWT_RocketLauncher UMETA(DisplauName = "Rocket Launcher"),

	EWT_MAX UMETA(DisplauName = "DefaultMAX")
};