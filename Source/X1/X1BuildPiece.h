// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Containers/Array.h"
#include "Components/ActorComponent.h"
#include "Engine/StaticMesh.h"
#include "X1BuildPiece.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class X1_API UX1BuildPiece : public UActorComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UX1BuildPiece();

    virtual void TickComponent(
        float DeltaTime,
        ELevelTick TickType,
        FActorComponentTickFunction *ThisTickFunction) override;

    static bool Attach(UX1BuildPiece *Piece1,
                       FName Socket1,
                       UX1BuildPiece *Piece2,
                       FName Socket2);

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FName> SocketComponents;

private:
    virtual void BeginPlay() override;
    static bool RotateFaceToFace(UX1BuildPiece *Piece1,
                                 FName Socket1Name,
                                 UX1BuildPiece *Piece2,
                                 FName Socket2Name);
    static bool AlignLocation(UX1BuildPiece *Piece1,
                              FName Socket1Name,
                              UX1BuildPiece *Piece2,
                              FName Socket2Name);
    static bool SetupConstraint(UX1BuildPiece *Piece1,
                                FName Socket1Name,
                                UX1BuildPiece *Piece2,
                                FName Socket2Name);
};
