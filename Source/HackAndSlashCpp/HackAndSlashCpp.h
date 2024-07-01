// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"


static class LOG
{
public:
    LOG(float f)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s"), *FString::SanitizeFloat(f));
    }
    LOG(int a)
    {
         UE_LOG(LogTemp, Warning, TEXT("%d"), a);
    }
    LOG(FString string)
    {
         UE_LOG(LogTemp, Warning, TEXT("%s"), *string);
    }
};