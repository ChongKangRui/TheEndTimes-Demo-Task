// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InventoryComponent.h"
#include "InventoryItem.generated.h"

class ACharacterBase;

UCLASS()
class THEENDTIMES_API AInventoryItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AInventoryItem();

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void OnItemUsed(const EInventoryItem item,ACharacterBase* inventoryOwner, UInventoryComponent* inventoryComponent);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void OnItemRemoved(const EInventoryItem item, ACharacterBase* inventoryOwner, UInventoryComponent* inventoryComponent);
};
