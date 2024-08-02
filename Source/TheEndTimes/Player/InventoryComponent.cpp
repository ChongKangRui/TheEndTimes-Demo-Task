// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "InventoryItem.h"
#include "Character/CharacterBase.h"
#include "Kismet/KismetStringLibrary.h"
// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	dataTableAssetInitialization();
}

void UInventoryComponent::BeginPlay()
{
	SetMaxInventoryWeight(defaultsMaxInventoryWeight);
	m_OwnerCharacter = Cast<ACharacterBase>(GetOwner());
}

bool UInventoryComponent::addItem(const EInventoryItem item)
{
	const FInventoryItemAttributeRow& itemAttribute = getItemAttributeRow(item);
	if (currentInventoryWeight + itemAttribute.itemWeight > currentMaxInventoryWeight) {
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent: Weight not allow to add item"));
		return false;
	}

	int32 insertIndex = m_Items.IndexOfByPredicate([&](const FInventoryStoredItem& storedItem) {
		return storedItem.itemInfo.itemWeight > itemAttribute.itemWeight;
		});

	FInventoryStoredItem itemToAdd = FInventoryStoredItem(item, itemAttribute);

	if (insertIndex == INDEX_NONE) {
		m_Items.Add(itemToAdd);
	}
	else {
		m_Items.Insert(itemToAdd, insertIndex);
	}
	currentInventoryWeight += itemAttribute.itemWeight;

	UE_LOG(LogTemp, Warning, TEXT("InventoryComponent: Add Item Success"));
	OnItemAdded.Broadcast();

	return true;
}

void UInventoryComponent::removeItem(const EInventoryItem item)
{
	const FInventoryItemAttributeRow& itemAttribute = getItemAttributeRow(item);

	int itemToRemove = m_Items.IndexOfByPredicate([&](const FInventoryStoredItem& storedItem) {
		return storedItem.itemEnum == item;
		});

	if (itemToRemove == INDEX_NONE) {
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent: Couldn't find item to remove"));
		return;
	}

	currentInventoryWeight -= itemAttribute.itemWeight;

	/*Spawn Item and call OnItemRemove it*/
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	ActorSpawnParams.Owner = GetOwner();
	ActorSpawnParams.Instigator = m_OwnerCharacter.Get();
	AInventoryItem* spawnedItem = GetWorld()->SpawnActor<AInventoryItem>(itemAttribute.itemBlueprint, ActorSpawnParams);

	m_Items.RemoveAt(itemToRemove);

	if (spawnedItem) {
		spawnedItem->OnItemRemoved(item, m_OwnerCharacter.Get(), this);
		OnItemRemove.Broadcast();
	}
	UE_LOG(LogTemp, Warning, TEXT("InventoryComponent: Remove Item Success"));
}

void UInventoryComponent::useItem(const EInventoryItem item)
{
	if (!m_OwnerCharacter.IsValid())
	{
		return;
	}

	if (!getItemAttributeRow(item).itemBlueprint) {
		UE_LOG(LogTemp, Error, TEXT("InventoryComponent: Use item %s but invalid class reference."), *UEnum::GetValueAsString(item));
		return;
	}

	/*Spawn Item and use it*/
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	ActorSpawnParams.Owner = GetOwner();
	ActorSpawnParams.Instigator = m_OwnerCharacter.Get();
	AInventoryItem* spawnedItem = GetWorld()->SpawnActor<AInventoryItem>(getItemAttributeRow(item).itemBlueprint, ActorSpawnParams);

	if (spawnedItem) {
		spawnedItem->OnItemUsed(item,m_OwnerCharacter.Get(), this);
		OnItemUsed.Broadcast();
	}
}

void UInventoryComponent::SetMaxInventoryWeight(float newWeight)
{
	currentMaxInventoryWeight = newWeight;
}

const TArray<FInventoryStoredItem>& UInventoryComponent::getAllItem() const
{
	return m_Items;
}

void UInventoryComponent::dataTableAssetInitialization()
{
	//Get datatable
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTablePath(TEXT("/Script/Engine.DataTable'/Game/Developer/DataAsset/DT_InventoryItem.DT_InventoryItem'"));
	m_DataTableAsset = DataTablePath.Object;
}

const FInventoryItemAttributeRow& UInventoryComponent::getItemAttributeRow(const TEnumAsByte<EInventoryItem> item) const
{
	static FInventoryItemAttributeRow defaultRow;
	if (!m_DataTableAsset.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryComponent: Invalid Data Asset Reference"));
		return defaultRow;
	}
	FString itemEnumString = UEnum::GetValueAsString(item);
	/*Convert it to name so we can remove the prefix of enum*/
	FName itemEnumName = UKismetStringLibrary::Conv_StringToName(itemEnumString);

	if (FInventoryItemAttributeRow* row = m_DataTableAsset->FindRow<FInventoryItemAttributeRow>(itemEnumName, "")) {
		return *row;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("InventoryComponent: Invalid Item Enum %s To Find from DataAsset"), *itemEnumString);
		return defaultRow;
	}
	
}
