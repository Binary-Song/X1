#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "X1Zoom.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UX1Zoom : public UInterface
{
    GENERATED_BODY()
};

class X1_API IX1Zoom
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Zoom(float delta);
};
