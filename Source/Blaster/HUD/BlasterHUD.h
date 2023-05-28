// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BlasterHUD.generated.h"

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()
public:
	class UTexture2D* CrosshairsCenter;
	UTexture2D* CrosshairsLeft;
	UTexture2D* CrosshairsRight;
	UTexture2D* CrosshairsTop;
	UTexture2D* CrosshairsBottom;
	float CrosshairSpread;
	FLinearColor CrosshairsColor;
};

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterHUD : public AHUD
{
	GENERATED_BODY()
public:
	virtual void DrawHUD() override;

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	TSubclassOf<class UUserWidget> CharacterOverlayClass;

	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;
	void AddCharacterOverlay();

	UPROPERTY(EditAnywhere, Category = "Announcements")
	TSubclassOf<class UUserWidget> AnnouncementClass;

	UPROPERTY(EditAnywhere, Category = "ElimAnnouncements")
	TSubclassOf<class UUserWidget> ElimAnnouncementClass;

	UPROPERTY(EditAnywhere, Category = "ElimAnnouncements")
	TSubclassOf<class UUserWidget> ChatWidgetClass;

	UPROPERTY()
	class UChatWidget* ChatWidget;

	UPROPERTY()
	class UAnnouncement* Announcement;
	void AddAnnouncement();

	void AddElimAnnouncement(FString Attack, FString Victim);

	void AddChatWidget();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	APlayerController* OwningPlayer;

	FHUDPackage HUDPackage;

	UPROPERTY(Editanywhere)
	float CrosshairSpreadMax = 16.f;

private:
	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairsColor);

	UPROPERTY(EditAnywhere)
	float ElimAnnouncementTime = 5.f;

	UFUNCTION()
	void ElimAnnouncementTimerFinished(class UElimAnnouncement* MsgToRemove);

	UPROPERTY()
	TArray<UElimAnnouncement*> ElimMessages;


public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) { HUDPackage = Package; }
};
