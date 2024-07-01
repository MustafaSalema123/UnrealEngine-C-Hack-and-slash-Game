// Fill out your copyright notice in the Description page of Project Settings.

#include "ShadowStrikeComponent.h"
#include "Camera/CameraComponent.h"
#include "MyCharacterController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "EnemyCharacter.h"
// /#include "UObject/UObjectGlobals.h"
#include "Components/CapsuleComponent.h"
#include "HackAndSlashCpp.h"

#include "Kismet/KismetMathLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "StrikeInterface.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Sound/SoundCue.h"
//#include "Engine/CollisionProfile.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
// Sets default values for this component's properties
UShadowStrikeComponent::UShadowStrikeComponent()

{
	//Super::(objectininTialize);
CurrntDistace = 1000.f;
	if(!FloatC){
		//static ConstructorHelpers::FObjectFinder<UCurveFloat>CURVE("/Game/gfrgfr");
		//CURVE.Succeeded();
		//FloatC =  CURVE.Object;

	}
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

		  // Set default values
    DashDistance = 1300.0f; // Set your desired dash distance
    DashCooldown = 0.45f;    // Set your desired dash cooldown time
    DashSpeedMultiplier = 6.0f; // Set your desired dash speed multiplier
	bCanDash = true;
	

	// ...
}

// Called when the game starts
void UShadowStrikeComponent::BeginPlay()
{
	Super::BeginPlay();
	ACharacter = Cast<AMyCharacterController>(GetOwner());
	// ...
}

// Called every frame
void UShadowStrikeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	PerformTargetfromCenterOfScreen();
	//StartInitialStrike();

	//TraceEnemiesForChainAttack();
	// ...
}
void UShadowStrikeComponent::SlowMotionEffectGame()
{

	// const float ElapsedTime = GetOwner()->GetWorldTimerManager().GetTimerElapsed(SlowInterpTimer);
	//		Get curve value corresponding to ElapsedTime
	// const float CurveValue = slowCurve->GetFloatValue(ElapsedTime);

	UGameplayStatics::SetGlobalTimeDilation(this, 0.3f);
}
void UShadowStrikeComponent::SlowMotionEffectOff()
{

	UGameplayStatics::SetGlobalTimeDilation(this, 1.0f);
}

void UShadowStrikeComponent::ZoomInCam(float Alpha)
{


	ToggleTargetIndicatorsInteface(currentTarget ,false);
	//SetActionState(EActionState::EAS_Aim);
	//ToggleWarithMaterial(8.0f);
	 Cast<AMyCharacterController>(GetOwner())->GetFollowCamera()->SetFieldOfView(Alpha);


}
void UShadowStrikeComponent::ToggleWarithMaterial(float Alpha)
{

    ACharacter->GetFollowCamera()->PostProcessSettings.bOverride_BloomIntensity = true;
        ACharacter->GetFollowCamera()->PostProcessSettings.BloomIntensity = Alpha;

	

}
void UShadowStrikeComponent::PerformTargetfromCenterOfScreen()
{

	if (!bAiming)
		return;

	FHitResult OuthitResult;
	if (CurrActionState == EActionState::EAS_Aim)
	{
		FVector2D ViewPortSize;
		if (GEngine && GEngine->GameViewport)
		{
			GEngine->GameViewport->GetViewportSize(ViewPortSize);
		}
		FVector2D CrossHairLocation(ViewPortSize.X / 2.0f, ViewPortSize.Y / 2.0f);
		FVector CrossHairWorldPosition;
		FVector CrosshairWorldDirection;

		bool BgetScreentoWorldPos = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), CrossHairLocation,
																			 CrossHairWorldPosition, CrosshairWorldDirection);

		if (BgetScreentoWorldPos)
		{

			const FVector Start{CrossHairWorldPosition};
			const FVector End{CrossHairWorldPosition + CrosshairWorldDirection * 50'000.f};
			// OutHitLocation = End;

			UKismetSystemLibrary::LineTraceSingleForObjects(this, Start, End, pawnObjectType, false,
												  ACharacter->myPlayer, EDrawDebugTrace::None, OuthitResult, true);
			// GetWorld()->LineTraceSingleByChannel(OuthitResult, Start, End, ECollisionChannel::ECC_Camera);
			// 			UKismetSystemLibrary::LineTraceSingle(
			// 	this,
			// 	Start,
			// 	End,
			// UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Camera) ,
			// 	false,
			// 	ACharacter->myPlayer,
			// 	EDrawDebugTrace::None,
			// 	OuthitResult ,
			// 	false        
			// );
			AActor *EnemyActor = OuthitResult.GetActor();
			strike = Cast<IStrikeInterface>(EnemyActor);

			if (EnemyActor && (!strike->IsDied()))
			{
			

				// UE_LOG(LogTemp , Warning , TEXT("we hit Enemy") );
				//  FString SoftTargetName = EnemyActor->GetName();
    			//UE_LOG(LogTemp, Warning, TEXT("Soft target game object name: %s"), *SoftTargetName);
				//AEnemyCharacter* enemytar = Cast<AEnemyCharacter>(EnemyActor);
				
				currentTarget  = EnemyActor;
				
				
						foundDirectionState = CheckforValidTelePortationLocation(currentTarget);
						ToggleTargetIndicatorsInteface(currentTarget ,true);
				
				}
			}else
			{		
				
				ToggleTargetIndicatorsInteface(currentTarget ,false);
				 currentTarget = nullptr; // No valid target
			}
		}
	}

