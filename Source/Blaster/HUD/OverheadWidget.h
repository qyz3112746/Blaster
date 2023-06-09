// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverheadWidget.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UOverheadWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetDisplayText(FString TextToDisplay);

	UFUNCTION(BlueprintCallable)
	void ShowPlayerNetRole(APawn* InPawn);

	UFUNCTION(BlueprintCallable)
	void UpdataPlayerName(APawn* InPawn);

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DisplayText;

protected:
	virtual void NativeDestruct() override;

private:
	FTimerHandle UpdateTimer;
	void StartUpdateTimer(APawn* InPawn);
	void UpdateTimerFinished();
	APawn* LocalPawn;
};
