// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DamageSystem/Damageble.h"
#include "CharacterBase.generated.h"

/*Use this to define which standing behavior character are*/
UENUM(Blueprintable)
enum ECharacterStandState {
	Stand,
	Crouch,
	Prone,
	Death
};

/*Use this to define the maximum moving speed of character*/
UENUM(Blueprintable)
enum ECharacterMovementState {
	Walk,
	Jog,
	Sprint
};

/*Character base attribute, used for data collection*/
USTRUCT(Blueprintable)
struct FCharacterBaseAttribute : public FTableRowBase {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Attribute")
	float maxHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Attribute")
	float walkingSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Attribute")
	float joggingSpeed = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Attribute")
	float sprintSpeed = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Attribute")
	float crouchWalkSpeed = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Attribute")
	float crouchJogSpeed = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Attribute")
	float proneWalkSpeed = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Attribute")
	float proneJogSpeed = 80.0f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterDead);

struct FInputActionValue;
UCLASS()
class THEENDTIMES_API ACharacterBase : public ACharacter, public IDamageable
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterBase();

	UFUNCTION(BlueprintPure)
	FCharacterBaseAttribute getCharacterAttribute() const;

	UFUNCTION(BlueprintPure)
	ECharacterMovementState getCharacterMovementState() const;

	UFUNCTION(BlueprintPure)
	ECharacterStandState getCharacterStandState() const;

	UFUNCTION(BlueprintCallable)
	void setCharacterMovementState(const ECharacterMovementState newCharacterMovementState);

	UFUNCTION(BlueprintCallable)
	void setCharacterStandingState(const ECharacterStandState newCharacterStandState);

	/*Used to set if character allow to start attack logic, this should be temporary solution for demo*/
	UFUNCTION(BlueprintCallable, Category = DamageSystem)
	void setMeleeDetectTrace(bool detectTrace);

	UFUNCTION(BlueprintCallable)
	void addCharacterHealth(float newHealth);

	UFUNCTION(BlueprintCallable, Category = DamageSystem)
	void applyDamageToTarget(AActor* target, int damage);

	UFUNCTION(BlueprintCallable, Category = DamageSystem)
	void applyDamageToTargetWithInfo(AActor* target, FDamageInfo damageInfo);

	void BeginPlay() override;

public:
	UPROPERTY(BlueprintAssignable)
	FOnCharacterDead OnCharacterDead;

protected:
	UFUNCTION(BlueprintCallable)
	void setCharacterAttribute(const FCharacterBaseAttribute& newCharacterAttribute);

	// Damageble Interface
	float getHealth_Implementation() const override;
	float getMaxHealth_Implementation() const override;
	void applyDamage_Implementation(const FDamageInfo DamageInfo) override;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Base Attribute")
	float health = 100.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Base Attribute")
	FCharacterBaseAttribute characterAttribute;
	
	UPROPERTY(BlueprintReadOnly, Category = "Base Attribute")
	TEnumAsByte<ECharacterMovementState> characterMovementState = ECharacterMovementState::Walk;

	UPROPERTY(BlueprintReadOnly, Category = "Base Attribute")
	TEnumAsByte<ECharacterStandState> characterStandState = ECharacterStandState::Stand;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DamageSystem, meta = (AllowPrivateAccess = "true"))
	bool canDetectTrace = false;

	/*Array that used to trace if target already being damaged by this actor*/
	TArray<TWeakObjectPtr<AActor>> damagedActors;
private:
	void setCharacterMovementSpeed(const float newSpeed);

};
