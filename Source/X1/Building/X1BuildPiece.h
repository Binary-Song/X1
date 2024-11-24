// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Containers/Array.h"
#include "Components/ActorComponent.h"
#include "Engine/StaticMesh.h"
#include "X1BuildPiece.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class X1_API UX1BuildPiece : public UActorComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UX1BuildPiece();

    static bool Attach(UX1BuildPiece *Piece1,
                       FName Port1,
                       UX1BuildPiece *Piece2,
                       FName Port2);

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FName> Ports;

private:
    virtual void BeginPlay() override;
    virtual void TickComponent(
        float DeltaTime,
        ELevelTick TickType,
        FActorComponentTickFunction *ThisTickFunction) override;
    static bool RotateFaceToFace(UX1BuildPiece *Piece1,
                                 FName Port1,
                                 UX1BuildPiece *Piece2,
                                 FName Port2);
    static bool AlignLocation(UX1BuildPiece *Piece1,
                              FName Port1,
                              UX1BuildPiece *Piece2,
                              FName Port2);
    static bool SetupConstraint(UX1BuildPiece *Piece1,
                                FName Port1,
                                UX1BuildPiece *Piece2,
                                FName Port2);
};
