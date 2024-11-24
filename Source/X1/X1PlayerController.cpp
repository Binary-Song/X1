// Fill out your copyright notice in the Description page of Project Settings.

#include "X1PlayerController.h"
#include "Building/X1BuildPiece.h"
#include "Components/StaticMeshComponent.h"
#include "CoreGlobals.h"
#include "Engine/Engine.h"
#include "Engine/EngineTypes.h"
#include "Engine/LocalPlayer.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputTriggers.h"
#include "Logging/LogVerbosity.h"
#include "Logging/StructuredLog.h"
#include "Math/MathFwd.h"
#include "Math/Vector2D.h"
#include "PhysicsEngine/PhysicsConstraintActor.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Templates/Casts.h"
#include "UObject/NameTypes.h"
#include "X1Assert.h"
#include "X1BuildHud.h"
#include "X1BuildPiece.h"
#include "X1CollectionUtils.h"
#include "X1Format.h"
#include "X1Grabber.h"
#include "X1Interactable.h"
#include "X1TraceUtils.h"
#include "X1Zoom.h"
#include <cassert>
#include <set>

void AX1PlayerController::BeginPlay()
{
    this->SetupInput();
    this->SetupCamera();
    this->SetShowMouseCursor(true);
    this->SetupMovementComponent();
}

void AX1PlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AX1PlayerController::OnMouseMoveFromMouseDetector_Implementation(
    const FX1OverlayMouseEvent &Event)
{
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
    X1_ASSERT_RET_VOID(GetLocalPlayer());

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
        { TEXT("IA_X1HoldToLook"),            {ETriggerEvent::Triggered,   nullptr}},
        { TEXT("IA_X1MoveForward"),     {ETriggerEvent::Triggered,   &AX1PlayerController::OnInputAction_MoveForward  }},
        { TEXT("IA_X1MoveRight"),       {ETriggerEvent::Triggered,   &AX1PlayerController::OnInputAction_MoveRight    }},
        { TEXT("IA_X1Interact"),        {ETriggerEvent::Started,     &AX1PlayerController::OnInputAction_Interact     }},
        { TEXT("IA_X1Fly"),             {ETriggerEvent::Triggered,   &AX1PlayerController::OnInputAction_Fly          }},
        { TEXT("IA_X1SwitchFlyMode"),   {ETriggerEvent::Started,     &AX1PlayerController::OnInputAction_SwitchFlyMode}},
        { TEXT("IA_X1Zoom"),            {ETriggerEvent::Started,     &AX1PlayerController::OnInputAction_Zoom         }},
        { TEXT("IA_X1Build"),           {ETriggerEvent::Started,     &AX1PlayerController::OnInputAction_Build        }},
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
            if (Data->Callback)
            {
                EnhancedInputComponent->BindAction(
                    Map.Action, Data->Trigger, this, Data->Callback);
            }
        }
    }
};
void AX1PlayerController::SetupCamera()
{
    X1_ASSERT_RET_VOID(PlayerCameraManager);
    PlayerCameraManager->ViewPitchMin = -90.0f;
    PlayerCameraManager->ViewPitchMax = 90.0f;
}

