// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AIController_Zombie.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "EnemyBase.h"

AAIController_Zombie::AAIController_Zombie()
{
	// Create the Perception Component
	zombiePerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	sightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	hearSense = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	// Setup perception configuration
	zombiePerceptionComponent->ConfigureSense(*sightConfig);
	zombiePerceptionComponent->ConfigureSense(*hearSense);
	zombiePerceptionComponent->SetDominantSense(sightConfig->GetSenseImplementation());

	SetActorTickInterval(0.5f);
	SetActorTickEnabled(false);
	
}

void AAIController_Zombie::OnPossess(APawn* PawnToProcess)
{
	Super::OnPossess(PawnToProcess);

	if (AEnemyBase* enemy = Cast<AEnemyBase>(PawnToProcess)) {
		m_PossessedEnemy = enemy;
	}
	zombiePerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &AAIController_Zombie::onPerceptionUpdated);
}

void AAIController_Zombie::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (m_Target.IsValid()) {
		//Check if player too far away or already dead
		if ((FVector::Distance(m_PossessedEnemy->GetActorLocation(), m_Target->GetActorLocation()) > 2000.0f) || m_Target->getCharacterStandState() == ECharacterStandState::Death) {

			if (auto BB = GetBlackboardComponent()) {

				if (m_PossessedEnemy.IsValid()) {
					m_PossessedEnemy->setCharacterMovementState(ECharacterMovementState::Walk);
				}

				BB->ClearValue("Target");
				ClearFocus(EAIFocusPriority::Gameplay);
				m_Target.Reset();
				

				SetActorTickEnabled(false);
			}
		}
	}
}

void AAIController_Zombie::onPerceptionUpdated(const TArray<AActor*>& updatedActors)
{
	if (!GetBlackboardComponent()) {
		UE_LOG(LogTemp, Error, TEXT("%s: Invalid Blackboard Component"), *GetName())
		return;
	}

	if (m_Target.IsValid())
		return;

	for (AActor* target : updatedActors) {
		if (auto character = Cast<ACharacterBase>(target)) {
			if (character->getCharacterStandState() == ECharacterStandState::Death)
				continue;
			else {
				m_Target = character;

				if (m_PossessedEnemy.IsValid()) {
					m_PossessedEnemy->setCharacterMovementState(ECharacterMovementState::Jog);
				}
				GetBlackboardComponent()->SetValueAsObject("Target", target);
				SetFocus(m_Target.Get(), EAIFocusPriority::Gameplay);
				//Enable tick to check condition, otherwise should disable to save performance
				SetActorTickEnabled(true);
				

				break;
			}
		}
	}
}

ETeamAttitude::Type AAIController_Zombie::GetTeamAttitudeTowards(const AActor& actor) const
{
	if (actor.ActorHasTag("Player"))
		return ETeamAttitude::Hostile;
	else
		return ETeamAttitude::Neutral;
}
