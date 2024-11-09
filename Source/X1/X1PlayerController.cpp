// Fill out your copyright notice in the Description page of Project Settings.

#include "X1PlayerController.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/Pawn.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "Engine/LocalPlayer.h"
#include "Templates/Casts.h"
#include "EnhancedInputComponent.h"
#include <cassert>
void AX1PlayerController::BeginPlay()
{
    // Get the local player subsystem
    auto EnhancedInputSubsystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
    if (!EnhancedInputSubsystem)
        return;
    auto EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
    if (!EnhancedInputComponent)
        return;

    EnhancedInputSubsystem->AddMappingContext(InputMappingContext, 0);

    if (!InputMappingContext)
        return;

    auto Mappings = InputMappingContext->GetMappings();
    verify(Mappings[0].GetMappingName() == "MoveForward");
    verify(Mappings[1].GetMappingName() == "MoveForward");
    verify(Mappings[2].GetMappingName() == "MoveForward");

    EnhancedInputComponent->BindAction(Mappings[0].Action, ETriggerEvent::Triggered, this,
                                       &AX1PlayerController::OnInputAction_Look);
    EnhancedInputComponent->BindAction(Mappings[1].Action, ETriggerEvent::Triggered, this,
                                       &AX1PlayerController::OnInputAction_MoveForward);
    EnhancedInputComponent->BindAction(Mappings[2].Action, ETriggerEvent::Triggered, this,
                                       &AX1PlayerController::OnInputAction_MoveRight);
}

void AX1PlayerController::OnInputAction_MoveForward(const struct FInputActionInstance &Instance)
{
}

void AX1PlayerController::OnInputAction_MoveRight(const struct FInputActionInstance &Instance)
{
}

void AX1PlayerController::OnInputAction_Look(const struct FInputActionInstance &Instance)
{
}

// 注意区别： ControlRotation 是视角朝向， Actor 的 forward 是角色模型的朝向
// 移动的方向应该基于视角的朝向。
void AX1PlayerController::MoveForward(float Value)
{

    if (!Value)
        return;
    FRotator ControlRot = GetControlRotation();
    FRotator ControlRot_YawOnly(0, ControlRot.Yaw, 0);
    FVector ForwardVector = FRotationMatrix(ControlRot_YawOnly).GetUnitAxis(EAxis::X);
    GetPawn()->AddMovementInput(ForwardVector, Value);
}

void AX1PlayerController::MoveRight(float Value)
{
    if (!Value)
        return;
    FRotator ControlRot = GetControlRotation();
    FRotator ControlRot_YawOnly(0, ControlRot.Yaw, 0);
    FVector RightVector = FRotationMatrix(ControlRot_YawOnly).GetUnitAxis(EAxis::Y);
    GetPawn()->AddMovementInput(RightVector, Value);
}

void AX1PlayerController::AddYaw(float Value)
{
    if (!Value)
        return;
    GetPawn()->AddControllerYawInput(Value);
}

void AX1PlayerController::AddPitch(float Value)
{
    if (!Value)
        return;
    GetPawn()->AddControllerPitchInput(Value);
}