void AX1PlayerController::SetupMovementComponent()
{
    X1_ASSERT_RET_VOID(GetCharacter());
    auto Movement = GetCharacter()->GetCharacterMovement();
    X1_ASSERT_RET_VOID(Movement);
    Movement->SetMovementMode(MOVE_Flying);
    Movement->BrakingFrictionFactor = 5.0f;
    Movement->BrakingDecelerationFlying = 5000.0f;
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

void AX1PlayerController::OnInputAction_Build(
    const struct FInputActionInstance &Instance)
{
}

void AX1PlayerController::HandleInput_MouseMove(
    const FX1OverlayMouseEvent &Event)
{
    Event.MouseEvent.GetScreenSpacePosition();
    FVector2D ScreenPosition = Event.MouseEvent.GetScreenSpacePosition();
    FVector WorldLocation, WorldDirection;
    DeprojectScreenPositionToWorld(
        ScreenPosition.X, ScreenPosition.Y, WorldLocation, WorldDirection);

    FHitResult HitResult;
    GetWorld()->LineTraceSingleByChannel(
        HitResult,
        WorldLocation,
        WorldLocation + WorldDirection * 10000.0f,
        ECC_Visibility);

    if (HitResult.bBlockingHit)
    {
        AActor *HitActor = HitResult.GetActor();
        if (HitActor && HitActor->FindComponentByClass<UStaticMeshComponent>())
        {
            UE_LOG(LogTemp, Log, TEXT("Hit mesh: %s"), *HitActor->GetName());
        }
    }
}

// 注意区别： ControlRotation 是视角朝向， Actor 的 forward 是角色模型的朝向
// 移动的方向应该基于视角的朝向。

void AX1PlayerController::HandleInput_MoveForward(float Value)
{
    if (!Value)
        return;
    X1_ASSERT_RET_VOID(GetPawn());
    FRotator ControlRot = GetControlRotation();
    FVector ForwardVector = FRotationMatrix(ControlRot).GetUnitAxis(EAxis::X);
    GetPawn()->AddMovementInput(ForwardVector, Value);
}

void AX1PlayerController::HandleInput_MoveRight(float Value)
{
    if (!Value)
        return;
    X1_ASSERT_RET_VOID(GetPawn());
    FRotator ControlRot = GetControlRotation();
    FVector RightVector = FRotationMatrix(ControlRot).GetUnitAxis(EAxis::Y);
    GetPawn()->AddMovementInput(RightVector, Value);
}

void AX1PlayerController::HandleInput_Yaw(float Value)
{
    if (!Value)
        return;
    X1_ASSERT_RET_VOID(GetPawn());
    this->AddYawInput(Value);
    //   this->UpdateGhost();
}

void AX1PlayerController::HandleInput_Pitch(float Value)
{
    if (!Value)
        return;
    X1_ASSERT_RET_VOID(GetPawn());
    this->AddPitchInput(Value);
    //   this->UpdateGhost();
}

void AX1PlayerController::HandleInput_Interact(float Value)
{
    // AActor *HitActor{};
    // const FX1TraceSearch SearchParams = []() {
    //     FX1TraceSearch Params{};
    //     Params.TraceDistance = 300;
    //     Params.bDrawDebugLines = true;
    //     Params.ActorFilterFunction = [](AActor *Actor) {
    //         if (Actor->Implements<UX1Interactable>())
    //             return IX1Interactable::Execute_InteractType(Actor) !=
    //                    EInteractType::None;
    //         return false;
    //     };
    //     return Params;
    // }();
    // FX1TraceSearchResult SearchResult;
    // FX1TraceUtils::TraceSearch(SearchResult, this, SearchParams);
    // auto FindResult = UX1CollectionUtil::FindMinElement(
    //     SearchResult.Items,
    //     [](const FX1CameraTraceSearchItem &a,
    //        const FX1CameraTraceSearchItem &b) {
    //         return FVector2D{a.RotOffset.Yaw, a.RotOffset.Pitch}
    //                    .SquaredLength() <
    //                FVector2D{b.RotOffset.Yaw, b.RotOffset.Pitch}
    //                    .SquaredLength();
    //     });

    // if (!FindResult.bFound)
    //     return;

    // UStaticMeshComponent *StaticMeshComponent =
    //     FindResult.Item.Actor->FindComponentByClass<UStaticMeshComponent>();
    // X1_ASSERT_RET_VOID(StaticMeshComponent);
    // // this->CurrentBuildingMesh = StaticMeshComponent;
    // // this->CurrentBuildingMesh->SetCollisionEnabled(
    // //     ECollisionEnabled::NoCollision);

    // auto TargetActor = FindResult.Item.Actor;
    // X1_ASSERT_RET_VOID(TargetActor);

    // if (Actor1)
    //     Actor2 = TargetActor;
    // else
    //     Actor1 = TargetActor;
    // X1_ASSERT_RET_VOID(Actor1);
    // X1_ASSERT_RET_VOID(Actor2);

    // auto pc1 = Actor1->FindComponentByClass<UX1BuildPiece>();
    // auto pc2 = Actor2->FindComponentByClass<UX1BuildPiece>();
    // X1_ASSERT_RET_VOID(pc1);
    // X1_ASSERT_RET_VOID(pc2);

    // X1_ASSERT_RET_VOID(UX1BuildPiece::Attach(
    //     pc1, TEXT("DefaultPort"), pc2, TEXT("DefaultPort")));

    // todo: PickUp Logic commented out.

    // const EInteractType IT =
    //     IX1Interactable::Execute_InteractType(FindResult.Item.Actor);
    // if (IT == EInteractType::Pickup)
    // {
    //     X1_ASSERT_RET_VOID(GetPawn());
    //     X1_ASSERT_RET_VOID(GetPawn()->Implements<UX1Grabber>());

    //     const FX1GrabParam GrabParam = [&]() {
    //         FX1GrabParam P;
    //         P.GrabbedBoneName = FindResult.Item.HitResult.BoneName;
    //         P.GrabbedComponent = FindResult.Item.HitResult.Component.Get();
    //         return P;
    //     }();

    //     IX1Grabber::Execute_Ungrab(GetPawn());
    //     IX1Grabber::Execute_Grab(GetPawn(), GrabParam);
    // }
}

void AX1PlayerController::HandleInput_Fly(float value)
{
    X1_ASSERT_RET_VOID(GetPawn());
    GetPawn()->AddMovementInput(FVector::UpVector, value);
}

void AX1PlayerController::HandleInput_SwitchFlyMode()
{
}

void AX1PlayerController::HandleInput_Zoom(float value)
{
    X1_ASSERT_RET_VOID(GetPawn());
    X1_ASSERT_RET_VOID(GetPawn()->Implements<UX1Zoom>());
    IX1Zoom::Execute_Zoom(GetPawn(), value);
}

void AX1PlayerController::UpdateGhost()
{
    // Get the mouse position in screen space
    FVector2D MousePosition;
    if (!GetMousePosition(MousePosition.X, MousePosition.Y))
        return;
    // X1_LOG("X = {0}, Y = {1}", MousePosition.X, MousePosition.Y);

    if (!this->CurrentBuildingMesh)
        return;

    // Convert the mouse position to world space
    FVector WorldLocation, WorldDirection;
    if (!DeprojectScreenPositionToWorld(
            MousePosition.X, MousePosition.Y, WorldLocation, WorldDirection))
        return;

    // Trace from the camera to the world position
    FHitResult HitResult;
    if (!GetWorld()->LineTraceSingleByChannel(
            HitResult,
            WorldLocation,
            WorldLocation + WorldDirection * 10000.f,
            ECC_Visibility))
        return;

    // Move the mesh to the hit location
    this->CurrentBuildingMesh->SetWorldLocation(HitResult.Location);
}