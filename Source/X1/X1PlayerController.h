// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Templates/Casts.h"
#include "X1MouseDetectorConsumer.h"
#include "X1MouseDetectorOverlay.h"
#include "X1PlayerController.generated.h"


UCLASS()
class X1_API AX1PlayerController : public APlayerController,
                                   public IX1MouseDetectorConsumer
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UInputMappingContext *InputMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UStaticMeshComponent *CurrentBuildingMesh;

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    void OnMouseMoveFromMouseDetector_Implementation(
        const FX1OverlayMouseEvent &Event) override;

private:
    bool bIsFlying = false;

    UPROPERTY(EditAnywhere)
    UX1MouseDetectorOverlay *Overlay = nullptr;

    void SetupInput();
    void SetupCamera();
    void SetupMovementComponent();

    void OnInputAction_MoveForward(const struct FInputActionInstance &Instance);
    void OnInputAction_MoveRight(const struct FInputActionInstance &Instance);
    void OnInputAction_Look(const struct FInputActionInstance &Instance);
    void OnInputAction_Interact(const struct FInputActionInstance &Instance);
    void OnInputAction_Fly(const struct FInputActionInstance &Instance);
    void OnInputAction_SwitchFlyMode(
        const struct FInputActionInstance &Instance);
    void OnInputAction_Zoom(const struct FInputActionInstance &Instance);
    void OnInputAction_Build(const struct FInputActionInstance &Instance);

    void HandleInput_MouseMove(const FX1OverlayMouseEvent &Event);
    void HandleInput_MoveRight(float Value);
    void HandleInput_MoveForward(float Value);
    void HandleInput_Yaw(float Value);
    void HandleInput_Pitch(float Value);
    void HandleInput_Interact(float Value);
    void HandleInput_Fly(float value);
    void HandleInput_Zoom(float value);
    void HandleInput_SwitchFlyMode();
    void UpdateGhost();

    AActor *CurrentBuildPiece{};
};
