// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Delegates/DelegateCombinations.h"
#include "Engine/HitResult.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Math/MathFwd.h"
#include "Templates/Tuple.h"
#include "UObject/ObjectMacros.h"
#include "Templates/Function.h"
#include <functional>

struct FX1TraceSearch
{
    float TraceDistance = 500;

    /**
     * Minimum yaw and pitch angles to scan.
     */
    FVector2f MinAngles{-45, -45};

    /**
     * Maximum yaw and pitch angles to scan.
     */
    FVector2f MaxAngles{45, 45};

    /**
     * Step sizes between traces in angles.
     */
    FVector2f StepSizes{5, 5};
    /**
     * Filters the result. True means to keep the actor, false means to discard.
     */
    std::function<bool(class AActor *actor)> ActorFilterFunction;

    bool bDrawDebugLines = false;
};

struct FX1CameraTraceSearchItem
{
    AActor *Actor;

    /**
     * The angles required to rotate the camera from its current rot to center
     * this actor in the viewport. If the value is (0,0,0), the actor is already
     * centered in the viewport.
     */
    FRotator RotOffset;

    /**
     * The hit result from the line trace.
     */
    FHitResult HitResult;
};

struct FX1TraceSearchResult
{
    TArray<FX1CameraTraceSearchItem> Items;
};

class FX1TraceUtils
{
public:
    static void TraceSearch(FX1TraceSearchResult &Result,
                            APlayerController *PlayerController,
                            const FX1TraceSearch &Params);

private:
    static bool SphericalTrace(AActor *&HitActor,
                               APlayerController *PlayerController,
                               float TraceDistance, float yawOffset = 0,
                               float pitchOffset = 0);
};
