// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "WeaponTypes.h"

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
		if (HasAuthority() && InstigatorController)
		{
			UGameplayStatics::ApplyDamage(
				BlasterCharacter,
				Damage,
				InstigatorController,
				this,
				UDamageType::StaticClass()
			);
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
		FVector End = bUseScatter ? TraceEndWithScatter(SocketLocation, HitTarget) : SocketLocation + (HitTarget - SocketLocation) * 1.25f;

		World->LineTraceSingleByChannel(
			OutHit,
			SocketLocation,
			End,
			ECollisionChannel::ECC_Visibility
		);



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

FVector AHitScanWeapon::TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget)
{
	FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;
	FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
	FVector EndLoc = SphereCenter + RandVec;
	FVector ToEndLoc = EndLoc - TraceStart;

	//DrawDebugSphere(GetWorld(), SphereCenter, SphereRadius, 12, FColor::Red, true);
	//DrawDebugSphere(GetWorld(), EndLoc, 4.f, 12, FColor::Green, true);
	//DrawDebugLine(GetWorld(),
	//	TraceStart,
	//	FVector(TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size()),
	//	FColor::Cyan,
	//	true
	//);

	return FVector(TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size());
}