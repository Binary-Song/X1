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
#include "X1Assert.h"
#include <cassert>
void AX1PlayerController::BeginPlay()
{
    // Get the local player subsystem
    auto EnhancedInputSubsystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
    X1_ASSERT_RET_VOID(EnhancedInputSubsystem);

    auto EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
    X1_ASSERT_RET_VOID(EnhancedInputComponent);

    EnhancedInputSubsystem->AddMappingContext(InputMappingContext, 0);
    X1_ASSERT_RET_VOID(InputMappingContext);

    auto Mappings = InputMappingContext->GetMappings();

    for (auto &&Map : Mappings)
    {
        FName ActionName = Map.Action.GetFName();
        if (ActionName == FName(TEXT("IA_X1Look")))
        {
            EnhancedInputComponent->BindAction(Map.Action, ETriggerEvent::Triggered, this,
                                               &AX1PlayerController::OnInputAction_Look);
        }
        else if (ActionName == FName(TEXT("IA_X1MoveForward")))
        {
            EnhancedInputComponent->BindAction(Map.Action, ETriggerEvent::Triggered, this,
                                               &AX1PlayerController::OnInputAction_MoveForward);
        }
        else if (ActionName == FName(TEXT("IA_X1MoveRight")))
        {
            EnhancedInputComponent->BindAction(Map.Action, ETriggerEvent::Triggered, this,
                                               &AX1PlayerController::OnInputAction_MoveRight);
        }
        else
        {
            X1_ASSERTF_CONTINUE(0, "unknown map name: %s", *ActionName.ToString());
        }
    }
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
