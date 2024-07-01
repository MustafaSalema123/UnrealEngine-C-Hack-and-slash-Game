// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacterController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnemyCharacter.h"
#include "Sound/SoundCue.h"
#include "ShadowStrikeComponent.h"

// Sets default values
AMyCharacterController::AMyCharacterController()

{
	
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(FName("Camera Boom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	CameraFollow = CreateDefaultSubobject<UCameraComponent>(FName("Text Camera"));
	CameraFollow->SetupAttachment(CameraBoom ,  USpringArmComponent::SocketName);
	CameraFollow->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	weaponKatana =  CreateDefaultSubobject<UStaticMeshComponent>(FName("Weapon"));
	   //ShadowStrikeCompo = CreateDefaultSubobject<UShadowStrikeComponent>(TEXT("ShadowStrikeComponenet"));
	//ShadowStrikeCompo = Cast<UShadowStrikeComponent>(GetCharacterMovement());

	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 400.f;

	bShouldSaveLightAttack = false;
	bShouldSaveHeavyAttack = false;




}
void AMyCharacterController::SpawnBloodPartcle()
{
	if(BloodHitParticles){

		const FVector EndSocket = weaponKatana->GetSocketLocation(FName("Mid"));
		 UGameplayStatics::SpawnEmitterAtLocation(this , BloodHitParticles , EndSocket);  
	//UGameplayStatics::SpawnEmitterAtLocation(this , BloodHitParticles , EndSocket);
		}
}
// Called when the game starts or when spawned
void AMyCharacterController::BeginPlay()
{
	Super::BeginPlay();
	AnimInstance = GetMesh()->GetAnimInstance();
	weaponKatana->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale , FName("WeaponSocket"));
	myPlayer.Add(this);

	// if(LaunchCurve)
	// {
	// FOnTimelineFloat TimelineProgress;
	// TimelineProgress.BindUFunction(this , FName("LaunchCharacterTimeline"));
	//  CurveTimeline.AddInterpFloat(LaunchCurve, TimelineProgress);

	// }

	ShadowStrikeCompo = FindComponentByClass<UShadowStrikeComponent>();
}
 void AMyCharacterController::AddControllerPitchInput(float Val) 
 {	
	if(!TargetActor)
		Super::AddControllerPitchInput(Val);
 }
void AMyCharacterController::AddControllerYawInput(float Val)
{
	if(!TargetActor)
		Super::AddControllerYawInput(Val);
}

void AMyCharacterController::MoveRight(float Value)
{
	//if(currentState != ECombatState::ECS_Nothing) return;
		if(currentState == ECombatState::ECS_Attack) return;
if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AMyCharacterController::MoveForward(float Value)
{
	//if(currentState != ECombatState::ECS_Nothing) return;
	if(currentState == ECombatState::ECS_Attack) return;
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMyCharacterController::TurnAtRate(float Rate)
{

		// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	
}

void AMyCharacterController::LookUpAtRate(float Rate)
{
		
		// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	
}

// Called every frame
void AMyCharacterController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	WeaponSphereTracing();

	LookAtEnemy();
	//CurveTimeline.TickTimeline(DeltaTime);
	//RotateTotarget(DeltaTime);
}

void AMyCharacterController::BufferTimer(float Value)
{
	const FVector Start = GetActorLocation();
	const FVector forward =Start +  GetActorForwardVector() * BufferAmount;


	SetActorLocation(FMath::Lerp(Start, forward, Value ), true);
}
bool AMyCharacterController::CanAttack()
{
	return !GetCharacterMovement()->IsFalling() && !GetCharacterMovement()->IsFlying();//  && currentState == ECombatState::ECS_Attack  || currentState == ECombatState::ECS_Dodge;
}
void AMyCharacterController::DoLightAttack()
{

	if(currentState == ECombatState::ECS_Attack && bShouldSaveLightAttack) return;

	bShouldSaveHeavyAttack = false;
	bLightIputHead = true;

		if(CanAttack()){
			
		//	if(!CanLaunchPlayer()){
						StopBuffer();
					
					BufferAmount = 2.0f;
					StartBuffer();
					RotateTotargetEvent();
					SoftOntarget();
					PerformCombatAttacks(LightAttackMontage  , 4, FString("l_Attack"));
		 			
					bShouldSaveLightAttack = true;

								if(SwingSound){
	UGameplayStatics::PlaySound2D(this ,SwingSound );
	}
			//}
			}
			else
			{
				if(bLaunched){
					RotateTotargetEvent();
				// &&  currentState == ECombatState::ECS_Attack  || currentState == ECombatState::ECS_Dodge){
				GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
				DoAirAttack();
				}
			}
        
    
}
void AMyCharacterController::DoLightAttackRelesead()
{
bLightIputHead = false;
}
void AMyCharacterController::DoHeavyAttack()
{
	if(currentState == ECombatState::ECS_Attack || bShouldSaveHeavyAttack ) return;
	
	

	// {
		if(CanLaunchPlayer())
		{
		SetCombatStatus(ECombatState::ECS_Attack);
		RotateTotargetEvent();
		 PlayMontages(lauchCharMontage);	
		}else
		{
			if(!bLaunched){
			//SoftOntarget();
			RotateTotargetEvent();
	
										if(SwingSound){
	UGameplayStatics::PlaySound2D(this ,SwingSound );
	}
			PerformCombatAttacks(HeavyAttackMontage , 4, FString("h_Attack"));
			bShouldSaveHeavyAttack = true;
			}
		}
	// }
	// else
	// {
	// 	CrashedSmackDown();
	// }
        

}
void AMyCharacterController::DoAirAttack()
{
	
	//PerformCombatAttacks(AirAttackMontage ,  6 , );
	  if (AirAttackMontage  == nullptr|| AnimInstance == nullptr)  return;
             if (AirAttackIndexes < 6)
             {

				RotateTotargetEvent();
				  							if(SwingSound){
	UGameplayStatics::PlaySound2D(this ,SwingSound );
	}
				//RotateTotargetEvent();
				FString name = FString("a_Attack") + FString::FromInt(AirAttackIndexes + 1);
                // Play the next animation in the montage
                 //FString SectionNameAir = FString::Printf(TEXT("l_Attack%d"), AirAttackIndex + 1); // Assuming section names are Attack1, Attack2, etc.
	
                // Play the specific section of the montage
				AnimInstance->Montage_Play(AirAttackMontage, 1.f);
              //  AnimInstance->Montage_Play(LightAttackMontage, 1.f);
                AnimInstance->Montage_JumpToSection(FName(name));

              AirAttackIndexes++;
			

			//  UE_LOG(LogTemp, Warning, TEXT("Do name %d") ,AirAttackIndexes );
                if (AirAttackIndexes >= 6)
                {
					 //AirAttackIndexes = 0;
					 //Add in the notify in the AIr combat animation last anim
					CrashedSmackDown();
					   //UE_LOG(LogTemp, Warning, TEXT("aor combat Complete ") );
					 bLaunched = false;

                }
				bShouldSaveAirAttack = true;
				SetCombatStatus(ECombatState::ECS_Attack);
			 }
}
    
void AMyCharacterController::PerformCombatAttacks(UAnimMontage* montage ,int32 maxAttack , FString SectionName)
{
	  if (montage  == nullptr|| AnimInstance == nullptr)  return;
            //  if (AttacksIndex < maxAttack)
            // {
				
				FString name = SectionName + FString::FromInt(AttackIndexes + 1);
            
	
                // Play the specific section of the montage
				AnimInstance->Montage_Play(montage, 1.f);
              //  AnimInstance->Montage_Play(LightAttackMontage, 1.f);
                AnimInstance->Montage_JumpToSection(FName(name));

              AttackIndexes++;
		

				//    UE_LOG(LogTemp, Warning, TEXT("Do name %d") ,AttacksIndex );
                if (AttackIndexes >= maxAttack)
                {
					 AttackIndexes = 0;
					 bLaunched = false;

                }
				SetCombatStatus(ECombatState::ECS_Attack);
 }
        
    


void AMyCharacterController::ResetAirCombatAttack()
{
	AirAttackIndexes = 0;
	bLaunched = false;

}

void AMyCharacterController::SaveLightAttack()
{
	
	if(bShouldSaveLightAttack && currentState == ECombatState::ECS_Attack)
	{
		currentState = ECombatState::ECS_Nothing;
		bShouldSaveLightAttack = false;
	}
}
void AMyCharacterController::ResetStatesAttack()
{

	if(GetCharacterMovement()->IsFlying())
	{
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
	}
		bShouldSaveLightAttack = false;
		currentState = ECombatState::ECS_Nothing;
		AttackIndexes = 0;
		TargetActor = nullptr;
		bTargeting = false;
		ResetAirCombatAttack();
		//ECombatState::ECS_Fininshe;
		if(GetCharacterMovement()->IsFlying())
		{
			CrashedSmackDown();
		}
	//	CrashedSmackDown();
	
}

void AMyCharacterController::SaveHeavyAttack()
{
	
	if(bShouldSaveHeavyAttack && currentState == ECombatState::ECS_Attack)
	{
		bShouldSaveHeavyAttack = false;
		currentState = ECombatState::ECS_Nothing;
	}
}


void AMyCharacterController::DoDodge()
{
	if(currentState == ECombatState::ECS_Dodge || bShouldSaveDodge ) return;

	StopBuffer();
		RotationForDodge();
		BufferAmount = 10.0f;
		StartBuffer();
			PlayMontages(dodgeMontage);
		bShouldSaveDodge = true;
			SetCombatStatus(ECombatState::ECS_Dodge);
		
	
}

void AMyCharacterController::SaveDodgeMon()
{

	if(bShouldSaveDodge && currentState == ECombatState::ECS_Dodge){
	bShouldSaveDodge = false;
	currentState = ECombatState::ECS_Nothing ;
	}
}

void AMyCharacterController::RotationForDodge()
{
	if(GetCharacterMovement()->GetLastInputVector() == FVector::ZeroVector){}
	{

		const  FRotator newRotX =  UKismetMathLibrary::MakeRotFromX(GetCharacterMovement()->GetLastInputVector());
		SetActorRotation(newRotX );
	}
}

void AMyCharacterController::WeaponSphereTracing()
{

if(!bActiveCollision)  return;

	

	TArray<FHitResult> hitResult;


	const FVector StartSocket = weaponKatana->GetSocketLocation(FName("Start"));
	const FVector EndSocket = weaponKatana->GetSocketLocation(FName("End"));
	UKismetSystemLibrary::SphereTraceMultiForObjects(this, StartSocket , EndSocket , 
	20.0f , collsionSurfaceTraceTypes , false ,myPlayer, EDrawDebugTrace::None  , hitResult  , true, FLinearColor::Red, FLinearColor::Green, 5.f);
	//UKismetSystemLibrary::SphereTraceSingle()

	//if(hitResult.Num() == 0) return;

	for(const FHitResult& hitResults:hitResult)
	{
		AActor* HitActor = hitResults.GetActor();
		if(HitActor)
		{
		
		
		AEnemyCharacter* Enemy =  Cast<AEnemyCharacter>(HitActor);
		//HitActor->GetClass() ==
		if( Enemy && !AlredyhitActor.Contains(HitActor)) 
		{
			// if(AlredyhitActor.Num() > 0)
			// {
				AlredyhitActor.AddUnique(HitActor);
				 
			//	 hitPoint =  hitResults.ImpactPoint;
				 
				UGameplayStatics::ApplyDamage(Enemy , 20 , GetInstigatorController(), this, DamageTypeClass);
			
		
		}
	}


}
}

void AMyCharacterController::StartWeaponCollision()
{
	
	AlredyhitActor.Empty();
	bActiveCollision = true;
}

void AMyCharacterController::EndWeaponCollision()
{
	bActiveCollision = false;
}

bool AMyCharacterController::CanLaunchPlayer()
{
	if(!bTargeting) return false;
	
	 if(GetActorForwardVector().Y <= -0.7)
		 {
			return true;
		}

   return false;
}
//This Funtion isn calling in AttackUpNotify 
void AMyCharacterController::launchCharacterMy()
{
	//SetCombatStatus(ECombatState::ECS_Attack);

		RaiseLaunch();
	//}
   // CurveTimeline.PlayFromStart();
}
// void AMyCharacterController::LaunchCharacterTimeline(float Alpha)
// {

// 	// 	 UE_LOG(LogTemp, Warning, TEXT("Do anims "));
// 	// const FVector DesiredLocation = GetActorLocation();
// 	// const FVector launchDirection = FVector(DesiredLocation.X, DesiredLocation.Y , DesiredLocation.Z + 100.0f); 
// 	// // =  FMath::VInterpTo(DesiredLocation, DesiredLocation + launchDirection , Value);
// 	// FVector NewLocation  = UKismetMathLibrary::VLerp(DesiredLocation,launchDirection, Alpha);

// 	// SetActorLocation(NewLocation , false);
//     DesiredLocation = GetActorLocation();
//     const FVector LaunchDirection = FVector(0.0f, 0.0f, 100.0f); // Modify this direction as needed

//     const FVector NewLocation = FMath::Lerp(DesiredLocation, DesiredLocation + LaunchDirection, Alpha);

// 	  SetActorLocation(NewLocation  * Alpha);
//   //  SetActorLocation(NewLocation, true);

// }

void AMyCharacterController::DoAnimationWith()
{

	SetCombatStatus(ECombatState::ECS_Attack);
	PlayMontages(lauchCharMontage);

}

void AMyCharacterController::CrashedSmackDown()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	PlayMontages(GroundSlamMontage);
		SetCombatStatus(ECombatState::ECS_Attack);
	FHitResult hitResult;
	AirAttackIndexes = 0;
	
//We want to tarce for teh ground and tehn move our player down until wwe touch
	UKismetSystemLibrary::LineTraceSingleForObjects(
		this,
		GetActorLocation(),
		FVector(GetActorLocation().X, GetActorLocation().Y , GetActorLocation().Z - 100000.0f ),
		collsionDown ,false , myPlayer ,
		 EDrawDebugTrace::None ,
		  hitResult ,true);

		AActor* groundMesh = hitResult.GetActor();
		  if(groundMesh )
		  {
				SetCombatStatus(ECombatState::ECS_Nothing);
			GroundLocation = hitResult.ImpactPoint;
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
		//	DownPlayer();
		  }
	

}
void AMyCharacterController::EventNotifyGroundSlam()
{
	//UE_LOG(LogTemp, Warning, TEXT("Crcu Dowbn Is Wirking") );
	DownPlayer();
}
void AMyCharacterController::DowncharacterTimeline(float Alpha)
{

	SetActorLocation(UKismetMathLibrary::VLerp(GetActorLocation(), GroundLocation, Alpha) , true);
}
void AMyCharacterController::BufferStart()
{
}
void AMyCharacterController::PlayMontages( UAnimMontage* montage , FName Section )
{
		if(AnimInstance)
		{
			AnimInstance->Montage_Play(montage, 1.f);
			AnimInstance->Montage_JumpToSection(Section);
			
		}
}
void AMyCharacterController::Jump()
{

	if(currentState != ECombatState::ECS_Nothing) return;

	if(bHasDoubleJumped || !GetCharacterMovement()->IsFalling())
	{
		bHasDoubleJumped = false;
		Super::Jump();
	}else
	{
		bHasDoubleJumped = true;
		//Super::Jump();
		PlayMontages(doubleJumpMontage);
		//  if(AnimInstance)
		//  {
		// 		AnimInstance->Montage_Play(doubleJumpMontage, 1.f);
		//  }
   
		LaunchCharacter(FVector::UpVector * 600.f , false , true);

	}
}
void AMyCharacterController::TabButtonPressed()
{
	if(TargetActor || bTargeting) return;

	FHitResult hitResult;

	const FVector StartL = GetActorLocation();
	const FVector End =StartL +  CameraFollow->GetForwardVector() * 400.0f;
	UKismetSystemLibrary::SphereTraceSingleForObjects(this, StartL , End , 
	70.0f , collsionToPawn , false ,myPlayer, EDrawDebugTrace::None  , hitResult  , true, FLinearColor::Red, FLinearColor::Green, 5.f);

	AActor* getActor  =	hitResult.GetActor();
	if(getActor)
	{
		bTargeting = true;
		TargetActor = getActor;
	}

}

void  AMyCharacterController::LookAtEnemy()
{
	if(TargetActor && bTargeting)
	 {

		if(GetDistanceTo(TargetActor) <= 600.0f)
		{

		FVector offset  = FVector(TargetActor->GetActorLocation().X , TargetActor->GetActorLocation().Y , TargetActor->GetActorLocation().Z + 60.0f);
		//z is go to -30  ;
		FRotator targetRot =  UKismetMathLibrary::FindLookAtRotation(GetActorLocation() , offset);
		  FRotator lerpRotate = 	UKismetMathLibrary::RInterpTo(GetController()->GetPawn()->GetActorRotation() ,targetRot  , 0.0f , GetWorld()->GetDeltaSeconds() );
			GetController()->SetControlRotation(lerpRotate);
		
		
		}else
		{
			bTargeting = false;
			TargetActor = nullptr;
		}
	 }
}
void AMyCharacterController::SoftOntarget()
{
//bTargeting &&

	if( TargetActor == nullptr  && bTargeting == false){

	   
	FHitResult hitResult;
	
	const FVector StartL = GetActorLocation();
	const FVector End = StartL +  GetCharacterMovement()->GetLastInputVector() * 800.0f;

	UKismetSystemLibrary::SphereTraceSingleForObjects(this, StartL , End , 
	70.0f , collsionToPawn , false ,myPlayer, EDrawDebugTrace::None  , hitResult  , true, FLinearColor::Red, FLinearColor::Green, 5.f);
	
	
	SoftTarget = hitResult.GetActor();
	bTargeting = true;
	}
		

		
					
	
}
void AMyCharacterController::RotateTotarget(float Value)
{	
	

	FVector ActorLocation = GetActorLocation();
	
	   if (TargetActor)
    {
        //FVector TargetDirection = TargetActor->GetActorLocation() - GetActorLocation();
        //FRotator TargetRotation = TargetDirection.Rotation();
	 	
        // Use Lerp to interpolate towards the target rotation
        //FRotator NewRotation = FMath::Lerp(GetActorRotation(), TargetRotation, 100 * DeltaTime);

		//FVector	 slectVec = UKismetMathLibrary::SelectVector(, SoftTarget->GetActorLocation() , TargetActor);
			FRotator targetRot =  UKismetMathLibrary::FindLookAtRotation(GetActorLocation() , TargetActor->GetActorLocation() );
			//FRotator MakeRotator(targetRot.Roll ,GetActorRotation().Pitch , targetRot.Yaw );
		FRotator MakeRotator(GetActorRotation().Pitch, targetRot.Yaw, GetActorRotation().Roll);
		 FRotator smmothRot = UKismetMathLibrary::RLerp(GetActorRotation(), MakeRotator , Value , false);

        SetActorRotation(smmothRot);
	//	SmoothRotate(TargetActor)	;

    }else if ( SoftTarget && TargetActor == nullptr) 
	{

		FVector SoftTargetLocation = SoftTarget->GetActorLocation();
		FRotator SofttargetRot = UKismetMathLibrary::FindLookAtRotation(ActorLocation, SoftTargetLocation);
		// Get the name of the soft target game object
		// FString SoftTargetName = SoftTarget->GetName();
		// UE_LOG(LogTemp, Warning, TEXT("Soft target game object name: %s"), *SoftTargetName);
		FRotator MakeRotator(GetActorRotation().Pitch, SofttargetRot.Yaw, GetActorRotation().Roll);
		FRotator SmoothRot = FMath::Lerp(GetActorRotation(), MakeRotator, Value);
		SetActorRotation(SmoothRot);

	}
}
void AMyCharacterController::SpecialAttackPlay()
{
	if(bTargeting && TargetActor)
	{
		 if(GetActorForwardVector().Y <= -0.7)
		 {
			//Play Monatge
		
			RotateTotargetEvent();
			SetCombatStatus(ECombatState::ECS_Attack);
			//if wwant then add buffer 

			PlayMontages(dodgeMontage);
		//	return true;
		}
	}
}

void AMyCharacterController::FininshButtonPressed()
{
	if(bTargeting && TargetActor)
	{

			if(GetDistanceTo(TargetActor) <= 100.0f)
			{
				
				AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(TargetActor);
				if(Enemy)
				{
						SetCombatStatus(ECombatState::ECS_Fininsher);
					FRotator targetRot = 	UKismetMathLibrary::FindLookAtRotation(GetActorLocation() ,Enemy->GetActorLocation());
					Enemy->FinishedEnemy(this);
					//	UE_LOG(LogTemp, Warning, TEXT("Do play Lock is Work") );
					SetActorRotation(targetRot);
					PlayMontages(FinishMontage ,Enemy->ExecutionName);

					//Clear The TargetActor  = nullptrand bTargeting = false
					//StopRotation
					//Stop Buffer
				}
			}
	}
}

void AMyCharacterController::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

    AActor* HitActor = Hit.GetActor();
	if(HitActor)
	{
		if (HitActor->IsA<AStaticMeshActor >())
		{
			 AStaticMeshActor* LandedActor = Cast<AStaticMeshActor>(HitActor);
            if (LandedActor)
            {
             	bLaunched = false;
            }
		}
	}
}

 void AMyCharacterController::AimButtonPressed()
 {
	
	if(ShadowStrikeCompo->GetShadowStrikeState() == EActionState::EAS_Idle){	
		
			if(BowAimSound){
	UGameplayStatics::PlaySound2D(this ,BowAimSound );
	}

			bUseControllerRotationYaw = true;
		CameraBoom->TargetOffset = FVector(0.0f , 0.0f , 35.0f);
		CameraBoom->SocketOffset = FVector(0.0f , 75.0f , 0.0f);
		ShadowStrikeCompo->bAiming = true;
		ShadowStrikeCompo->SlowMotionEffectGame();
		AdjustCameraZoomInOut(true);
		ShadowStrikeCompo->SetActionState(EActionState::EAS_Aim);
	}
		//ShadowStrikeCompo->ZoomInCam();
 }

 void AMyCharacterController::AimButtonReleased()
 {
		if(ShadowStrikeCompo->GetShadowStrikeState() == EActionState::EAS_Aim){	

	
		ShadowStrikeCompo->bAiming = false;
			ShadowStrikeCompo->SetActionState(EActionState::EAS_Idle);
		}
		ShadowStrikeCompo->SlowMotionEffectOff();
		bUseControllerRotationYaw = false;
			AdjustCameraZoomInOut(false);

				CameraBoom->TargetOffset = FVector(0.0f , 0.0f , 0.0f);
		CameraBoom->SocketOffset = FVector(0.0f , 0.0f , 0.0f);

 }

 void AMyCharacterController::TeleportPlayerBtnPressed()
 {
	//ShadowStrikeCompo->TeleportTarget();

	ShadowStrikeCompo->StartInitialStrike();
	
	//DirectRotateToTarget(ShadowStrikeCompo->currentTarget);
 }
