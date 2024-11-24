// Fill out your copyright notice in the Description page of Project Settings.

#include "X1BuildHud.h"
#include "Engine/Engine.h"
#include "X1BuildHud.h"
#include "X1Assert.h"
#include <cassert>
 
void AX1BuildHud::BeginPlay()
{
    AX1BuildHud::CreateWidgets();

    X1_ASSERT_RV(this->Overlay);
}
