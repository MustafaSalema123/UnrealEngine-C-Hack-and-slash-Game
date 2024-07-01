// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class HACKANDSLASHCPP_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AEnemyAIController();	

	UPROPERTY(VisibleAnyWhere)
	class UAIPerceptionComponent *PerceComp;

	UFUNCTION()
	void AiSeePlayer(AActor* Actor , FAIStimulus Stimulus);

protected:

	virtual void BeginPlay() override;

	private:
	class AMyCharacterController *Target;

	UPROPERTY(EditDefaultsOnly )
	TSubclassOf<class UAISense> SenseType;
};