void AMyCharacterController::SetCameraLagboom(float CameraLagSpeeds)
{
CameraBoom->CameraLagSpeed = CameraLagSpeeds;
}
 void AMyCharacterController::DirectRotateToTarget(AActor* Target)
 {
	FRotator TargetRotation =  UKismetMathLibrary::FindLookAtRotation(GetActorLocation() , Target->GetActorLocation() );
	//this->GetCapsuleComponent()->SetWorldRotation(targetRot);

	   SetActorRotation(TargetRotation);
	// /UE_LOG(LogTemp , Warning ,TEXT("Rotate to taregt "));
 }
void AMyCharacterController::UpDateGlobalTimeDialiation(float timeDalVal)
{
	UGameplayStatics::SetGlobalTimeDilation(this, timeDalVal);
}
void AMyCharacterController::TraceEnemiesForChainAttackNotify()
{

	ShadowStrikeCompo->TraceEnemiesForChainAttack();
}
void AMyCharacterController::OnChainTimerExpiredNotify()
{
	UpDateGlobalTimeDialiation(1.0f);
	ShadowStrikeCompo->OnChainTimerExpired();
}

void AMyCharacterController::PerformDashs()
{
    if (currentState == ECombatState::ECS_Nothing)
    {
       ShadowStrikeCompo->PerformDash();
    }
}

