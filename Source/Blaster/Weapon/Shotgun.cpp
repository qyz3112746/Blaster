// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blaster/BlasterComponents/LagCompensationComponent.h"

void AShotgun::FireShotgun(const FVector_NetQuantize& SocketLocation, const TArray<FVector_NetQuantize>& HitTargets)
{
	AWeapon::Fire(SocketLocation, FVector());
	auto World = GetWorld();

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr)
	{
		return;
	}
	AController* InstigatorController = OwnerPawn->GetController();
	
	TMap <ABlasterCharacter*, uint32> HitMap;
	TMap <ABlasterCharacter*, uint32> HeadShotHitMap;

	for (auto HitTarget : HitTargets)
	{
		FHitResult FireHit;
		WeaponTraceHit(SocketLocation, HitTarget, FireHit);
		ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
		if (BlasterCharacter)
		{
			const bool bHeadShot = FireHit.BoneName.ToString() == FString("head");
			if (bHeadShot)
			{
				if (HeadShotHitMap.Contains(BlasterCharacter)) HeadShotHitMap[BlasterCharacter]++;
				else HeadShotHitMap.Emplace(BlasterCharacter, 1);
			}
			else
			{
				if (HitMap.Contains(BlasterCharacter)) HitMap[BlasterCharacter]++;
				else HitMap.Emplace(BlasterCharacter, 1);
			}
			if (HitSound)
			{
				UGameplayStatics::PlaySoundAtLocation(
					this,
					HitSound,
					FireHit.ImpactPoint,
					0.5f,
					FMath::FRandRange(-0.5f, 0.5f)
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
					FireHit.ImpactPoint,
					0.5f,
					FMath::FRandRange(-0.5f, 0.5f)
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
	}
	TArray<ABlasterCharacter*> HitCharacters;
	BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlasterCharacter>(OwnerPawn) : BlasterOwnerCharacter;
	BlasterOwnerController = BlasterOwnerController == nullptr ? Cast<ABlasterPlayerController>(InstigatorController) : BlasterOwnerController;
	if (BlasterOwnerCharacter == nullptr || BlasterOwnerController == nullptr)
	{
		return;
	}
	// Maps Character hit to total damage
	TMap <ABlasterCharacter*, float> DamageMap;

	// Calculate body shot damage by multiplying times hit x Damage - shore in DamageMap
	for (auto HitPair : HitMap)
	{
		if (InstigatorController && HitPair.Key)
		{
			DamageMap.Emplace(HitPair.Key, Damage * float(HitPair.Value));
		}
	}

	// Calculate head shot damage by multiplying times hit x Damage - shore in DamageMap
	for (auto HitPair : HeadShotHitMap)
	{
		if (InstigatorController && HitPair.Key)
		{
			if (DamageMap.Contains(HitPair.Key))
			{
				DamageMap[HitPair.Key] += HeadShotDamage * float(HitPair.Value);
			}
			else
			{
				DamageMap.Emplace(HitPair.Key,HeadShotDamage * float(HitPair.Value));
			}
		}
	}

	// Loop through DamageMap to get total damage for each character
	for (auto DamagePair : DamageMap)
	{
		if (InstigatorController && DamagePair.Key)
		{
			HitCharacters.Add(DamagePair.Key);
			if (HasAuthority() && (!bUseServerSideRewind || BlasterOwnerCharacter->IsLocallyControlled()))
			{
				UGameplayStatics::ApplyDamage(
					DamagePair.Key,
					DamagePair.Value,
					InstigatorController,
					this,
					UDamageType::StaticClass()
				);
			}
		}
	}
	if (bUseServerSideRewind)
	{
		if (!HasAuthority())
		{
			if (BlasterOwnerCharacter->GetLagCompensationComponent() && 
				BlasterOwnerCharacter->IsLocallyControlled())
			{
				BlasterOwnerCharacter->GetLagCompensationComponent()->ShotgunServerScoreRequest(
					HitCharacters,
					SocketLocation,
					HitTargets,
					BlasterOwnerController->GetServerTime() - BlasterOwnerController->SingleTripTime
				);
			}
		}
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

void AShotgun::ShotgunTraceEndWithScatter(const FVector& TraceStart, const FVector_NetQuantize& HitTarget, TArray<FVector_NetQuantize>& HitTargets)
{
	const FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	const FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;
	for (size_t i = 0; i < NumberOfPellets; i++)
	{
		const FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
		const FVector EndLoc = SphereCenter + RandVec;
		const FVector ToEndLoc = EndLoc - TraceStart;
		FVector Target = TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size();
		HitTargets.Add(Target);
	}
}
