// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "X1MouseDetectorOverlay.h"
#include "X1MouseDetectorConsumer.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UX1MouseDetectorConsumer : public UInterface
{
    GENERATED_BODY()
};

/**
 * If PlayerController impls this, it will receive the mouse move
 * events even when the mouse is shown.
 */
class X1_API IX1MouseDetectorConsumer
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void OnMouseMoveFromMouseDetector(const FX1OverlayMouseEvent &Event);
};