// target will be Enmey
EFoundDirection UShadowStrikeComponent::CheckforValidTelePortationLocation(AActor *target)
{
	if(target == nullptr) return EFoundDirection::EFD_None;
	// const FVector currntForward = ACharacter->GetFollowCamera()->GetForwardVector();
	// float DotPro = FVector::DotProduct(target->GetActorForwardVector(),
	// 								   currntForward);
	EFoundDirection L_FoundDirection;

	   const FVector CurrentForward = ACharacter->GetFollowCamera()->GetForwardVector();
    float DotProduct = FVector::DotProduct(target->GetActorForwardVector(), CurrentForward);

	//UE_LOG(LogTemp , Warning , TEXT("Dot produect  %s") ,*FString::SanitizeFloat(DotProduct));			
	if (DotProduct < 0)
	{
		//SetFoundDirectionsState(EFoundDirection::EFD_Front);
		L_FoundDirection =  EFoundDirection::EFD_Front;
	}
	else
	{
		//SetFoundDirectionsState(EFoundDirection::EFD_back);

		L_FoundDirection  =EFoundDirection::EFD_back;
	}
	// /Do capsule trace from the direction found to see if player could teleport there?
	bool bDirectionisTraced =DirectionCapsuleTrace(target, L_FoundDirection);
	if(bDirectionisTraced)
	{
		//Eventhough capsule is not hit we have to check if there is a ground where player can stand at that location
		if(FloorTrace(CapsuleLocation))
		{
			return  L_FoundDirection;
		}

	}else //If first capsule trace+line trace to detect ground was found, if not try again for the next side if that was not valid just return NONE for direction
	{
		if(L_FoundDirection == EFoundDirection::EFD_Front)
		{
			//SetFoundDirectionsState(EFoundDirection::EFD_back);
			L_FoundDirection  =EFoundDirection::EFD_back;

		}else
		{
			//SetFoundDirectionsState(EFoundDirection::EFD_Front);
			L_FoundDirection  =EFoundDirection::EFD_Front;
			
		}
			bool bDirectionisTrac =DirectionCapsuleTrace(target, L_FoundDirection  );
		if(bDirectionisTrac)
			{
				if(FloorTrace(CapsuleLocation))
				{
					return  L_FoundDirection ;
				}
				
			}else
			{
		L_FoundDirection = EFoundDirection ::EFD_None;

			
		}
	}

	return  L_FoundDirection;
}

