#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "UObject/Interface.h"
#include "X1Grabber.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UX1Grabber : public UInterface
{
    GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct X1_API FX1GrabParam
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UPrimitiveComponent *GrabbedComponent{};

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName GrabbedBoneName;
};

class X1_API IX1Grabber
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Grab(const FX1GrabParam &params);
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Ungrab();
};
