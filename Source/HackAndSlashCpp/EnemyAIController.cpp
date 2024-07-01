// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "MyCharacterController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Sight.h"

AEnemyAIController::AEnemyAIController()
{
    PerceComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionCompo"));
}

void AEnemyAIController::BeginPlay()
{
    Super::BeginPlay();

    Target  = Cast<AMyCharacterController>(UGameplayStatics::GetPlayerCharacter(GetWorld() , 0));
    PerceComp->OnTargetPerceptionUpdated.AddDynamic(this ,&AEnemyAIController::AiSeePlayer );


}



void AEnemyAIController::AiSeePlayer(AActor *Actor, FAIStimulus Stimulus)
{
    FAISenseID sight_Id = Stimulus.Type;
    if(sight_Id == UAISense::GetSenseID(SenseType))
    {
         if(Stimulus.WasSuccessfullySensed())
        {
            GetBlackboardComponent()->SetValueAsObject(FName("Player") , Target);
        }
        //  else
        // {
        //  //   GetBlackboardComponent()->ClearValue("Player");
        // }
    }
   else
   {
    //for hearing perseption
        //GetBlackboardComponent()->SetValueAsObject(FName("Player") , Stimulus.StimulusLocation);
   }
   
}
