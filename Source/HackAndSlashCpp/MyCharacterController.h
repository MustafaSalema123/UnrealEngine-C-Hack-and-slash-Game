// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
//#include "Components/TimelineComponent.h"
#include "MyCharacterController.generated.h"

UENUM(BlueprintType)    //Enum of movement stats
enum class ECombatState : uint8
{
	ECS_Nothing UMETA(DisplayName = "Nothing"),
	ECS_Attack UMETA(DisplayName = "Attack"),
	ECS_Blocking UMETA(DisplayName = "Block"),
	ECS_Dodge UMETA(DisplayName = "Dodge"),
	ECS_Fininsher UMETA(DisplayName = "finisher"),
	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class HACKANDSLASHCPP_API AMyCharacterController : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacterController();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void AddControllerPitchInput(float Val) override;
	virtual void AddControllerYawInput(float Val) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
   	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraFollow;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	//class USkeletalMeshComponent* weaponKatana;
	class UStaticMeshComponent* weaponKatana;
	
	UPROPERTY(VisibleAnywhere , BlueprintReadOnly , meta = (AllowPrivateAccess = "true")) //we make read only because we make a caleed in bluerint also for finfih montage
		class UShadowStrikeComponent* ShadowStrikeCompo;




private:
#pragma region InputVariable
	void MoveRight(float Value);
	void MoveForward(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);
	
#pragma endregion	


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

#pragma region Buffer

	UFUNCTION(BlueprintCallable)
	void BufferTimer(float Value);

	float BufferAmount;


	UFUNCTION(BlueprintImplementableEvent)
	void StartBuffer();

	UFUNCTION(BlueprintImplementableEvent)
	void StopBuffer();

	
		UFUNCTION(BlueprintImplementableEvent)
		void AdjustCameraZoomInOut(bool bAim);
	
#pragma endregion	

	class UAnimInstance* AnimInstance;
	//void PlaySpeceficMontages(UAnimMontage* montage);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	ECombatState currentState;   //enum of Movemnent status

	UPROPERTY(EditAnywhere, Category = "maxInt")
    int32 MaxAnimationsInMontage; // Maximum number of animations in the montage

	bool CanAttack();
#pragma region LightAttack


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LightAttack")
	class UAnimMontage* LightAttackMontage;   //Combat animation

	

    UPROPERTY(BlueprintReadOnly, Category = "LightAttack")
    int32 AttackIndexes; // Current index for light attack

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "LightAttack")
    bool bShouldSaveLightAttack; // Current index for light attack

    UFUNCTION()
    void DoLightAttack();
	UFUNCTION()
	void DoLightAttackRelesead();
	UFUNCTION(BlueprintCallable)
	void SaveLightAttack();
	UFUNCTION(BlueprintCallable)
	void ResetStatesAttack();

	bool bLightIputHead;



#pragma endregion	
#pragma region HeavyAttack

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HeavyAttack")
	class UAnimMontage* HeavyAttackMontage;   //Combat animation

	    UPROPERTY(BlueprintReadOnly, Category = "LightAttack")
    int32 AirAttackIndexes; // Current index for light attack

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "HeavyAttack")
    bool bShouldSaveHeavyAttack; // Current index for light attack

    UFUNCTION(BlueprintCallable, Category = "HeavyAttack")
    void DoHeavyAttack();

	UFUNCTION(BlueprintCallable)
	void SaveHeavyAttack();
	
#pragma endregion	
#pragma region AirAttack


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Air Attack")
	class UAnimMontage* AirAttackMontage;   //Combat animation

	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category = "Air Attack")
    bool bLaunched; // Current index for light attack
	


	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Air Attack")
    bool bShouldSaveAirAttack; // Current index for light attack
	
    UFUNCTION()
    void DoAirAttack();
	
	// UFUNCTION(BlueprintCallable)
	// void SaveAirAttack();

	UFUNCTION(BlueprintCallable)
	void ResetAirCombatAttack();
#pragma endregion	
#pragma region Dodge

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HeavyAttack")
	class UAnimMontage* dodgeMontage;   //Combat animation

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Air Attack")
    bool bShouldSaveDodge; // Current index for light attack
    UFUNCTION(BlueprintCallable, Category = "HeavyAttack")
    void DoDodge();

	UFUNCTION(BlueprintCallable)
	void SaveDodgeMon();

	void RotationForDodge();
	
#pragma endregion	

#pragma region DoubleJump

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DoubleJump")
	class UAnimMontage* doubleJumpMontage;   //Combat animation

	virtual void Jump() override;
	virtual void Landed(const FHitResult& Hit);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DoubleJump")
	bool bHasDoubleJumped;

#pragma endregion	
#pragma region WeaponCollsion

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Weapon Collision",meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery> > collsionSurfaceTraceTypes;
TArray<AActor*> myPlayer;
TArray<AActor*>  AlredyhitActor;

UFUNCTION(BlueprintCallable)
	void WeaponSphereTracing();
	bool bActiveCollision;
UFUNCTION(BlueprintCallable)
	void StartWeaponCollision();

UFUNCTION(BlueprintCallable)
	void EndWeaponCollision();

bool bActiveCollison;

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Collision")

TSubclassOf<UDamageType> DamageTypeClass;  //This is UDamageTyep A DamageType is intended to define and describe a particular form of damage and to provide an
#pragma endregion	

