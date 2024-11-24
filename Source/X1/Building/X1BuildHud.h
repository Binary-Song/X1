// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UObject/ObjectMacros.h"
#include "X1Assert.h"
#include "X1MouseDetectorOverlay.h"
#include "X1BuildHud.generated.h"


/**
 *
 */
UCLASS(BlueprintType)
class X1_API AX1BuildHud : public AHUD
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UX1MouseDetectorOverlay *Overlay;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void CreateWidgets();
    virtual void CreateWidgets_Implementation()
    {
        X1_ASSERT_RV(0, "Not Implemented!");
    }

private:
    virtual void BeginPlay() override;
};