bool UShadowStrikeComponent::DirectionCapsuleTrace(AActor *Target, EFoundDirection Direction)
{
	if (Target == nullptr) return false;
	
		FHitResult hitResult;
	
				const FVector StartLocation = Target->GetActorLocation();
			const FVector ForwardVector = Target->GetActorForwardVector();
			const float TraceDistance = ReturnStateValue(Direction);

			const FVector CapsuleStart = StartLocation + ForwardVector * TraceDistance;
			 CapsuleLocation = CapsuleStart;
			const FVector CapsuleEnd = CapsuleStart + (ForwardVector); // Adjust the distance if needed

			
		UKismetSystemLibrary::CapsuleTraceSingle(this, CapsuleStart, CapsuleEnd, 42.0f, 96.0f,
												UEngineTypes::ConvertToTraceType(ECollisionChannel:: ECC_Camera), false, ACharacter->myPlayer, EDrawDebugTrace::None,
												hitResult, true , FLinearColor::Black, FLinearColor::Green, 4.0f);
							
		if (!hitResult.bBlockingHit)
		{		
			return true;
		}

	return false;

	//return hitResult;
}

bool UShadowStrikeComponent::FloorTrace(FVector capsuleLocation)
{
	FHitResult OuthitResult;
	const FVector MakeVectorEnd(0.0f, 0.0f, -150.0f);

	
	UKismetSystemLibrary::LineTraceSingle(this, capsuleLocation, MakeVectorEnd,UEngineTypes::ConvertToTraceType(ECollisionChannel:: ECC_Visibility), false,
										  ACharacter->myPlayer, EDrawDebugTrace::None, OuthitResult, true);
	if (OuthitResult.bBlockingHit)
	{		
		return true;
	}
	return  false;
}


//Once the strike happen, start multi sphere trace to detect possible targets around the player
void UShadowStrikeComponent::TraceEnemiesForChainAttack()
{
	if(CurrActionState == EActionState::EAS_Striking)
	{

	
		TArray<FHitResult> hitResult;
		const FVector Start =  ACharacter->GetActorLocation() ;
		UKismetSystemLibrary::SphereTraceMultiForObjects(this,Start , Start - 100.0f, 
	2200.0f , pawnObjectType , false ,ACharacter->myPlayer, EDrawDebugTrace::None  , hitResult  ,
	 true, FLinearColor::Red, FLinearColor::Green, 5.f);

	 for(const FHitResult& hitResults:hitResult)
	{
		AActor* HitActor = hitResults.GetActor();
		if(HitActor)
		{
		
		strike = Cast<IStrikeInterface>(HitActor);
		if(strike)
		{
			if(!strike->IsDied())
			{
				if( !ValidChainStrikeActors.Contains(HitActor)) 
				{
				ValidChainStrikeActors.AddUnique(HitActor);	}
				}
			}
		}
		//AEnemyCharacter* Enemy =  Cast<AEnemyCharacter>(HitActor);
		
	}
	ChainStrikeTarget = SelectNextTarget();
}
}

