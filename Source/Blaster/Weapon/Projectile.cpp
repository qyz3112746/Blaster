// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/Blaster.h"
#include "Net/UnrealNetwork.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);
	CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECollisionResponse::ECR_Block);
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	if (Tracer)
	{
		TracerComponent = UGameplayStatics::SpawnEmitterAttached(
			Tracer,
			CollisionBox,
			FName(),
			GetActorLocation(),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition
		);
	}

	if (HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this,&AProjectile::OnHit);
	}
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	bool bHitted = false;
	if (BlasterCharacter)
	{
		bHitted = true;
	}
	MulticastHitEffect(bHitted, Hit.ImpactPoint);
}

void AProjectile::MulticastHitEffect_Implementation(bool bHitted, const FVector_NetQuantize& HitPoint)
{
	if (!bHitted)
	{
		if (ImpactParticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, HitPoint);
		}
		if (ImpactSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, HitPoint);
		}
	}
	else
	{
		if (HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, HitSound, HitPoint);
		}
	}
	Destroy();
}

void AProjectile::Destroyed()
{
	Super::Destroyed();

	//if (!bHitted)
	//{
	//	if (ImpactParticles)
	//	{
	//		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, GetActorTransform());
	//	}
	//	if (ImpactSound)
	//	{
	//		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	//	}
	//}
	//else
	//{
	//	if (HitSound)
	//	{
	//		UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
	//	}
	//}
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


