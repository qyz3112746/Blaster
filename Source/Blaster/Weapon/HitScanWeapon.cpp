// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "WeaponTypes.h"
#include "Blaster/BlasterComponents/LagCompensationComponent.h"

#include "DrawDebugHelpers.h"

void AHitScanWeapon::Fire(const FVector_NetQuantize& SocketLocation, const FVector_NetQuantize& HitTarget)
{
	Super::Fire(SocketLocation,HitTarget);

	UWorld* World = GetWorld();

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr)
	{
		return;
	}
	AController* InstigatorController = OwnerPawn->GetController();

	FHitResult FireHit;
	WeaponTraceHit(SocketLocation, HitTarget, FireHit);

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
	if (BlasterCharacter)
	{
		if (InstigatorController)
		{
			if (HasAuthority() && !bUseServerSideRewind) // when not use SSR, only the server apply damage
			{
				UGameplayStatics::ApplyDamage(
					BlasterCharacter,
					Damage,
					InstigatorController,
					this,
					UDamageType::StaticClass()
				);
			}
			if(bUseServerSideRewind) // when use SSR, only the server apply damage
			{
				BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlasterCharacter>(OwnerPawn) : BlasterOwnerCharacter;
				BlasterOwnerController = BlasterOwnerController == nullptr ? Cast<ABlasterPlayerController>(InstigatorController) : BlasterOwnerController;
				if (!HasAuthority()) // if the client owner the weapon, send RPC to apply damage
				{
					if (BlasterOwnerCharacter && 
						BlasterOwnerController && 
						BlasterOwnerCharacter->GetLagCompensationComponent() && 
						BlasterOwnerCharacter->IsLocallyControlled())
					{
						BlasterOwnerCharacter->GetLagCompensationComponent()->ServerScoreRequest(
							BlasterCharacter,
							SocketLocation,
							FireHit.ImpactPoint,
							BlasterOwnerController->GetServerTime() - BlasterOwnerController->SingleTripTime,
							this
						);
					}
				}
				else // if the server owner the weapon, apply damage
				{
					if (BlasterOwnerCharacter && 
						BlasterOwnerController &&
						BlasterOwnerCharacter->IsLocallyControlled())
					{
						UGameplayStatics::ApplyDamage(
							BlasterCharacter,
							Damage,
							InstigatorController,
							this,
							UDamageType::StaticClass()
						);
					}
				}
			}
		}
		if (HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				HitSound,
				FireHit.ImpactPoint
			);
		}
	}
	else
	{
		if (ImpactSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				ImpactSound,
				FireHit.ImpactPoint
			);
		}
	}
	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			World,
			ImpactParticles,
			FireHit.ImpactPoint,
			FireHit.ImpactNormal.Rotation()
		);
	}
	if (MuzzleFlash)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			World,
			MuzzleFlash,
			SocketLocation
		);
	}
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			FireSound,
			SocketLocation
		);
	}
}

void AHitScanWeapon::WeaponTraceHit(const FVector_NetQuantize& SocketLocation, const FVector& HitTarget, FHitResult& OutHit)
{
	UWorld* World = GetWorld();

	if (World)
	{
		FVector End = SocketLocation + (HitTarget - SocketLocation) * 1.25f;

		bool bHit = World->LineTraceSingleByChannel(
			OutHit,
			SocketLocation,
			End,
			ECollisionChannel::ECC_Visibility
		);

		if (!bHit)
		{
			OutHit.ImpactPoint = End;
		}

		DrawDebugSphere(GetWorld(), OutHit.ImpactPoint, 4.f, 12, FColor::Orange, true);

		FVector BeamEnd = End;
		if (OutHit.bBlockingHit)
		{
			BeamEnd = OutHit.ImpactPoint;
		}
		if (BeamParticles)
		{
			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
				World,
				BeamParticles,
				SocketLocation
			);
			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}
		}
	}
}