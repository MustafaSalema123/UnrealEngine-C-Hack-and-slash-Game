// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "MyBTDecorator_CheckAttackRange.generated.h"

/**
 * 
 */
UCLASS()
class HACKANDSLASHCPP_API UMyBTDecorator_CheckAttackRange : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()

	public :

	UMyBTDecorator_CheckAttackRange();
protected:	

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

public:
	float CalculateDistanceFromEnemy(FVector PointA, FVector PointB)const;
};
