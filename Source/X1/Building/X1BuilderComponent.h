// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "X1BuilderComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class X1_API UX1BuilderComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UX1BuilderComponent();

protected:
	virtual void BeginPlay() override;
};
