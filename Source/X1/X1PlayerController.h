// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "X1PlayerController.generated.h"

/**
 *
 */
UCLASS()
class X1_API AX1PlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UInputMappingContext *InputMappingContext;

protected:
    virtual void BeginPlay() override;

private:
    void SetupInput();
    void SetupCamera();

    void OnInputAction_MoveForward(const struct FInputActionInstance &Instance);
    void OnInputAction_MoveRight(const struct FInputActionInstance &Instance);
    void OnInputAction_Look(const struct FInputActionInstance &Instance);
    void OnInputAction_Interact(const struct FInputActionInstance &Instance);
    void HandleInput_MoveRight(float Value);
    void HandleInput_MoveForward(float Value);
    void HandleInput_Yaw(float Value);
    void HandleInput_Pitch(float Value);
    void HandleInput_Interact(float Value);
};
