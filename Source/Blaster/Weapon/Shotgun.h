// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HitScanWeapon.h"
#include "Shotgun.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AShotgun : public AHitScanWeapon
{
	GENERATED_BODY()
public:
	virtual void FireShotgun(const FVector_NetQuantize& SocketLocation, const TArray<FVector_NetQuantize>& HitTargets);
	void ShotgunTraceEndWithScatter(const FVector& TraceStart, const FVector_NetQuantize& HitTarget,TArray<FVector_NetQuantize>& HitTargets);
private:
	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	int32 NumberOfPellets = 10;

};