AActor* UShadowStrikeComponent::SelectNextTarget()
{

	TArray<AActor*>  LFoundActor;

	for( AActor* ValidChainStrikeActor:ValidChainStrikeActors)
	{
			//CheckforlineSight(ValidChainStrikeActor , "Head");

	    const FVector CurrentForwardVector = ACharacter->GetFollowCamera()->GetForwardVector();
        FVector NormalizedCameraLocation = (ValidChainStrikeActor->GetActorLocation() - ACharacter->GetFollowCamera()->GetComponentLocation()).GetSafeNormal(0.0001f);


	    float DotProduct = FVector::DotProduct(CurrentForwardVector, NormalizedCameraLocation);

	float AngleInRadians = UKismetMathLibrary::Acos(DotProduct);

		float AngleInDegrees = FMath::RadiansToDegrees(AngleInRadians);
	
		if(AngleInDegrees < 30.0f &&  CheckforlineSight(ValidChainStrikeActor, "head"))
		{
			LFoundActor.AddUnique(ValidChainStrikeActor);
			//ToggleTargetIndicatorsInteface(ValidChainStrikeActor,true);
		}else
		{
			//off the target
			ToggleTargetIndicatorsInteface(ValidChainStrikeActor ,false);
			LFoundActor.Remove(ValidChainStrikeActor);
		}
	}

	return FilterActorbasedOnDistance(LFoundActor);
}
//Check if the target is in 1. Within vision range 2.the line of sight to that character is not blocked then add to a actor array
bool UShadowStrikeComponent::CheckforlineSight(AActor* Target , FName SocketName)
{
	if(Target == nullptr) return false;

	 FHitResult	OuthitResult; 
	  USkeletalMeshComponent* TargetMesh = Target->FindComponentByClass<USkeletalMeshComponent>();
	 if (TargetMesh){
          //  FVector SocketLocation = FVector::ZeroVector;
		  FVector SocketLocation = TargetMesh->GetSocketLocation(SocketName);

		
        FVector Start = ACharacter->GetFollowCamera()->GetComponentLocation();
    FVector Direction = (SocketLocation - Start).GetSafeNormal(); // Direction from camera to socket

		 FVector End =  (Direction * 1500.0f) + Start; 
       // FVector End = Start  + (SocketLocation - Start).GetSafeNormal(0.0001f) * 10000.0f; 

		//GetWorld()->LineTraceSingleByChannel(OuthitResult , Start , End , ECollisionChannel::ECC_Visibility , 
	
		 UKismetSystemLibrary::LineTraceSingle(
        this,
        Start,
        End,
   	 UEngineTypes::ConvertToTraceType(ECollisionChannel:: ECC_Visibility) ,
        false,
         ACharacter->myPlayer,
        EDrawDebugTrace::None,
        OuthitResult ,
        false        
    );

		return  !OuthitResult.bBlockingHit;
		// if(!OuthitResult.bBlockingHit)
		// {
		// 	return true;
		// }
 }

	return false;
}
//We have to also get the target based on the closest distance to player
AActor* UShadowStrikeComponent::FilterActorbasedOnDistance(TArray<AActor*>  LFoundActors)
{	
	AActor* LCurrntActor ;
	for( AActor* foundActor:LFoundActors)
	{
		
    float CurrentDistance = 1000.0f;
	 		LCurrntActor = foundActor;
			const FVector fa = foundActor->GetActorLocation();
			const FVector thisAc = ACharacter->GetActorLocation();
		  float LDistance ;//  = FVector::DistSquared(fa, thisAc);
			float lenth = ( foundActor->GetActorLocation() - ACharacter->GetActorLocation()).Size();
			
			LDistance = lenth;
		 if (LDistance < CurrentDistance)
            {
                CurrentDistance = LDistance;
               if( UpdatedActor != LCurrntActor )
			   {
						UpdatedActor = LCurrntActor ;
							ToggleTargetIndicatorsInteface(LCurrntActor ,true);
				// if(UpdatedActor != nullptr)
				
				// {
				// 		UE_LOG(LogTemp , Warning , TEXT("this work INdicator valid  hai "));
				// 	ToggleTargetIndicatorsInteface(UpdatedActor ,false);
				// 	//UpdatedActor =foundActor ;// LCurrntActor ;
				// }else
				// {
				// 	UpdatedActor = LCurrntActor ;
				// 	UE_LOG(LogTemp , Warning , TEXT("Toggle valdi nhi hai "));
				// 	ToggleTargetIndicatorsInteface(LCurrntActor ,true);
				// }
			   }
            }
	}
	//Return the most appropriate target 
	if(LFoundActors.Num() > 0)
	{
		return UpdatedActor;
	}

	return nullptr;
}

