// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TET_PlayerController.generated.h"

class UPlayerInputMapping;
class APlayerCharacter;

UCLASS()
class THEENDTIMES_API ATET_PlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void OnPossess(APawn* pawn) override;
	virtual void SetupInputComponent() override;

	void startLook();
	void stopLook();
	void lookTick();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TSoftObjectPtr<UPlayerInputMapping> playerMappingDataset;

private:
	void loadPlayerMappingDataset();

private:
	TObjectPtr<APlayerCharacter> m_Character;
	FTimerHandle m_LookAtTimer;
};