#pragma region SoftLockOn

	void  LookAtEnemy();
	void  TabButtonPressed();
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Weapon Collision",meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery> > collsionToPawn;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Weapon Collision",meta = (AllowPrivateAccess = "true"))
	AActor* TargetActor;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Weapon Collision",meta = (AllowPrivateAccess = "true"))
	AActor* SoftTarget;
	bool bTargeting = false;


	UFUNCTION()
	void SoftOntarget();

	UFUNCTION(BlueprintCallable)
	void RotateTotarget(float Value);

	UFUNCTION(BlueprintImplementableEvent)
	void RotateTotargetEvent();

	UFUNCTION(BlueprintImplementableEvent)
	void StopRotation(); // add in teh time line and do in int reset attack evevnt
//	void SmoothRotate(AActor* targets);
#pragma endregion	

#pragma region LaunchCharacter

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Launch Settings")
	class UAnimMontage* lauchCharMontage;   //Combat animation

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Launch Settings")
	class UAnimMontage* GroundSlamMontage;   //Combat animation

	// UPROPERTY(EditAnywhere, Category = "Launch Settings")
    // class UCurveFloat* LaunchCurve;
	
    
    // FTimeline CurveTimeline;
	
	bool CanLaunchPlayer();
	void DoAnimationWith();

	UFUNCTION(BlueprintCallable)
	void launchCharacterMy();
	
	//UFUNCTION(BlueprintCallable, Category = "Launch Settings")
	 //void LaunchCharacterTimeline(float Alpha);
	//FVector DesiredLocation ;
	 	UFUNCTION(BlueprintImplementableEvent, Category = "Launch Settings")
	void RaiseLaunch();   //this is implement is floorSwitchbp  the image is floorSwitch.png screenshot 

	UFUNCTION(BlueprintImplementableEvent, Category = "Launch Settings")
	void DownPlayer();   //this is implement is   the image is floorSwitch.png screenshot
	UFUNCTION(BlueprintCallable)
	void CrashedSmackDown();  

	UFUNCTION(BlueprintCallable, Category = "Launch Settings")
	 void DowncharacterTimeline(float Alpha);
	UFUNCTION(BlueprintCallable, Category = "Launch Settings")
	 void EventNotifyGroundSlam();

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Launch Settings",meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery> > collsionDown;

	UPROPERTY(BlueprintReadWrite, Category = "Launch Settings")
	FVector GroundLocation;
#pragma endregion	
#pragma region SpacialAtatck
UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpacialAtatck")
	class UAnimMontage* SpacialAtatckMontage;   //Combat animation

	UFUNCTION(BlueprintCallable, Category = "SpacialAtatck")
	 void SpecialAttackPlay();

#pragma endregion
#pragma region FininishingAttack
UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FininishingAttack")
	class UAnimMontage* FinishMontage;   //Combat animation

	UFUNCTION()
	 void FininshButtonPressed();

#pragma endregion

#pragma region AimingFuch
	UFUNCTION()
	 void AimButtonPressed();
	 UFUNCTION()
	 void AimButtonReleased();
#pragma endregion

#pragma region DashEffect


    UFUNCTION()
    void PerformDashs();


	void SetVisibilityMesh(bool bShow);
#pragma endregion
	/**
	* Play montage functions
	*/
	void PlayMontages(UAnimMontage* montage , FName Section = FName("Default"));

		void PerformCombatAttacks(UAnimMontage* montage ,int32 maxAttack , FString SectionName);

/** The curve asset to use for the item's Z location when interping */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* bufferCurve;
	UFUNCTION(BlueprintCallable)
	void BufferStart();

	
	void TeleportPlayerBtnPressed();

	
	void DirectRotateToTarget(AActor* Target);
	UFUNCTION(BlueprintCallable)
	void UpDateGlobalTimeDialiation(float timeDalVal);
	UFUNCTION(BlueprintCallable)
	void TraceEnemiesForChainAttackNotify();

	UFUNCTION(BlueprintCallable)
	void SetCameraLagboom(float CameraLagSpeeds);
	UFUNCTION(BlueprintImplementableEvent)
	void ChangeCameraLagboomEvent();

	UFUNCTION(BlueprintCallable)
	void OnChainTimerExpiredNotify();



		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class USoundCue* BowAimSound;   // hit shound to enemy

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class USoundCue* SwingSound;   // hit shound to enemy
			UFUNCTION(BlueprintCallable)
	void SpawnBloodPartcle();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UParticleSystem* BloodHitParticles;            


	FVector VerticalSurfaceMovement();


#pragma region Blocking



	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "blockPressed")
	bool bBlockPressed = false;

	UFUNCTION()
	void StartBlocking();

	UFUNCTION()
	void StopBlocking();


	
#pragma endregion	
	// UPROPERTY(VisibleAnyWhereueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	// FVector SpawnBloodLocation;
	public :
	
	FORCEINLINE ECombatState GetState() const { return currentState ;}
	FORCEINLINE void SetCombatStatus(ECombatState NewcurrentState) { currentState =  NewcurrentState; }   
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return CameraFollow ;}

	
	//FORCEINLINE UCameraComponent* GetFollowCamera() const { return CameraFollow ;}
};
