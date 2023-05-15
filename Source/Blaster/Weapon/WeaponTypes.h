#pragma once

#define TRACE_LENGTH 80000.f

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_AssaultRifle UMETA(DisplauName = "Assault Rifle"),
	EWT_RocketLauncher UMETA(DisplauName = "Rocket Launcher"),
	EWT_Pistol UMETA(DisplauName = "Pistol"),
	EWT_SubmachineGun UMETA(DisplauName = "Submachine Gun"),
	EWT_Shotgun UMETA(DisplauName = "Shotgun"),
	EWT_SniperRifle UMETA(DisplauName = "Sniper Rifle"),
	EWT_GrenadeLaunch UMETA(DisplauName = "Grenade Launch"),

	EWT_MAX UMETA(DisplauName = "DefaultMAX")
};