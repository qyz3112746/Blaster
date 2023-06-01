// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatWidget.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UChatWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void SendMessage(const FText& Text, ETextCommit::Type CommitMethod);
public:
	void ReceiveMessage(FText Message);

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UListView* ChatListView;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UEditableTextBox* ChatInputBox;

	UPROPERTY(BlueprintReadWrite)
	class UItemData* CurrentItem;

	UPROPERTY()
	class ABlasterPlayerController* Controller;
};
