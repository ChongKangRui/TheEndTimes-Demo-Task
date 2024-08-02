// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Perception/AISense_Hearing.h"
#include "InventoryComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

APlayerCharacter::APlayerCharacter()
{
	// Create a camera boom (pulls in towards the player if there is a collision)
	cameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	cameraBoom->SetupAttachment(RootComponent);
	cameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	cameraBoom->bUsePawnControlRotation = false; // Rotate the arm based on the controller

	// Create a follow camera
	followCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	followCamera->SetupAttachment(cameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	followCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	batMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BatMesh"));
	batMesh->SetupAttachment(GetMesh());

	inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
}

void APlayerCharacter::move(const FInputActionValue& value)
{
	if (isPlayerControlDisable())
		return;

	FVector2D MovementVector = value.Get<FVector2D>();

	FVector LeftRightMovementDirection = UKismetMathLibrary::GetRightVector(FRotator(0.0f, cameraBoom->GetRelativeRotation().Yaw, 0.0f));
	FVector ForwardBackMovementDirection = UKismetMathLibrary::GetForwardVector(FRotator(0.0f, cameraBoom->GetRelativeRotation().Yaw, 0.0f));

	//Report Noise based on the current speed
	UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), 1.0f, this, GetCharacterMovement()->MaxWalkSpeed * 2.0f);

	AddMovementInput(LeftRightMovementDirection, MovementVector.X);
	AddMovementInput(ForwardBackMovementDirection, MovementVector.Y);
}

void APlayerCharacter::setDisablePlayerControl(bool disable)
{
	m_DisablePlayerControl = disable;
}

void APlayerCharacter::setCameraZoom(const FInputActionValue& value)
{
	if (isPlayerControlDisable())
		return;

	float zoomValue = value.Get<float>();
	getFollowCamera()->FieldOfView = FMath::Clamp(getFollowCamera()->FieldOfView + zoomValue, maxZoomIn, maxZoomOut);
}

void APlayerCharacter::setPlayerCharacterStandingState(const ECharacterStandState newCharacterStandState)
{
	if (isPlayerControlDisable())
		return;

	setCharacterStandingState(newCharacterStandState);
}

void APlayerCharacter::switchCrouchState()
{
	if (getCharacterStandState() != ECharacterStandState::Crouch)
		setPlayerCharacterStandingState(ECharacterStandState::Crouch);
	else
		setPlayerCharacterStandingState(ECharacterStandState::Stand);
}

void APlayerCharacter::switchProneState()
{
	if (getCharacterStandState() != ECharacterStandState::Prone)
		setPlayerCharacterStandingState(ECharacterStandState::Prone);
	else
		setPlayerCharacterStandingState(ECharacterStandState::Stand);
}

void APlayerCharacter::meleeAttack()
{
	if (!m_CanAttack)
		return;

	if (!attackMontage)
		return;

	if (m_DisablePlayerControl)
		return;

	setCanMeleeAttack(false);
	PlayAnimMontage(attackMontage);
}

void APlayerCharacter::setCanMeleeAttack(bool canAttack)
{
	m_CanAttack = canAttack;
}

void APlayerCharacter::useControllerRotation(bool IsTrue)
{
	GetCharacterMovement()->bOrientRotationToMovement = !IsTrue;
	GetCharacterMovement()->bUseControllerDesiredRotation = IsTrue;
}

bool APlayerCharacter::isPlayerControlDisable() const
{
	return m_DisablePlayerControl;
}