// void AMyCharacterController::EndDashEffect()
// {
		
// 		SetVisibilityMesh(true);
//     bCanDash = true;
// }


// void AMyCharacterController::BlinkDashFX()
// {
// 	if(dashTrailSystem){

// 	 UNiagaraFunctionLibrary::SpawnSystemAtLocation(this , dashTrailSystem , GetCharacterMovement()->GetLastUpdateLocation() ,
// 	UKismetMathLibrary::Conv_VectorToRotator(GetActorForwardVector()) ,FVector(1.0f , 1.0f , 1.0f)  , true , true , ENCPoolMethod::None , true);
// 	}
 
// }

void AMyCharacterController::SetVisibilityMesh(bool bShow)
{
	GetMesh()->SetVisibility(bShow , false);
	weaponKatana->SetVisibility(bShow);
}

// Function to handle vertical surface movement using ray trace
	FVector AMyCharacterController::VerticalSurfaceMovement()
{
    FVector Start = GetActorLocation();
    FVector End = Start - FVector(0.0f, 0.0f, 1000.0f);

    FHitResult HitResult;
    FCollisionQueryParams CollisionParams;
    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);

    if (bHit)
    {
        // Check if hit normal is mostly vertical
        if (FMath::Abs(HitResult.ImpactNormal.Z) > 0.8f)
        {
            return HitResult.ImpactNormal; // Return surface normal
        }
    }

    return FVector::ZeroVector; // Character is not on a vertical surface
}

