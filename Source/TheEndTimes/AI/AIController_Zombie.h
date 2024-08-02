// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIController_Zombie.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
class AEnemyBase;
class ACharacterBase;

UCLASS()
class THEENDTIMES_API AAIController_Zombie : public AAIController
{
	GENERATED_BODY()
	
public:
	AAIController_Zombie();

protected:
	UFUNCTION()
	void onPerceptionUpdated(const TArray<AActor*>& updatedActors);

	virtual void OnPossess(APawn* PawnToProcess) override;
	virtual void Tick(float DeltaTime) override;

	ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& actor) const override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UAIPerceptionComponent> zombiePerceptionComponent;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UAISenseConfig_Sight> sightConfig;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UAISenseConfig_Hearing> hearSense;

private:
	TWeakObjectPtr<AEnemyBase> m_PossessedEnemy;
	TWeakObjectPtr<ACharacterBase> m_Target;
};
