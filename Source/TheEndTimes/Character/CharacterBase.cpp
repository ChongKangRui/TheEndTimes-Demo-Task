// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ACharacterBase::ACharacterBase()
{
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	setCharacterMovementState(ECharacterMovementState::Walk);
	setCharacterStandingState(ECharacterStandState::Stand);
}

void ACharacterBase::addCharacterHealth(float newHealth)
{
	float MaxHealth = IDamageable::Execute_getHealth(this);
	if (health + newHealth >= MaxHealth)
		health = MaxHealth;

	health += newHealth;
}

//Character Movement, Standing, Attribute setter and getter
void ACharacterBase::setCharacterAttribute(const FCharacterBaseAttribute& attribute)
{
	characterAttribute = attribute;
	health = characterAttribute.maxHealth;
}

void ACharacterBase::setCharacterMovementState(const ECharacterMovementState newCharacterMovementState)
{
	//Set movement speed based on the current character standing state.
	switch (newCharacterMovementState) {

	case ECharacterMovementState::Walk:
		switch (getCharacterStandState()) {
		case ECharacterStandState::Stand:
			setCharacterMovementSpeed(getCharacterAttribute().walkingSpeed);
			break;
		case ECharacterStandState::Crouch:
			setCharacterMovementSpeed(getCharacterAttribute().crouchWalkSpeed);
			break;
		case ECharacterStandState::Prone:
			setCharacterMovementSpeed(getCharacterAttribute().proneWalkSpeed);
			break;
		}
		break;
	case ECharacterMovementState::Jog:
		switch (getCharacterStandState()) {
		case ECharacterStandState::Stand:
			setCharacterMovementSpeed(getCharacterAttribute().joggingSpeed);
			break;
		case ECharacterStandState::Crouch:
			setCharacterMovementSpeed(getCharacterAttribute().crouchJogSpeed);
			break;
		case ECharacterStandState::Prone:
			setCharacterMovementSpeed(getCharacterAttribute().proneJogSpeed);
			break;
		}
		break;
	case ECharacterMovementState::Sprint:
		if (getCharacterStandState() != ECharacterStandState::Stand)
			setCharacterStandingState(ECharacterStandState::Stand);
		setCharacterMovementSpeed(getCharacterAttribute().sprintSpeed);
		break;
	}
	characterMovementState = newCharacterMovementState;
}

void ACharacterBase::setCharacterStandingState(const ECharacterStandState newCharacterStandState)
{
	characterStandState = newCharacterStandState;

	/*Set the collision properly so when prone, collision won't messup*/
	if (characterStandState == ECharacterStandState::Prone)
		GetCapsuleComponent()->SetCapsuleRadius(90.0f);
	else
		GetCapsuleComponent()->SetCapsuleRadius(35.0f);

	//If character dead, start ragdoll
	if (characterStandState == ECharacterStandState::Death) {
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetMesh()->SetAllBodiesSimulatePhysics(true);
		return;
	}

	//Reset character movement speed
	setCharacterMovementState(characterMovementState);
}

void ACharacterBase::setCharacterMovementSpeed(const float newSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = newSpeed;
}

void ACharacterBase::applyDamageToTargetWithInfo(AActor* target, FDamageInfo damageInfo)
{
	if (!target)
		return;

	if (target == this)
		return;

	if (damagedActors.Contains(target))
		return;

	IDamageable::Execute_applyDamage(target, damageInfo);
	damagedActors.AddUnique(target);
}

void ACharacterBase::applyDamageToTarget(AActor* target, int damage)
{
	if (!target)
		return;

	if (target == this)
		return;
	
	if (damagedActors.Contains(target))
		return;

	IDamageable::Execute_applyDamage(target, FDamageInfo(damage));
	damagedActors.AddUnique(target);
}

void ACharacterBase::setMeleeDetectTrace(bool detectTrace)
{
	canDetectTrace = detectTrace;

	if(!canDetectTrace)
		damagedActors.Empty();
}

FCharacterBaseAttribute ACharacterBase::getCharacterAttribute() const
{
	return characterAttribute;
}

ECharacterMovementState ACharacterBase::getCharacterMovementState() const
{
	return characterMovementState;
}

ECharacterStandState ACharacterBase::getCharacterStandState() const
{
	return characterStandState;
}

//Damageble Interface
void ACharacterBase::applyDamage_Implementation(const FDamageInfo DamageInfo)
{
	health -= DamageInfo.damage;

	UE_LOG(LogTemp, Error, TEXT("%s being attacked"), *GetName());
	if (health <= 0.0f) {
		OnCharacterDead.Broadcast();
		UE_LOG(LogTemp, Error, TEXT("%s Dead"), *GetName());
		setCharacterStandingState(ECharacterStandState::Death);
	}
}

float ACharacterBase::getHealth_Implementation() const
{
	return health;
}

float ACharacterBase::getMaxHealth_Implementation() const
{
	return getCharacterAttribute().maxHealth;
}

