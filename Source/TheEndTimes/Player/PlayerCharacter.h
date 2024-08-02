// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterBase.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInventoryComponent;
struct FInputActionValue;

UCLASS()
class THEENDTIMES_API APlayerCharacter : public ACharacterBase
{
	GENERATED_BODY()
	
public:
	APlayerCharacter();

	/*Restraint player to spam the attack animation*/
	UFUNCTION(BlueprintCallable, Category = Player)
	void setCanMeleeAttack(bool canAttack);

	//Disable player control
	UFUNCTION(BlueprintCallable, Category = Player)
	void setDisablePlayerControl(bool disable);

	UFUNCTION(BlueprintCallable, Category = Player)
	bool isPlayerControlDisable() const;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* getCameraBoom() const { return cameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* getFollowCamera() const { return followCamera; }

	//Input
	void move(const FInputActionValue& value);
	void setCameraZoom(const FInputActionValue& value);
	//Same functionality with the setCharacterStandingState in CharacterBase, but add a boolean check for disable control
	void setPlayerCharacterStandingState(const ECharacterStandState newCharacterStandState);
	//Movement and standing state setter
	void switchCrouchState();
	void switchProneState();
	//Melee Attack
	void meleeAttack();

	/*Used this function to toggle using controller rotation or orient rotation to movement*/
	void useControllerRotation(bool IsTrue);
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PlayerBaseAttribute)
	float maxZoomIn = 50;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PlayerBaseAttribute)
	float maxZoomOut = 120;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PlayerBaseAttribute)
	float lookAtInterpSpeed = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PlayerBaseAttribute)
	int meleeAttackDamage = 30;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PlayerBaseAttribute)
	TObjectPtr<UAnimMontage> attackMontage;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> cameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> followCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInventoryComponent> inventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> batMesh;

private:
	// Disable control of character
	bool m_DisablePlayerControl = false;
	// Melee Attack
	bool m_CanAttack = true;

};
