// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

void AShotgun::Fire(const FVector_NetQuantize& SocketLocation, const FVector_NetQuantize& HitTarget)
{
	AWeapon::Fire(SocketLocation, HitTarget);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr)
	{
		return;
	}
	AController* InstigatorController = OwnerPawn->GetController();

	for (size_t i = 1; i < NumberOfPellets; i++)
	{
		FVector End = TraceEndWithScatter(SocketLocation, HitTarget);
		OnceTracing(SocketLocation, End, InstigatorController);
	}
}
