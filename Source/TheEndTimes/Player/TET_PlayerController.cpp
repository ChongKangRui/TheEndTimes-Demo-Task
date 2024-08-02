// Fill out your copyright notice in the Description page of Project Settings.


#include "TET_PlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Data/PlayerInputMapping.h"
#include "GameFramework/Character.h"
#include "Player/PlayerCharacter.h"
#include "InputMappingContext.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputAction.h"

void ATET_PlayerController::OnPossess(APawn* pawn)
{
    Super::OnPossess(pawn);

    if (pawn) {
        m_Character = Cast<APlayerCharacter>(pawn);
        loadPlayerMappingDataset();
        SetupInputComponent();
    }
}

void ATET_PlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (!playerMappingDataset.IsValid()) {
        UE_LOG(LogTemp, Error, TEXT("PC: PlayerMappingData Reference Invalid"));
        return;
    }

    if (!m_Character){
       UE_LOG(LogTemp, Error, TEXT("PC: Invalid Character Reference"));
       return;
    }
    //Binding Input Action
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
    {
        if (const UInputAction* Move = playerMappingDataset->moveAction.Get()) {
            EnhancedInputComponent->BindAction(Move, ETriggerEvent::Triggered, m_Character.Get(), &APlayerCharacter::move);
        }

        if (const UInputAction* Look = playerMappingDataset->lookAction.Get()) {
            EnhancedInputComponent->BindAction(Look, ETriggerEvent::Started, this, &ATET_PlayerController::startLook);
            EnhancedInputComponent->BindAction(Look, ETriggerEvent::Canceled, this, &ATET_PlayerController::stopLook);
            EnhancedInputComponent->BindAction(Look, ETriggerEvent::Completed, this, &ATET_PlayerController::stopLook);
        }

        if (const UInputAction* Sprint = playerMappingDataset->sprintAction.Get()) {
            EnhancedInputComponent->BindAction(Sprint, ETriggerEvent::Triggered, m_Character.Get(), &APlayerCharacter::setCharacterMovementState, ECharacterMovementState::Sprint);
            EnhancedInputComponent->BindAction(Sprint, ETriggerEvent::Canceled, m_Character.Get(), &APlayerCharacter::setCharacterMovementState, ECharacterMovementState::Walk);
            EnhancedInputComponent->BindAction(Sprint, ETriggerEvent::Completed, m_Character.Get(), &APlayerCharacter::setCharacterMovementState, ECharacterMovementState::Walk);

        }

        if (const UInputAction* Joy = playerMappingDataset->runAction.Get()) {
            EnhancedInputComponent->BindAction(Joy, ETriggerEvent::Started, m_Character.Get(), &APlayerCharacter::setCharacterMovementState, ECharacterMovementState::Jog);
            EnhancedInputComponent->BindAction(Joy, ETriggerEvent::Canceled, m_Character.Get(), &APlayerCharacter::setCharacterMovementState, ECharacterMovementState::Walk);
            EnhancedInputComponent->BindAction(Joy, ETriggerEvent::Completed, m_Character.Get(), &APlayerCharacter::setCharacterMovementState, ECharacterMovementState::Walk);
        }

        if (const UInputAction* Crouch = playerMappingDataset->crouchAction.Get()) {
            EnhancedInputComponent->BindAction(Crouch, ETriggerEvent::Started, m_Character.Get(), &APlayerCharacter::switchCrouchState);
        }

        if (const UInputAction* Prone = playerMappingDataset->proneAction.Get()) {
            EnhancedInputComponent->BindAction(Prone, ETriggerEvent::Started, m_Character.Get(), &APlayerCharacter::switchProneState);
        }

        if (const UInputAction* Attack = playerMappingDataset->attackAction.Get()) {
            EnhancedInputComponent->BindAction(Attack, ETriggerEvent::Started, m_Character.Get(), &APlayerCharacter::meleeAttack);
        }

        if (const UInputAction* Zoom = playerMappingDataset->cameraZoomAction.Get()) {
             EnhancedInputComponent->BindAction(Zoom, ETriggerEvent::Triggered, m_Character.Get(), &APlayerCharacter::setCameraZoom);
        }


        UE_LOG(LogTemp, Error, TEXT("PC: Bind input action success"));
    }

    //After finish binding, release the memory of playerMappingDataset
    playerMappingDataset.Reset();
}

void ATET_PlayerController::startLook()
{
    if (!m_Character)
        return;

    if (m_Character->isPlayerControlDisable())
        return;

    FTimerDelegate MoveDelegate;
    MoveDelegate.BindWeakLambda(this, [&]()
        {
            lookTick();
        });

    GetWorld()->GetTimerManager().SetTimer(m_LookAtTimer, MoveDelegate, GetWorld()->GetDeltaSeconds(), true);
}

void ATET_PlayerController::stopLook()
{
    GetWorld()->GetTimerManager().ClearTimer(m_LookAtTimer);
    m_LookAtTimer.Invalidate();
    m_Character->useControllerRotation(false);
}

void ATET_PlayerController::lookTick()
{
    if (m_Character->isPlayerControlDisable()) {
        return;
    }

    TArray<TEnumAsByte<EObjectTypeQuery>> hitObjectChannel = { EObjectTypeQuery::ObjectTypeQuery1, EObjectTypeQuery ::ObjectTypeQuery2};
    FHitResult hitResult;
    GetHitResultUnderCursorForObjects(hitObjectChannel, false, hitResult);

    if (!hitResult.bBlockingHit) {
        m_Character->useControllerRotation(false);
        return;
    }
    m_Character->useControllerRotation(true);

    FVector direction = hitResult.Location - m_Character->GetActorLocation();
    FVector directionNormal = direction.GetSafeNormal();

    float yawRadian = FMath::Atan2(directionNormal.Y, directionNormal.X);
    float yawAngle = FMath::RadiansToDegrees(yawRadian);

    FRotator targetRotation = FRotator(0, yawAngle, 0.0f);
    FRotator FinalRotation = FMath::RInterpTo(GetControlRotation(),
        targetRotation, GetWorld()->GetDeltaSeconds(), m_Character->lookAtInterpSpeed);

    SetControlRotation(FinalRotation);
}

void ATET_PlayerController::loadPlayerMappingDataset()
{
    //Load the player mapping context and input action from the data asset
    if (playerMappingDataset.IsPending()) {
        playerMappingDataset.LoadSynchronous();
    }

    if (playerMappingDataset.IsValid()) {
        if (const UInputMappingContext* MappingContext = playerMappingDataset->defaultMappingContext.Get()) {
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(this->GetLocalPlayer())) {
                Subsystem->AddMappingContext(MappingContext, 0);
            }
        }
    }

}


