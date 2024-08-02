// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DebuffTrigger.generated.h"

/**
 * 
 */
class ACharacterBase;

UCLASS()
class THEENDTIMES_API UDebuffTrigger : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void onDebuffBegin(ACharacterBase* debuffOwner);

	UFUNCTION(BlueprintNativeEvent)
	void onDebuffEnd();
	
};