void UShadowStrikeComponent::TeleportTarget()
{	
	//SetActionState(EActionState::EAS_Teleporting);
	//Calculate the teleport position based on animation data and the found direction
	if(currentTarget && ACharacter){

	if(StrikeSound){
	UGameplayStatics::PlaySound2D(this ,StrikeSound );
	}
		UGameplayStatics::SetGlobalTimeDilation(this, 1.0f);

		bXpressed = true;
	 const FVector GetForwrdvecStart = currentTarget->GetActorForwardVector() * ReturnStateValue(foundDirectionState);
	const FVector teleportTarget = GetForwrdvecStart + currentTarget->GetActorLocation()  ;
	    FLatentActionInfo LatentInfo;
    LatentInfo.CallbackTarget = this;
	LatentInfo.Linkage = 0;
    LatentInfo.UUID = 0; 
	LatentInfo.ExecutionFunction = FName(TEXT("MoveToTargetFinished"));
	
		ACharacter->SetVisibilityMesh(false);

		//ACharacter->SetCameraLagboom(0.1f);
       
        UKismetSystemLibrary::MoveComponentTo(
            Cast<AMyCharacterController>(GetOwner())->GetCapsuleComponent(),
            teleportTarget,
        FRotator(0.0f , 0.0f , 0.0f),
            false,
            false,
            0.2f,
            false,
            EMoveComponentAction::Move,
            LatentInfo
        );
        SetActionState(EActionState::EAS_Striking);

	//	FRotator TeleportRotation = UKismetMathLibrary::FindLookAtRotation( ACharacter->GetOwner()->GetRootComponent()->GetComponentLocation(), teleportTarget);

    // Set the player's rotation to face the target
    // ACharacter->GetOwner()->GetRootComponent()->SetWorldRotation(TeleportRotation);
		FinishedEnemyPlay(currentTarget , foundDirectionState);
 
}
}
void UShadowStrikeComponent::MoveToTargetFinished()
{

	if(currentTarget == nullptr) return;
	
	ACharacter->SetVisibilityMesh(true);

		ACharacter->ChangeCameraLagboomEvent();
	
	      FRotator targetRot =  UKismetMathLibrary::FindLookAtRotation(ACharacter->GetActorLocation() , currentTarget->GetActorLocation() );
			//FRotator MakeRotator(targetRot.Roll ,GetActorRotation().Pitch , targetRot.Yaw );
		
		// FRotator smmothRot = UKismetMathLibrary::RLerp(ACharacter->GetActorRotation(), MakeRotator , 0.3f , false);
		 Cast<AMyCharacterController>(GetOwner())->GetCapsuleComponent()->SetWorldRotation(targetRot);
        
		if(teleportSystem){

	 UNiagaraFunctionLibrary::SpawnSystemAtLocation(this , teleportSystem , ACharacter->GetActorLocation() ,
	UKismetMathLibrary::Conv_VectorToRotator(ACharacter->GetActorForwardVector()) ,FVector(0.5f , 0.5f , 0.5f)  , true , true , ENCPoolMethod::None , true);
	}
}

void UShadowStrikeComponent::StartInitialStrike()
{
	//if(!bXpressed)  return;
	if(currentTarget)
	{
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		//PlayerController->ClientStartCameraShake()
		//StartChainShadowStrike();
		switch (CurrActionState)
	{
	case EActionState::EAS_Idle:

		break;
	case EActionState::EAS_Aim:
		TeleportTarget();
		CurrActionState = EActionState::EAS_Striking;

		break;
	case EActionState::EAS_Teleporting:
		SetActionState(EActionState::EAS_Teleporting);
		break;
		case EActionState::EAS_Striking:
			// /ACharacter->DisableInput(PlayerController);
		bXpressed = false;
		//After intial shadow strike next chained one goes here- It also check for a valid location
		if(currentTarget){
		UpdatedActor = nullptr;
		currentTarget = nullptr;
		}
		if(ChainStrikeTarget)
		{
			currentTarget = ChainStrikeTarget;
			
		//TraceEnemiesForChainAttack();
		foundDirectionState = CheckforValidTelePortationLocation(currentTarget);
			StartChainShadowStrike();

		}
		
		TeleportTarget();
		break;
	default:
		// Handle default case if needed
		break;
	}
	}
}
void UShadowStrikeComponent::StartChainShadowStrike()
{
	if(CurrActionState == EActionState::EAS_Striking && currentTarget)
	{
			ValidChainStrikeActors.Empty();
				//SetActionState(EActionState::EAS_Idle);
			ToggleTargetIndicatorsInteface(currentTarget ,false);
			//off teh C buttin in enemy
		currentTarget = ChainStrikeTarget;


	}
}

