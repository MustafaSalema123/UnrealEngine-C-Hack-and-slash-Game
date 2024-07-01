// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "MyDamageType.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "MyCharacterController.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/StaticMeshComponent.h"

#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"

#include "EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	targetIndicator = CreateDefaultSubobject<UWidgetComponent>("target Indicator");
	targetIndicator->SetupAttachment(GetRootComponent());


	
	//weaponKatana =  CreateDefaultSubobject<USkeletalMeshComponent>(FName("Weapon katana "));
	
	 //SpawnBloodCompo = CreateDefaultSubobject<USceneComponent>(FName("SapwnParticle"));
	//SpawnBloodCompo->SetupAttachment(GetRootComponent());
}
void AEnemyCharacter::AttachWeaponHand(class USkeletalMeshComponent *AttachInhand)
{
	AttachInhand->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale , FName("WeaponSocket"));
}
// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	character = Cast<AMyCharacterController>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	OntargetIndicatorShow(false);
	AnimInstance = GetMesh()->GetAnimInstance();


		//weaponKatana->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale , FName("WeaponSocket"));

		EnemyController = Cast<AEnemyAIController>(GetController());
		if(EnemyController)
		{
			Strafe = true;
		EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("Strafe") , Strafe);

		}
		if(character){

				if (EnemyController->GetBlackboardComponent())
			{
				EnemyController->GetBlackboardComponent()->SetValueAsObject(
					TEXT("Player"),
					character);
			}
		}

			//UKismetSystemLibrary::K2_SetTimer(this , FString("SetDefense") , F , true );

			//GetWorldTimerManager().SetTimer(DefenseTimer , this , true  , )
		
			GetWorldTimerManager().SetTimer(
		DefenseTimer,
		this,
		&AEnemyCharacter::SetDefense,
		UKismetMathLibrary::RandomFloatInRange(10.0f , 15.0f) ,
		true
	);

}

void AEnemyCharacter::SetDefense()
{

   bDefense = true;

    // Generate a random delay between 3 to 5 seconds
    float RandomDelay = FMath::RandRange(3.0f, 5.0f);

	    FLatentActionInfo LatentInfo;
    LatentInfo.CallbackTarget = this;
	LatentInfo.Linkage = 1;
    LatentInfo.UUID = 1; 
	LatentInfo.ExecutionFunction = FName(TEXT("ResetSetDefenseFalse"));
	GetWorldTimerManager().ClearTimer(DefenseTimer);

	
    // Call the Delay function to execute a lambda after the random delay
    UKismetSystemLibrary::Delay(this, RandomDelay,LatentInfo);


	
    
}

void AEnemyCharacter::ResetSetDefenseFalse()
{
	
	   bDefense = false;
}
void AEnemyCharacter::OntargetIndicatorShow(bool canShow)
{
	
			targetIndicator->SetVisibility(canShow);
	
}
void AEnemyCharacter::ToggleTargetIndicator(bool bShow)
{
	
		targetIndicator->SetVisibility(bShow);
	
}
 void AEnemyCharacter::PlayCharacterAnimMontage(class UAnimMontage* MontageToPlay , FName Section )
 {

	if(MontageToPlay && AnimInstance)
	{
		AnimInstance ->Montage_Play(MontageToPlay);
	    AnimInstance ->Montage_JumpToSection(FName(Section));
	}	
 }

void AEnemyCharacter::SetDeadState(bool Died)
{
	isDead = Died;
}

bool AEnemyCharacter::IsDied()
 {
	 return isDead;

 }

 void AEnemyCharacter::ActivateDeath()
 {
  isDead = true;

 }

  void AEnemyCharacter::DisableCollision()
 {

  	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 }


float AEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const &DamageEvent, AController *EventInstigator, AActor *DamageCauser)
{

	if(bDefense)
	{
			if (AnimInstance && DefenseMontage)
					AnimInstance->Montage_Play(DefenseMontage );

				
	}else
	{

 	AnyDamage(DamageAmount , DamageEvent.DamageTypeClass , EventInstigator , DamageCauser);
	}
    return DamageAmount;
}

