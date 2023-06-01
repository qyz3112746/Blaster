// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemData.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class BLASTER_API UItemData : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "default", meta = (ExposeOnSpawn = "true"))
	FText Content;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "default", meta = (ExposeOnSpawn = "true"))
	bool bSelected;
};
