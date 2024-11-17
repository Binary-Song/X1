// Fill out your copyright notice in the Description page of Project Settings.

#include "X1PlayerController.h"
#include "CoreGlobals.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Actor.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/Pawn.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "Engine/LocalPlayer.h"
#include "InputTriggers.h"
#include "Logging/LogVerbosity.h"
#include "Logging/StructuredLog.h"
#include "Math/MathFwd.h"
#include "Math/Vector2D.h"
#include "Templates/Casts.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "UObject/NameTypes.h"
#include "X1Assert.h"
#include <cassert>
#include <set>
#include "X1Grabber.h"
#include "X1Interactable.h"
#include "X1TraceUtils.h"
#include "X1CollectionUtils.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "X1Zoom.h"
#include "X1Format.h"
void AX1PlayerController::BeginPlay()
{
    SetupInput();
    SetupCamera();
    this->SetShowMouseCursor(true);
}

static void CheckSets(TSet<FName> &ActionNamesInMapping,
                      TSet<FName> &ExpectedActionNames)
{
    TSet<FName> Diff = ExpectedActionNames.Difference(ActionNamesInMapping);
    X1_ASSERT_PASS(Diff.Num() == 0, "unregistered actions {0}", DbgFmt(Diff));
    Diff = ActionNamesInMapping.Difference(ExpectedActionNames);
    X1_ASSERT_PASS(Diff.Num() == 0, "unexpected actions {0}", DbgFmt(Diff));
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

    using InputCallback =
        void (AX1PlayerController::*)(const FInputActionInstance &Instance);

    struct ActionData
    {
        ETriggerEvent Trigger;
        InputCallback Callback;
    };

    // clang-format off
    TMap<FName, ActionData> ActionNames =
    {
        { TEXT("IA_X1Look"),            {ETriggerEvent::Triggered,   &AX1PlayerController::OnInputAction_Look         }},
        { TEXT("IA_X1MoveForward"),     {ETriggerEvent::Triggered,   &AX1PlayerController::OnInputAction_MoveForward  }},
        { TEXT("IA_X1MoveRight"),       {ETriggerEvent::Triggered,   &AX1PlayerController::OnInputAction_MoveRight    }},
        { TEXT("IA_X1Interact"),        {ETriggerEvent::Started,     &AX1PlayerController::OnInputAction_Interact     }},
        { TEXT("IA_X1Fly"),             {ETriggerEvent::Triggered,   &AX1PlayerController::OnInputAction_Fly          }},
        { TEXT("IA_X1SwitchFlyMode"),   {ETriggerEvent::Started,     &AX1PlayerController::OnInputAction_SwitchFlyMode}},
        { TEXT("IA_X1Zoom"),            {ETriggerEvent::Started,     &AX1PlayerController::OnInputAction_Zoom         }},
    };
    // clang-format on

    TSet<FName> ActionNamesInMapping;
    for (auto &&Map : Mappings)
        ActionNamesInMapping.Add(Map.Action.GetFName());

    TSet<FName> ExpectedActionNames;
    ActionNames.GetKeys(ExpectedActionNames);

    CheckSets(ActionNamesInMapping, ExpectedActionNames);

    for (auto &&Map : Mappings)
    {
        const FName ActionName = Map.Action.GetFName();
        if (ActionData *Data = ActionNames.Find(ActionName))
        {
            EnhancedInputComponent->BindAction(Map.Action, Data->Trigger, this,
                                               Data->Callback);
        }
    }
};
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

void AX1PlayerController::OnInputAction_Fly(
    const struct FInputActionInstance &Instance)
{
    this->HandleInput_Fly(Instance.GetValue().Get<float>());
}

void AX1PlayerController::OnInputAction_SwitchFlyMode(
    const struct FInputActionInstance &Instance)
{
    this->HandleInput_SwitchFlyMode();
}

void AX1PlayerController::OnInputAction_Zoom(
    const struct FInputActionInstance &Instance)
{
    this->HandleInput_Zoom(-50 * Instance.GetValue().Get<float>());
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

void AX1PlayerController::HandleInput_Fly(float value)
{
    X1_ASSERT_RET_VOID(GetPawn());
    GetPawn()->AddMovementInput(FVector::UpVector, value);
}

void AX1PlayerController::HandleInput_SwitchFlyMode()
{
    X1_ASSERT_RET_VOID(GetCharacter());
    auto Movement = GetCharacter()->GetCharacterMovement();
    X1_ASSERT_RET_VOID(Movement);
    if (!bIsFlying)
        Movement->SetMovementMode(MOVE_Flying);
    else
        Movement->SetMovementMode(MOVE_Walking);
    bIsFlying = !bIsFlying;
    Movement->BrakingDecelerationFlying = 2048.0f;
}

void AX1PlayerController::HandleInput_Zoom(float value)
{
    X1_ASSERT_RET_VOID(GetPawn());
    X1_ASSERT_RET_VOID(GetPawn()->Implements<UX1Zoom>());
    IX1Zoom::Execute_Zoom(GetPawn(), value);
}
