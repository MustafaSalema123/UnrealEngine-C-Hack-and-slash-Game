// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "MyDamageType.generated.h"


UENUM(BlueprintType)
enum class E_DamageDirection : uint8
{
    DMG_Left UMETA(DisplayName = "Left"),
    DMG_Right UMETA(DisplayName = "Right"),
    DMG_Middle UMETA(DisplayName = "Middle"),
    DMG_KnockBack UMETA(DisplayName = "KnockBack"),
    DMG_launching UMETA(DisplayName = "launching"),
    DMG_KnockDown UMETA(DisplayName = "KnockDown")
};

/**
 * 
 */
UCLASS()
class HACKANDSLASHCPP_API UMyDamageType : public UDamageType
{
	GENERATED_BODY()

	public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    E_DamageDirection DamageDirection;

	FORCEINLINE E_DamageDirection GetDamageType ()const { return  DamageDirection;}
	FORCEINLINE void SetDamageType (  E_DamageDirection DamageDirectionss) { DamageDirection =  DamageDirectionss;}
	
};