void AMyCharacterController::StartBlocking()
{
	SetCombatStatus(ECombatState::ECS_Blocking);
	bBlockPressed = true;
}


void AMyCharacterController::StopBlocking()
{
	SetCombatStatus(ECombatState::ECS_Nothing);
	bBlockPressed = false;
}
// void AMyCharacterController::PlaySpeceficMontages(UAnimMontage* montage)
// {

// }
// Called to bind functionality to input
void AMyCharacterController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward" , this , &AMyCharacterController::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight" , this , &AMyCharacterController::MoveRight);

	PlayerInputComponent->BindAction("Jump" , IE_Pressed , this , &AMyCharacterController::Jump);
	//PlayerInputComponent->BindAction("Jump" , IE_Released , this , &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("Turn", this, &AMyCharacterController::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMyCharacterController::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &AMyCharacterController::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMyCharacterController::LookUpAtRate);

	//PlayerInputComponent->BindAction("MouseLeftButton")
	PlayerInputComponent->BindAction("LeftMouseB" , IE_Pressed , this , &AMyCharacterController::DoLightAttack);
	PlayerInputComponent->BindAction("LeftMouseB" , IE_Released , this , &AMyCharacterController::DoLightAttackRelesead);

	PlayerInputComponent->BindAction("RightMouseB" , IE_Pressed , this , &AMyCharacterController::DoHeavyAttack);

	
	PlayerInputComponent->BindAction("DodgeBtn" , IE_Pressed , this , &AMyCharacterController::DoDodge);
	PlayerInputComponent->BindAction("TabButton" , IE_Pressed , this , &AMyCharacterController::TabButtonPressed);

		PlayerInputComponent->BindAction("Finish" , IE_Pressed , this , &AMyCharacterController::FininshButtonPressed);
	PlayerInputComponent->BindAction("MidMouseBtn" , IE_Pressed , this , &AMyCharacterController::AimButtonPressed);
	PlayerInputComponent->BindAction("MidMouseBtn" , IE_Released , this , &AMyCharacterController::AimButtonReleased);

	
	PlayerInputComponent->BindAction("XBtn" , IE_Pressed , this , &AMyCharacterController::TeleportPlayerBtnPressed);

	PlayerInputComponent->BindAction("LShiftBtn" , IE_Pressed , this , &AMyCharacterController::PerformDashs);

	PlayerInputComponent->BindAction("Block" , IE_Pressed , this , &AMyCharacterController::StartBlocking);
	PlayerInputComponent->BindAction("Block" , IE_Released , this , &AMyCharacterController::StopBlocking);
	//delet after
//	PlayerInputComponent->BindAction("Zb" , IE_Pressed , this , &AMyCharacterController::DoAnimationWith);
}


