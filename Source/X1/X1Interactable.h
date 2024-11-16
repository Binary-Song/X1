// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "UObject/Interface.h"
#include "X1Interactable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UX1Interactable : public UInterface
{
    GENERATED_BODY()
};

UENUM(BlueprintType)
enum class EInteractType : uint8
{
    None UMETA(DisplayName = "None"),
    Pickup UMETA(DisplayName = "Pickup"),
};

class X1_API IX1Interactable
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    EInteractType InteractType() const;
};
