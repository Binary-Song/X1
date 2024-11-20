// Fill out your copyright notice in the Description page of Project Settings.

#include "X1BuildPiece.h"
#include "Engine/StaticMesh.h"
#include "Math/MathFwd.h"
#include "Math/UnrealMathUtility.h"
#include "Templates/Casts.h"
#include "X1Assert.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"

static UStaticMeshComponent *GetSocketMesh(UX1BuildPiece *Piece,
                                           FName SocketName)
{
    X1_ASSERT_RET_EMPTY(Piece);
    AActor *Actor = Piece->GetOwner();
    X1_ASSERT_RET_EMPTY(Actor);
    auto Mesh = Cast<UStaticMeshComponent>(
        Actor->GetDefaultSubobjectByName(SocketName));
    return Mesh;
}

// Sets default values for this component's properties
UX1BuildPiece::UX1BuildPiece()
{
    PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UX1BuildPiece::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void UX1BuildPiece::TickComponent(float DeltaTime,
                                  ELevelTick TickType,
                                  FActorComponentTickFunction *ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UX1BuildPiece::Attach(UX1BuildPiece *Piece1,
                           FName Socket1Name,
                           UX1BuildPiece *Piece2,
                           FName Socket2Name)
{
    if (!RotateFaceToFace(Piece1, Socket1Name, Piece2, Socket2Name))
        return false;
    if (!AlignLocation(Piece1, Socket1Name, Piece2, Socket2Name))
        return false;
    if (!SetupConstraint(Piece1, Socket1Name, Piece2, Socket2Name))
        return false;
}

bool UX1BuildPiece::RotateFaceToFace(UX1BuildPiece *Piece1,
                                     FName Socket1Name,
                                     UX1BuildPiece *Piece2,
                                     FName Socket2Name)
{

    UStaticMeshComponent *SocketMesh1 = GetSocketMesh(Piece1, Socket1Name);
    X1_ASSERT_RET_EMPTY(SocketMesh1);
    UStaticMeshComponent *SocketMesh2 = GetSocketMesh(Piece2, Socket2Name);
    X1_ASSERT_RET_EMPTY(SocketMesh2);

    const FVector Socket1Location = SocketMesh1->GetComponentLocation();
    const FVector Socket2Location = SocketMesh2->GetComponentLocation();
    const FQuat Socket1Rotation = SocketMesh1->GetComponentQuat();
    const FQuat Socket2Rotation = SocketMesh2->GetComponentQuat();
    const FVector Socket2YAxis = Socket2Rotation.GetAxisY();
    const FQuat DestRotation = FQuat(Socket2YAxis, PI) * Socket2Rotation;
    AActor *Actor1 = Piece1->GetOwner();
    Actor1->SetActorRotation((DestRotation * Actor1->GetActorQuat()).Rotator());
    X1_LOG("Successfully Changed Actor1's Rotation");
    return true;
}

bool UX1BuildPiece::AlignLocation(UX1BuildPiece *Piece1,
                                     FName Socket1Name,
                                     UX1BuildPiece *Piece2,
                                     FName Socket2Name)
{

    UStaticMeshComponent *SocketMesh1 = GetSocketMesh(Piece1, Socket1Name);
    X1_ASSERT_RET_EMPTY(SocketMesh1);
    UStaticMeshComponent *SocketMesh2 = GetSocketMesh(Piece2, Socket2Name);
    X1_ASSERT_RET_EMPTY(SocketMesh2);

    const FVector Socket1Location = SocketMesh1->GetComponentLocation();
    const FVector Socket2Location = SocketMesh2->GetComponentLocation();
    const FVector LocationDifference = Socket2Location - Socket1Location;

    AActor *Actor1 = Piece1->GetOwner();
    Actor1->SetActorLocation(Actor1->GetActorLocation() + LocationDifference);
    return true;
}

bool UX1BuildPiece::SetupConstraint(UX1BuildPiece *Piece1,
                                    FName Socket1,
                                    UX1BuildPiece *Piece2,
                                    FName Socket2)
{
    return true;
}