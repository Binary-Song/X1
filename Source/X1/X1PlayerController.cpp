// Fill out your copyright notice in the Description page of Project Settings.

#include "X1PlayerController.h"
#include "CoreGlobals.h"
#include "GameFramework/Actor.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/Pawn.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "Engine/LocalPlayer.h"
#include "Logging/LogVerbosity.h"
#include "Logging/StructuredLog.h"
#include "Math/MathFwd.h"
#include "Math/Vector2D.h"
#include "Templates/Casts.h"
#include "EnhancedInputComponent.h"
#include "X1Assert.h"
#include <cassert>
#include <set>
#include "X1Grabber.h"
#include "X1Interactable.h"
#include "X1TraceUtils.h"
#include "X1CollectionUtils.h"

void AX1PlayerController::BeginPlay()
{
    SetupInput();
    SetupCamera();
}

void AX1PlayerController::SetupInput()
{
    auto EnhancedInputSubsystem =
        GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

    X1_ASSERT_RET_VOID(EnhancedInputSubsystem);

    auto EnhancedInputComponent =
        CastChecked<UEnhancedInputComponent>(InputComponent);
    X1_ASSERT_RET_VOID(EnhancedInputComponent);

    EnhancedInputSubsystem->AddMappingContext(InputMappingContext, 0);
    X1_ASSERT_RET_VOID(InputMappingContext);

    auto Mappings = InputMappingContext->GetMappings();

    TSet<FName> ActionNames = {
        TEXT("IA_X1Look"),
        TEXT("IA_X1MoveForward"),
        TEXT("IA_X1MoveRight"),
        TEXT("IA_X1Interact"),
    };
    for (auto &&Map : Mappings)
    {
        const FName ActionName = Map.Action.GetFName();
        ActionNames.Remove(ActionName);
        if (ActionName == FName(TEXT("IA_X1Look")))
        {
            EnhancedInputComponent->BindAction(
                Map.Action, ETriggerEvent::Triggered, this,
                &AX1PlayerController::OnInputAction_Look);
        }
        else if (ActionName == FName(TEXT("IA_X1MoveForward")))
        {
            EnhancedInputComponent->BindAction(
                Map.Action, ETriggerEvent::Triggered, this,
                &AX1PlayerController::OnInputAction_MoveForward);
        }
        else if (ActionName == FName(TEXT("IA_X1MoveRight")))
        {
            EnhancedInputComponent->BindAction(
                Map.Action, ETriggerEvent::Triggered, this,
                &AX1PlayerController::OnInputAction_MoveRight);
        }
        else if (ActionName == FName(TEXT("IA_X1Interact")))
        {
            EnhancedInputComponent->BindAction(
                Map.Action, ETriggerEvent::Started, this,
                &AX1PlayerController::OnInputAction_Interact);
        }
        else
        {
            X1_ASSERTX_CONTINUE(0, "unknown mapping name: {0}",
                                ActionName.ToString());
        }
    }

    X1_ASSERTX_IGNORE(ActionNames.Num() == 0, "{0} unregistered actions",
                      ActionNames.Num());
}

void AX1PlayerController::SetupCamera()
{
    X1_ASSERT_RET_VOID(PlayerCameraManager);
    PlayerCameraManager->ViewPitchMin = -70.0f;
    PlayerCameraManager->ViewPitchMax = 0.0f;
}

void AX1PlayerController::OnInputAction_MoveForward(
    const struct FInputActionInstance &Instance)
{
    this->HandleInput_MoveForward(Instance.GetValue().Get<float>());
}

void AX1PlayerController::OnInputAction_MoveRight(
    const struct FInputActionInstance &Instance)
{
    this->HandleInput_MoveRight(Instance.GetValue().Get<float>());
}

void AX1PlayerController::OnInputAction_Look(
    const struct FInputActionInstance &Instance)
{
    const auto v = Instance.GetValue().Get<FVector2D>();
    this->HandleInput_Yaw(v.X);
    this->HandleInput_Pitch(-v.Y);
}

void AX1PlayerController::OnInputAction_Interact(
    const struct FInputActionInstance &Instance)
{
    this->HandleInput_Interact(Instance.GetValue().Get<float>());
}

// 注意区别： ControlRotation 是视角朝向， Actor 的 forward 是角色模型的朝向
// 移动的方向应该基于视角的朝向。

void AX1PlayerController::HandleInput_MoveForward(float Value)
{
    if (!Value)
        return;
    X1_ASSERT_RET_VOID(GetPawn());
    FRotator ControlRot = GetControlRotation();
    FRotator ControlRot_YawOnly(0, ControlRot.Yaw, 0);
    FVector ForwardVector =
        FRotationMatrix(ControlRot_YawOnly).GetUnitAxis(EAxis::X);
    GetPawn()->AddMovementInput(ForwardVector, Value);
}

void AX1PlayerController::HandleInput_MoveRight(float Value)
{
    if (!Value)
        return;
    X1_ASSERT_RET_VOID(GetPawn());
    FRotator ControlRot = GetControlRotation();
    FRotator ControlRot_YawOnly(0, ControlRot.Yaw, 0);
    FVector RightVector =
        FRotationMatrix(ControlRot_YawOnly).GetUnitAxis(EAxis::Y);
    GetPawn()->AddMovementInput(RightVector, Value);
}

void AX1PlayerController::HandleInput_Yaw(float Value)
{
    if (!Value)
        return;
    X1_ASSERT_RET_VOID(GetPawn());
    this->AddYawInput(Value);
}

void AX1PlayerController::HandleInput_Pitch(float Value)
{
    if (!Value)
        return;
    X1_ASSERT_RET_VOID(GetPawn());
    this->AddPitchInput(Value);
}

void AX1PlayerController::HandleInput_Interact(float Value)
{
    AActor *HitActor{};
    const FX1TraceSearch SearchParams = []() {
        FX1TraceSearch Params{};
        Params.TraceDistance = 300;
        Params.bDrawDebugLines = true;
        Params.ActorFilterFunction = [](AActor *Actor) {
            if (Actor->Implements<UX1Interactable>())
                return IX1Interactable::Execute_InteractType(Actor) !=
                       EInteractType::None;
            return false;
        };
        return Params;
    }();
    FX1TraceSearchResult SearchResult;
    FX1TraceUtils::TraceSearch(SearchResult, this, SearchParams);
    auto FindResult = UX1CollectionUtil::FindMinElement(
        SearchResult.Items, [](const FX1CameraTraceSearchItem &a,
                               const FX1CameraTraceSearchItem &b) {
            return FVector2D{a.RotOffset.Yaw, a.RotOffset.Pitch}
                       .SquaredLength() <
                   FVector2D{b.RotOffset.Yaw, b.RotOffset.Pitch}
                       .SquaredLength();
        });

    if (!FindResult.bFound)
        return;

    const EInteractType IT =
        IX1Interactable::Execute_InteractType(FindResult.Item.Actor);
    if (IT == EInteractType::Pickup)
    {
        X1_ASSERT_RET_VOID(GetPawn());
        X1_ASSERT_RET_VOID(GetPawn()->Implements<UX1Grabber>());

        const FX1GrabParam GrabParam = [&]() {
            FX1GrabParam P;
            P.GrabbedBoneName = FindResult.Item.HitResult.BoneName;
            P.GrabbedComponent = FindResult.Item.HitResult.Component.Get();
            return P;
        }();

        IX1Grabber::Execute_Ungrab(GetPawn());
        IX1Grabber::Execute_Grab(GetPawn(), GrabParam);
    }
}
