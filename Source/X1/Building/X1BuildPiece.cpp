// Fill out your copyright notice in the Description page of Project Settings.

#include "X1BuildPiece.h"
#include "Engine/StaticMesh.h"
#include "Math/MathFwd.h"
#include "Math/UnrealMathUtility.h"

#include "Templates/Casts.h"
#include "../X1Assert.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Engine/World.h"
#include "PhysicsEngine/PhysicsConstraintActor.h"

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
    X1_LOG("Successfully Attached");
    return true;
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
                                    FName Socket1Name,
                                    UX1BuildPiece *Piece2,
                                    FName Socket2Name)
{

    UStaticMeshComponent *SocketMesh1 = GetSocketMesh(Piece1, Socket1Name);
    X1_ASSERT_RET_EMPTY(SocketMesh1);
    UStaticMeshComponent *SocketMesh2 = GetSocketMesh(Piece2, Socket2Name);
    X1_ASSERT_RET_EMPTY(SocketMesh2);

    // Create a physics constraint actor
    FActorSpawnParameters Params;
    Params.Name = FName("BuildSystemPhysicsConstraint");
    APhysicsConstraintActor *ConstraintActor =
        GWorld->SpawnActor<APhysicsConstraintActor>(Params);
    X1_ASSERT_RET_EMPTY(ConstraintActor);

    // Attach the constraint actor to the mesh's owner
    X1_ASSERT_RET_EMPTY(ConstraintActor->SetActorLocation(
        SocketMesh1->GetComponentTransform().GetLocation()));
    X1_ASSERT_RET_EMPTY(ConstraintActor->AttachToActor(
        SocketMesh2->GetOwner(),
        FAttachmentTransformRules::KeepWorldTransform));

    // Set up the constraint component
    UPhysicsConstraintComponent *ConstraintComp =
        ConstraintActor->GetConstraintComp();
    X1_ASSERT_RET_EMPTY(ConstraintComp);

    auto pc1 =
        SocketMesh1->GetOwner()->FindComponentByClass<UStaticMeshComponent>();
    auto pc2 =
        SocketMesh2->GetOwner()->FindComponentByClass<UStaticMeshComponent>();
    X1_ASSERT_RET_EMPTY(pc1);
    X1_ASSERT_RET_EMPTY(pc2);

    // Connect the two ports
    ConstraintComp->SetConstrainedComponents(pc1, NAME_None, pc2, NAME_None);
    // Set constraint properties
    ConstraintComp->SetAngularSwing1Limit(
        EAngularConstraintMotion::ACM_Locked, 0.0f);
    ConstraintComp->SetAngularSwing2Limit(
        EAngularConstraintMotion::ACM_Locked, 0.0f);
    ConstraintComp->SetAngularTwistLimit(
        EAngularConstraintMotion::ACM_Locked, 0.0f);
    return true;
}