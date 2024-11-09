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
    UPROPERTY(BlueprintReadWrite)
    class UInputMappingContext *InputMappingContext;

protected:
    virtual void BeginPlay() override;

private:
    void OnInputAction_MoveForward(const struct FInputActionInstance& Instance);
    void OnInputAction_MoveRight(const struct FInputActionInstance& Instance);
    void OnInputAction_Look(const struct FInputActionInstance& Instance);

    void Jump();
    void MoveRight(float Value);
    void MoveForward(float Value);
    void AddYaw(float Value);
    void AddPitch(float Value);
};
