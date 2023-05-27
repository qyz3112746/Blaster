// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Projectile.h"

void AProjectileWeapon::Fire(const FVector_NetQuantize& SocketLocation, const FVector_NetQuantize& HitTarget)
{
	Super::Fire(SocketLocation, HitTarget);


	// From muzzle flash socket to hit location from TraceUnderCrosshairs
	SpawnProjectile(SocketLocation, HitTarget);
}

void AProjectileWeapon::SpawnProjectile(const FVector_NetQuantize& SocketLocation, const FVector_NetQuantize& HitTarget)
{
	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	// From muzzle flash socket to hit location from TraceUnderCrosshairs
	FVector ToTarget = HitTarget - SocketLocation;
	FRotator TargetRotation = ToTarget.Rotation();
	UWorld* World = GetWorld();
	if (World)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = InstigatorPawn;

		if (InstigatorPawn == nullptr)
		{
			return;
		}

		AProjectile* SpawnedProjectile = nullptr;
		if (bUseServerSideRewind)
		{
			if (InstigatorPawn->HasAuthority()) // server
			{
				if (InstigatorPawn->IsLocallyControlled()) //server, host - use replicated projectile
				{
					if (ProjectileClass)
					{
						SpawnedProjectile = World->SpawnActor<AProjectile>(ProjectileClass, SocketLocation, TargetRotation, SpawnParams);
						SpawnedProjectile->bUseServerSideRewind = false;
						SpawnedProjectile->Damage = Damage;
						SpawnedProjectile->HeadShotDamage = HeadShotDamage;
					}
				}
				else // server, not locally controlled - spawn non-replicated projectile, no SSR
				{
					if (ServerSideRewindProjectileClass)
					{
						SpawnedProjectile = World->SpawnActor<AProjectile>(ServerSideRewindProjectileClass, SocketLocation, TargetRotation, SpawnParams);
						SpawnedProjectile->bUseServerSideRewind = true;
					}
				}
			}
			else // client, using SSR
			{
				if (InstigatorPawn->IsLocallyControlled()) // client, locally controlled - spawn non-replicated projectile, use SSR
				{
					if (ServerSideRewindProjectileClass)
					{
						SpawnedProjectile = World->SpawnActor<AProjectile>(ServerSideRewindProjectileClass, SocketLocation, TargetRotation, SpawnParams);
						SpawnedProjectile->bUseServerSideRewind = true;
						SpawnedProjectile->TraceStart = SocketLocation;
						SpawnedProjectile->InitialVelocity = SpawnedProjectile->GetActorForwardVector() * SpawnedProjectile->InitialSpeed;
						SpawnedProjectile->Damage = Damage;
						SpawnedProjectile->HeadShotDamage = HeadShotDamage;
					}
				}
				else // client, not locally controlled - spawn non-replicated projectile, no SSR
				{
					if (ServerSideRewindProjectileClass)
					{
						SpawnedProjectile = World->SpawnActor<AProjectile>(ServerSideRewindProjectileClass, SocketLocation, TargetRotation, SpawnParams);
						SpawnedProjectile->bUseServerSideRewind = false;
					}
				}
			}
		}
		else // Weapon not using SSR
		{
			if (InstigatorPawn->HasAuthority()) //server, host - use replicated projectile
			{
				if (ProjectileClass)
				{
					SpawnedProjectile = World->SpawnActor<AProjectile>(ProjectileClass, SocketLocation, TargetRotation, SpawnParams);
					SpawnedProjectile->bUseServerSideRewind = false;
					SpawnedProjectile->Damage = Damage;
				}
			}
		}
	}
}