void UShadowStrikeComponent::OnChainTimerExpired()
{


	for( AActor* ValidChainStrikeActor:ValidChainStrikeActors)
	{
	
		ToggleTargetIndicatorsInteface(ValidChainStrikeActor  , false);
	}
	ValidChainStrikeActors.Empty();
	if(CurrActionState == EActionState::EAS_Striking)
	{
		ChainStrikeTarget = nullptr;

	UpdatedActor = nullptr;
	currentTarget = nullptr;
	SetActionState(EActionState::EAS_Idle);
	bAiming = false;
	//UE_LOG(LogTemp ,Warning , TEXT("YE chain Time Expred hua hai"));
	//off teh effect
	}
}
float UShadowStrikeComponent::ReturnStateValue(EFoundDirection Direction)
{
	float dists = 0.0f;

	switch (Direction)
	{
	case EFoundDirection::EFD_None:
		dists = 0.0f;
		break;
	case EFoundDirection::EFD_Front:
		dists = 120.0f;
		break;
	case EFoundDirection::EFD_back:
		dists = -80.0f;
		break;
	default:
		// Handle default case if needed
		break;
	}
	return dists;
}

void UShadowStrikeComponent::ToggleTargetIndicatorsInteface(AActor* Target ,bool show)
{
	strike = Cast<IStrikeInterface>(Target);
						
		if(strike)
			strike->ToggleTargetIndicator(show);
		// /currentTarget = nullptr;
}

void UShadowStrikeComponent::SmoothRotateToTarget(AActor* Target)
{

	  if (Target)
    {
        //FVector TargetDirection = TargetActor->GetActorLocation() - GetActorLocation();
        //FRotator TargetRotation = TargetDirection.Rotation();
	 	
        // Use Lerp to interpolate towards the target rotation
        //FRotator NewRotation = FMath::Lerp(GetActorRotation(), TargetRotation, 100 * DeltaTime);

		//FVector	 slectVec = UKismetMathLibrary::SelectVector(, SoftTarget->GetActorLocation() , TargetActor);
			FRotator targetRot =  UKismetMathLibrary::FindLookAtRotation(ACharacter->GetActorLocation() , Target->GetActorLocation() );
			//FRotator MakeRotator(targetRot.Roll ,GetActorRotation().Pitch , targetRot.Yaw );
		FRotator MakeRotator(ACharacter->GetActorRotation().Pitch, targetRot.Yaw, ACharacter->GetActorRotation().Roll);
		 FRotator smmothRot = UKismetMathLibrary::RLerp(ACharacter->GetActorRotation(), MakeRotator , 0.3f , false);

        ACharacter->SetActorRotation(smmothRot);
	}
}

void UShadowStrikeComponent::DirectRotateToTarget(AActor* Target)
{
	 if (Target && GetOwner() && Cast<AMyCharacterController>(GetOwner()))
    {
        FRotator targetRot =  UKismetMathLibrary::FindLookAtRotation(ACharacter->GetActorLocation() , Target->GetActorLocation() );
			//FRotator MakeRotator(targetRot.Roll ,GetActorRotation().Pitch , targetRot.Yaw );
		FRotator MakeRotator(ACharacter->GetActorRotation().Pitch, targetRot.Yaw, ACharacter->GetActorRotation().Roll);
		// FRotator smmothRot = UKismetMathLibrary::RLerp(ACharacter->GetActorRotation(), MakeRotator , 0.3f , false);

        ACharacter->SetActorRotation(MakeRotator);
    }
   
}

