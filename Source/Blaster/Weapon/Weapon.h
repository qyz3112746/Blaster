// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponTypes.h"
#include "Blaster/BlasterTypes/Team.h"

#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState :uint8 
{
	EWS_Initial UMETA(DisplayName = "Initial State"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_EquippedSecondary UMETA(DisplayName = "Equipped Secondary"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),

	EWS_MAX UMETA(DisplayName = "DefaultMAX"),
};

UENUM(BlueprintType)
enum class EFireType :uint8
{
	EFT_HitScan UMETA(DisplayName = "Hit Scan Weapon"),
	EFT_Projectile UMETA(DisplayName = "Projectile Weapon"),
	EFT_Shotgun UMETA(DisplayName = "Shotgun"),

	EFT_MAX UMETA(DisplayName = "DefaultMAX"),
};

UCLASS()
class BLASTER_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Owner() override;
	void SetHUDAmmo();
	void ShowPickupWidget(bool bShowWidget);
	virtual void Fire(const FVector_NetQuantize& SocketLocation, const FVector_NetQuantize& HitTarget);
	virtual void Dropped();
	void SetWeaponState(EWeaponState State);
	void AddAmmo(int32 AmmoToAdd);
	FVector TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget);
	/**
	* Enable or Disable Custom Depth
	*/
	void EnableCustomDepth(bool bEnable);

	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	bool bUseScatter = false;

protected:
	virtual void BeginPlay() override;
	virtual void OnWeaponStateSet();
	virtual void OnEquipped();
	virtual void OnEquippedSecondary();
	virtual void OnDropped();

	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
		void OnSphereEndOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex
		);

public:
	/**
	*  Textures for the weapon crosshairs
	*/
	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	class UTexture2D* CrosshairsCenter;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	UTexture2D* CrosshairsLeft;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	UTexture2D* CrosshairsRight;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	UTexture2D* CrosshairsTop;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	UTexture2D* CrosshairsBottom;

	UPROPERTY(EditAnywhere)
	class USoundCue* EquipSound;

	bool bDestroyWeapon = false;

	UPROPERTY(EditAnywhere)
	EFireType FireType;

protected:
	/**
	* Trace end with Scatter
	*/
	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	float DistanceToSphere = 800.f;

	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	float SphereRadius = 75.f;

	UPROPERTY(EditAnywhere)
	float Damage = 20.f;

	UPROPERTY(EditAnywhere)
	float HeadShotDamage = 40.f;

	UPROPERTY(EditAnywhere)
	bool bOriginalUseServerSideRewind = false;

	UPROPERTY(Replicated)
	bool bUseServerSideRewind;

	UPROPERTY()
	class ABlasterCharacter* BlasterOwnerCharacter;
	UPROPERTY()
	class ABlasterPlayerController* BlasterOwnerController;

	UFUNCTION()
	void OnPingTooHigh(bool bPingTooHigh);

	UPROPERTY(EditAnywhere)
	ETeam Team = ETeam::ET_NoTeam;

private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class USphereComponent* AreaSphere;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState;

	UFUNCTION()
	void OnRep_WeaponState();

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent* PickupWidget;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	class UAnimationAsset* FireAnimation;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ACasing> CasingClass;

	/**
	* Zoomed FOV while aiming
	*/

	UPROPERTY(EditAnywhere)
	float ZoomedFOV = 30.f;

	UPROPERTY(EditAnywhere)
	float ZoomInterpSpeed = 20.f;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	float CrosshairInAirFactor = 2.25f;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	float CrosshairAimFactor = 0.58f;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	float CrosshairShootingFactor = 0.75f;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	float CrosshairShootingBackFactor = 4.f;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	float CrosshairShootingMaxFactor = 3.f;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	float CrosshairBasicSpread = 0.5;

	/**
	* Automatic fire
	*/
	UPROPERTY(EditAnywhere, Category = Combat)
	float FireDelay = .15f;

	UPROPERTY(EditAnywhere, Category = Combat)
	bool bAutomatic = true;

	UPROPERTY(EditAnywhere)
	int32 Ammo;

	UFUNCTION(Client,Reliable)
	void ClientUpdateAmmo(int32 ServerAmmo);

	// The number of unprocessed server requests for Ammo.
	// Incremented in SpendRound, decremented in ClientUpdateAmmo.
	int32 Sequence = 0;

	UFUNCTION(NetMulticast, Reliable)
	void ClientAddAmmo(int32 AmmoToAdd);

	void SpendRound();

	UPROPERTY(EditAnywhere)
	int32 MagCapacity;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;

public:
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; }

	FORCEINLINE float GetCrosshairInAirFactor() const { return CrosshairInAirFactor; }
	FORCEINLINE float GetCrosshairAimFactor() const { return CrosshairAimFactor; }
	FORCEINLINE float GetCrosshairShootingFactor() const { return CrosshairShootingFactor; }
	FORCEINLINE float GetCrosshairShootingBackFactor() const { return CrosshairShootingBackFactor; }
	FORCEINLINE float GetCrosshairShootingMaxFactor() const { return CrosshairShootingMaxFactor; }
	FORCEINLINE float GetCrosshairBasicSpread() const { return CrosshairBasicSpread; }

	FORCEINLINE float GetZoomedFOV() const {return ZoomedFOV;}
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }

	FORCEINLINE float GetFireDelay() const { return FireDelay; }
	FORCEINLINE bool IsAutomatic() const { return bAutomatic; }

	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE int32 GetMagCapacity() const { return MagCapacity; }

	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }

	FORCEINLINE float GetDamage() const { return Damage; }
	FORCEINLINE float GetHeadShotDamage() const { return HeadShotDamage; }

	FORCEINLINE ETeam GetTeam() const { return Team; }

	bool IsEmpty();
	bool IsFull();
};