void AEnemyCharacter::AnyDamage(float DamageAmount, TSubclassOf<UDamageType> DamageType, AController *InstigatedBy, AActor *DamageCauser)
{

	FName Section("");

	
		UMyDamageType* Ut_Main = Cast<UMyDamageType>(DamageType.GetDefaultObject());
		if(Ut_Main){
		switch (Ut_Main->DamageDirection)
		{
		   	case E_DamageDirection::DMG_Middle:			
			Section = FName("FromMiddle");
            break;
       		case E_DamageDirection::DMG_Left:
			Section = FName("FromLeft");
            break;
        	case E_DamageDirection::DMG_Right:
			Section = FName("FromRight");
            break;
			case E_DamageDirection::DMG_KnockBack:
		
			Section = FName("FromKnockBack");
            break;
			case E_DamageDirection::DMG_launching:
				 
				if (AnimInstance && AirHitReactioMontage)
					AnimInstance->Montage_Play(AirHitReactioMontage);
			GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
				launchThisEnemy(); // call in event 
			//Section = FName("FromKnockBack");
            break;
			    case E_DamageDirection::DMG_KnockDown:
				KnockDownEnemy();
			//Section = FName("FromKnockBack");
            break;
        default:
            // Handle default case if needed
            break;
			}
		}
			StopBuffer();
			BufferAmount = 5.0f;
			StartBuffer();

		FRotator SofttargetRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), DamageCauser->GetActorLocation());
		FRotator MakeRotator(GetActorRotation().Pitch, SofttargetRot.Yaw, GetActorRotation().Roll);
		
		SetActorRotation(MakeRotator);
		if(GetCharacterMovement()->MovementMode != EMovementMode::MOVE_Flying)
			PlayHitReactMontage(Section , DamageCauser);

}
void AEnemyCharacter::PlayHitReactMontage(const FName& SectionName , AActor* DamageCauser)
{
	
	if(GetCharacterMovement()->IsFlying() || GetCharacterMovement()->IsFalling())
	{
		
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
		const FVector playerLocation =  DamageCauser->GetActorLocation();
		const FVector perfectLocation = FVector(GetActorLocation().X , GetActorLocation().Y , playerLocation.Z );
		SetActorLocation(perfectLocation , true);
			if (AnimInstance && AirHitReactioMontage){
					AnimInstance->Montage_Play(AirHitReactioMontage , 0.8f);
			 // UE_LOG(LogTemp, Warning, TEXT("Air is working ") );
	//	AnimInstance->Montage_JumpToSection(SectionName, AirHitReactioMontage);
		}
		
	} else
	
	{
		if (AnimInstance && GroundHitReactioMontage && GetCharacterMovement()->MovementMode != EMovementMode::MOVE_Flying){
		AnimInstance->Montage_Play(GroundHitReactioMontage);
		AnimInstance->Montage_JumpToSection(SectionName, GroundHitReactioMontage);
		}
	}
}

void AEnemyCharacter::BufferTimer(float Value)
{
	const FVector Start = GetActorLocation();
	const FVector forward =Start +  (-GetActorForwardVector()) * BufferAmount;


	SetActorLocation(FMath::Lerp(Start, forward, Value ), true);
}
void AEnemyCharacter::ResetAirComabat()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
}

void AEnemyCharacter::KnockDownEnemy()
{
	
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
	if (AnimInstance && knockDownMonatge){
		 //UE_LOG(LogTemp, Warning, TEXT("Knock doen is working well ") );
		AnimInstance->Montage_Play(knockDownMonatge);
			// /AnimInstance->Montage_JumpToSection(FName("Smash"), knockDownMonatge);
		}

		LaunchCharacter( FVector::DownVector * 10000.f , true , true);
}

void AEnemyCharacter::FinishedEnemy(AActor* player)
{
	// add a logic of isDied for last animation
	FRotator targetRot = 	UKismetMathLibrary::FindLookAtRotation(GetActorLocation() ,player->GetActorLocation());

		 int32 rand  = FMath::RandRange(0 , 2);
		 FName Sections("");
		SetActorRotation(targetRot);
		switch (rand)
		{
			case 0:
			Sections =FName("Tar1") ;
			ExecutionName = FName("Exe1");
			break;
			case 1:
			Sections =FName("Tar2") ;
			ExecutionName = FName("Exe2");
			break;

			case 2:
			Sections =FName("Tar3") ;
			ExecutionName = FName("Exe3");
			break;
				
			}
	
		if (AnimInstance && finishingAttackMontage){
			
	//	AnimInstance->Montage_Play(finishingMontage[rand].TargetMontage);
		AnimInstance->Montage_Play(finishingAttackMontage);
	    AnimInstance->Montage_JumpToSection(FName(Sections));
		}	
		//Stop rotation in teh Rotations
		//End the last me off = capsule  no collision and mesh collsion off

		//Exevutsion Dead Ecen
}


void AEnemyCharacter::SwordTrace()
{
	FHitResult hitResult;
	TArray<AActor*> Ignore;
	Ignore.Add(this);

	FVector StartLocation{GetActorLocation() + FVector(0.0f , 0.0f , 50.0f)};
		FVector EndLocation{ StartLocation  + (GetActorForwardVector()  * 200.0f)};

		UKismetSystemLibrary::SphereTraceSingle(this , StartLocation , EndLocation , 50.0f , UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility) ,
		false , Ignore , EDrawDebugTrace::ForDuration , hitResult , true);

		if(hitResult.bBlockingHit)
		{
				FString TargetName = hitResult.GetActor()->GetName();
		UE_LOG(LogTemp, Warning, TEXT("Hit Result object name: %s"), *TargetName);
			AMyCharacterController* Player = Cast<AMyCharacterController>(hitResult.GetActor());

			if(Player)
			{
					//camera shake if want
					//blood spawn
				//	UGameplayStatics::SpawnEmitterAttached(Player->BloodFx , Player->GetMesh() , FName("Blood"));
				//Play sound At locatiomn
				//UGameplayStatics::PlaySoundAtLocation(GetWorld() , hitSound ,hitResult.Location , 1.f);

				
				FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(Player->GetActorLocation() , this->GetActorLocation());
				Player->SetActorRotation(NewRotation);

					UGameplayStatics::ApplyDamage(Player , 20 , GetInstigatorController(), this , UDamageType::StaticClass());
				//hit reaction Damage Call Also

			}
		}

		
}

void AEnemyCharacter::PlayMontageAttack()
{
	if(bDefense ) return;

	float rand = FMath::RandRange(0 , 5);

	if(rand == 1)
	{

		if(  EnemyDodgeMontage)
	{
		AnimInstance->Montage_Play(EnemyDodgeMontage , 1.0f);
		//Add a random attack Montage
	}

	}else
	{
	if(  AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage , 1.0f);
		//Add a random attack Montage
	}
	}


}
// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

