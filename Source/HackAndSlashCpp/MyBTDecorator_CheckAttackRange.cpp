// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTDecorator_CheckAttackRange.h"
#include "EnemyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MyCharacterController.h"
#include "EnemyAIController.h"

UMyBTDecorator_CheckAttackRange::UMyBTDecorator_CheckAttackRange()
{
    NodeName = "Check Attack Range";
}

bool UMyBTDecorator_CheckAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const {

    bool Valid = Super::CalculateRawConditionValue(OwnerComp , NodeMemory);

    if(!Valid)
        return false;

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(OwnerComp.GetAIOwner()->GetPawn());
    if(Enemy->bDefense)
    {   

       if(Enemy->GetDistanceTo(Enemy->character) <= 200.0f)
       {


            return true;
       }
        
    }

    return false;
}

float UMyBTDecorator_CheckAttackRange::CalculateDistanceFromEnemy(FVector PointA, FVector PointB) const
{
    //float Distance = ;
    return FVector::Distance(PointA , PointB);
}