void  UShadowStrikeComponent::FinishedEnemyPlay(AActor* target ,  EFoundDirection Directions)
{
	CurrActionState = EActionState::EAS_Striking;
	SetActionState(EActionState::EAS_Striking);
		 int32 rand  = FMath::RandRange(0 , 3);
		 FName Sections("");
		FName ExecutionName("");
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
			case 3:
			Sections =FName("Tar4") ;
			ExecutionName = FName("Exe4");
			break;
				
			}

	switch (Directions)
	{
	case EFoundDirection::EFD_None:
	
		break;
	case EFoundDirection::EFD_Front:
			if (ACharacter->AnimInstance && finishingAttackMontageFront){
			
	//	AnimInstance->Montage_Play(finishingMontage[rand].TargetMontage);
		ACharacter->AnimInstance ->Montage_Play(finishingAttackMontageFront);
	    ACharacter->AnimInstance ->Montage_JumpToSection(FName(Sections));

		strike = Cast<IStrikeInterface>(target);
						
		if(strike)
			strike->PlayCharacterAnimMontage(VicTinAttackMontageFront , ExecutionName);
		}	
		break;
	case EFoundDirection::EFD_back:
	

		if (ACharacter->AnimInstance && finishingAttackMontageBack){
			
	//	AnimInstance->Montage_Play(finishingMontage[rand].TargetMontage);
		ACharacter->AnimInstance ->Montage_Play(finishingAttackMontageBack);
	    ACharacter->AnimInstance ->Montage_JumpToSection(FName(Sections));

		strike = Cast<IStrikeInterface>(target);
						
		if(strike)
			strike->PlayCharacterAnimMontage(VicTinAttackMontageBack , ExecutionName);
		}	
		break;
	default:
		// Handle default case if needed
		break;
	}

}

void UShadowStrikeComponent::AnimMontagePlayShadow( UAnimMontage* AttackMontageBack )
{
		
}

void UShadowStrikeComponent::PerformDash()
{
    if (bCanDash)
    {
        // Get the player's forward direction or any desired dash direction
        DashDirection = ACharacter->GetActorForwardVector();
		ACharacter->SetVisibilityMesh(false);
		BlinkDashFX();
        // Disable dashing until the cooldown expires
			if(DashSoundCue){
				UGameplayStatics::PlaySound2D(this ,DashSoundCue );
		}
        // Apply dash effects (you can implement your visual effects here)
        //ApplyDashEffects();
        // Move the character by the dash distance in the specified direction
       ACharacter->LaunchCharacter(DashDirection * DashDistance * DashSpeedMultiplier, true, true);

        // Start the cooldown timer
        ACharacter->GetWorldTimerManager().SetTimer(
            CooldownTimerHandle,
            this,
            &UShadowStrikeComponent::EndDashEffect,
            DashCooldown,
            false
        );

        bCanDash = false;
		//SetVisibilityMesh(true);
    }
}

void UShadowStrikeComponent::EndDashEffect()
{
		
		ACharacter->SetVisibilityMesh(true);
    bCanDash = true;
}


void UShadowStrikeComponent::BlinkDashFX()
{
	if(dashTrailSystem){

	 UNiagaraFunctionLibrary::SpawnSystemAtLocation(this , dashTrailSystem , ACharacter->GetCharacterMovement()->GetLastUpdateLocation() ,
	UKismetMathLibrary::Conv_VectorToRotator(ACharacter->GetActorForwardVector()) ,FVector(1.0f , 1.0f , 1.0f)  , true , true , ENCPoolMethod::None , true);
	}
 
}



