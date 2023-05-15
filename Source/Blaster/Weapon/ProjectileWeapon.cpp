// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Projectile.h"

void AProjectileWeapon::Fire(const FVector_NetQuantize& SocketLocation, const FVector_NetQuantize& HitTarget)
{
	Super::Fire(SocketLocation, HitTarget);
	if (!HasAuthority())
	{
		return;
	}

	// From muzzle flash socket to hit location from TraceUnderCrosshairs
	ServerSpawnProjectile(SocketLocation, HitTarget);
}

void AProjectileWeapon::ServerSpawnProjectile(const FVector_NetQuantize& SocketLocation, const FVector_NetQuantize& HitTarget)
{
	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	// From muzzle flash socket to hit location from TraceUnderCrosshairs
	FVector ToTarget = HitTarget - SocketLocation;
	FRotator TargetRotation = ToTarget.Rotation();
	if (ProjectileClass && InstigatorPawn)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = InstigatorPawn;
		UWorld* World = GetWorld();
		if (World)
		{
			World->SpawnActor<AProjectile>(
				ProjectileClass,
				SocketLocation,
				TargetRotation,
				SpawnParams
			);
		}
	}
}

