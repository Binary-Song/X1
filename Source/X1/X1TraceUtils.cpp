// Fill out your copyright notice in the Description page of Project Settings.

#include "X1TraceUtils.h"
#include "Components/SceneComponent.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/HitResult.h"
#include "GameFramework/Pawn.h"
#include <cmath>
#include <numbers>
#include "Engine/GameViewportClient.h"
#include "Logging/LogMacros.h"
#include "Math/MathFwd.h"
#include "DrawDebugHelpers.h"
#include <unordered_set>
#include "X1Assert.h"
 
static bool CheckActor(const FX1TraceSearch &Params, AActor *actor)
{
    if (Params.ActorFilterFunction)
        return Params.ActorFilterFunction(actor);
    return true;
}

void FX1TraceUtils::TraceSearch(FX1TraceSearchResult &Result, APlayerController *PlayerController,
                                      const FX1TraceSearch &Params)
{
    Result.Items = {};
    int count = 0;
    float angle = 0;
    float radius = 0;

    X1_ASSERT_RET_VOID(PlayerController->GetPawn());

    FRotator rot;
    FVector start = PlayerController->GetPawn()->GetActorLocation();
    FVector _;

    PlayerController->GetPlayerViewPoint(_, rot);

    int tracesLeft = 20000;
    std::unordered_set<AActor*> dedup;
    for (float x = Params.MinAngles.X; x <= Params.MaxAngles.X; x += Params.StepSizes.X)
    {
        for (float y = Params.MinAngles.Y; y <= Params.MaxAngles.Y; y += Params.StepSizes.Y)
        {
            FRotator offset{x, y, 0};
            FRotator currentRot = rot + offset;
            FVector end = start + currentRot.Vector() * Params.TraceDistance;
            FHitResult hit;
            FCollisionQueryParams p;
            p.bDebugQuery = true;
            p.AddIgnoredActor(PlayerController->GetPawn()); // Ignore the player pawn
            bool bHit = GWorld->LineTraceSingleByChannel(hit, start, end, ECC_Visibility, p);

            if (Params.bDrawDebugLines)
                DrawDebugLine(GWorld, start, end, FColor::Green, true, 1, 0, 1);
            if (bHit)
            {
                auto actor = hit.GetActor();
                if (!dedup.contains(actor) && CheckActor(Params, actor))
                {
                    FX1CameraTraceSearchItem item;
                    item.Actor = actor;
                    item.RotOffset = offset;
                    item.HitResult = hit;
                    Result.Items.Push(item);
                }
                dedup.insert(actor);
            }
            tracesLeft--;
            X1_ASSERT_RET_VOID(tracesLeft, "{}", "too many traces");
        }
    }
}
