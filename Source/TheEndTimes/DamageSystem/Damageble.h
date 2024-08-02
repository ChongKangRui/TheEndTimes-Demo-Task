// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Damageble.generated.h"


UENUM(BlueprintType)
enum EDebuff {
	NoDebuff,
	Zombie_Scratch
};

class UDebuffTrigger;

USTRUCT(Blueprintable)
struct FDamageInfo : public FTableRowBase{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int damage;

	/*Useful when we can get the debuff information and known what debuff this character have*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TEnumAsByte<EDebuff> debuffType;

	/*Use DebuffTrigger to trigger the debuff accordingly*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UDebuffTrigger> debuffTrigger;

	FDamageInfo() : damage(10.0f), debuffType(EDebuff::NoDebuff), debuffTrigger() {
	}

	FDamageInfo(int Damage) : damage(Damage), debuffType(EDebuff::NoDebuff), debuffTrigger(){
	}
};
// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UDamageable : public UInterface
{
	GENERATED_BODY()
};

class THEENDTIMES_API IDamageable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Damage")
	void applyDamage(const FDamageInfo DamageInfo);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Damage")
	float getHealth() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Damage")
	float getMaxHealth() const;
};
