// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatWidget.h"
#include "Components/Listview.h"
#include "Components/EditableTextBox.h"
#include "ItemData.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"

void UChatWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (ChatInputBox)
	{
		ChatInputBox->OnTextCommitted.AddDynamic(this, &UChatWidget::SendMessage);
	}
}

void UChatWidget::ReceiveMessage(FText Message)
{
	if (ChatListView)
	{
		UItemData* Item = NewObject<UItemData>();
		Item->Content = Message;
		ChatListView->AddItem(Item);
		ChatListView->ScrollToBottom();
	}
}

void UChatWidget::SendMessage(const FText& Text, ETextCommit::Type CommitMethod)
{
	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(GetWorld()->GetFirstPlayerController()) : Controller;
	if (Controller == nullptr) return;
	Controller->SendMessage(Text);
	Controller->SetInputMode(FInputModeGameOnly());
	if (!Text.IsEmpty())
	{
		ChatInputBox->SetText(FText());
	}
}
