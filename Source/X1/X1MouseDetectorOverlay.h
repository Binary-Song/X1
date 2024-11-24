// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
#include "Input/Reply.h"
#include "X1MouseDetectorOverlay.generated.h"

USTRUCT(BlueprintType)
struct FX1OverlayMouseEvent
{
    GENERATED_BODY()
    FGeometry Geometry;
    FPointerEvent MouseEvent;
};

/**
 * This transparent widget is intended for handling mouse move events
 * without having to hide the mouse cursor.
 */
UCLASS()
class X1_API UX1MouseDetectorOverlay : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual FReply NativeOnMouseMove(const FGeometry &InGeometry,
                                     const FPointerEvent &InMouseEvent) override;
};
