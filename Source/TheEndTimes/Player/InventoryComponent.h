// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class AInventoryItem;
class ACharacterBase;

UENUM(BlueprintType)
enum EInventoryItem {
	None,
	Backpack,
	Apple,
	Banana,
	PistolArmor,
	Knife
};

/*Character base attribute, used for data collection*/
USTRUCT(Blueprintable)
struct FInventoryItemAttributeRow : public FTableRowBase {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UObject> itemImage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName itemName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AInventoryItem> itemBlueprint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float itemWeight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool usable;
};

USTRUCT(Blueprintable)
struct FInventoryStoredItem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<EInventoryItem> itemEnum;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FInventoryItemAttributeRow itemInfo;

	FInventoryStoredItem() : itemEnum(EInventoryItem::None), itemInfo(FInventoryItemAttributeRow()) {
	}

	FInventoryStoredItem(const TEnumAsByte<EInventoryItem> item, const FInventoryItemAttributeRow& itemAttribute) : itemEnum(item), itemInfo(itemAttribute) {
	}

};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THEENDTIMES_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

	UFUNCTION(BlueprintPure)
	const FInventoryItemAttributeRow& getItemAttributeRow(const TEnumAsByte<EInventoryItem> item) const;

	UFUNCTION(BlueprintPure)
	const TArray<FInventoryStoredItem>& getAllItem() const;

	UFUNCTION(BlueprintCallable)
	bool addItem(const EInventoryItem item);
	UFUNCTION(BlueprintCallable)
	void removeItem(const EInventoryItem item);
	UFUNCTION(BlueprintCallable)
	void useItem(const EInventoryItem item);

	UFUNCTION(BlueprintCallable)
	void SetMaxInventoryWeight(float newWeight);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float defaultsMaxInventoryWeight = 100.0f;

	/*Allow binding event to inventory event*/
	UPROPERTY(BlueprintAssignable)
	FOnInventoryChanged OnItemAdded;
	UPROPERTY(BlueprintAssignable)
	FOnInventoryChanged OnItemUsed;
	UPROPERTY(BlueprintAssignable)
	FOnInventoryChanged OnItemRemove;

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(BlueprintReadOnly)
	float currentInventoryWeight;

	UPROPERTY(BlueprintReadOnly)
	float currentMaxInventoryWeight;

private:
	/*Load the item list from datatable*/
	void dataTableAssetInitialization();

private:
	TArray<FInventoryStoredItem> m_Items;
	TWeakObjectPtr<UDataTable> m_DataTableAsset;
	TWeakObjectPtr<ACharacterBase> m_OwnerCharacter;
};
