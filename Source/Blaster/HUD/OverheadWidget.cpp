// Fill out your copyright notice in the Description page of Project Settings.


#include "OverheadWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "TimerManager.h"

void UOverheadWidget::SetDisplayText(FString TextToDisplay)
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}
}

void UOverheadWidget::ShowPlayerNetRole(APawn* InPawn)
{
	ENetRole RemoteRole = InPawn->GetRemoteRole();
	FString Role;
	switch (RemoteRole)
	{
	case ENetRole::ROLE_Authority:
		Role = FString("Authority");
		break;
	case ENetRole::ROLE_AutonomousProxy:
		Role = FString("AutonomousProxy");
		break;
	case ENetRole::ROLE_SimulatedProxy:
		Role = FString("SimulatedProxy");
		break;
	case ENetRole::ROLE_None:
		Role = FString("None");
		break;
	}
	FString RemoteRoleString = FString::Printf(TEXT("Remote Role: %s"), *Role);
	SetDisplayText(RemoteRoleString);
}

void UOverheadWidget::UpdataPlayerName(APawn* InPawn)
{
	LocalPawn = InPawn;
	APlayerState* PlayerState = InPawn->GetPlayerState();
	FString Name;
	if (PlayerState) 
	{
		Name = PlayerState->GetPlayerName();
	}
	else
	{
		StartUpdateTimer(InPawn);
	}
	FString CharacterNameString = FString::Printf(TEXT("Character Name: %s"), *Name);
	SetDisplayText(CharacterNameString);
}


void UOverheadWidget::NativeDestruct()
{
	RemoveFromParent();
	Super::NativeDestruct();
}

void UOverheadWidget::StartUpdateTimer(APawn* InPawn)
{
	InPawn->GetWorldTimerManager().SetTimer(
		UpdateTimer,
		this,
		&UOverheadWidget::UpdateTimerFinished,
		2.f
	);
}

void UOverheadWidget::UpdateTimerFinished()
{
	UpdataPlayerName(LocalPawn);
}
