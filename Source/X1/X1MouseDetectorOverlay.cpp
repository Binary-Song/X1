// Fill out your copyright notice in the Description page of Project Settings.

#include "X1MouseDetectorOverlay.h"
#include "Input/Reply.h"
#include "X1Assert.h"
#include "X1MouseDetectorConsumer.h"

FReply UX1MouseDetectorOverlay::NativeOnMouseMove(
    const FGeometry &InGeometry, const FPointerEvent &InMouseEvent)
{
    {
        auto PlayerController = this->GetOwningPlayer();
        X1_ASSERT_GOTO(PlayerController);
        X1_ASSERT_GOTO(
            PlayerController->Implements<UX1MouseDetectorConsumer>());
        FX1OverlayMouseEvent e;
        e.Geometry = InGeometry;
        e.MouseEvent = InMouseEvent;
        IX1MouseDetectorConsumer::Execute_OnMouseMoveFromMouseDetector(
            PlayerController, e);
        return FReply::Handled();
    }
fail:
    return FReply::Unhandled();
}
