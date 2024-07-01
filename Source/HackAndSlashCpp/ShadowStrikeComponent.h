// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShadowStrikeComponent.generated.h"


UENUM(BlueprintType)    
enum class EActionState : uint8
{
	EAS_Idle UMETA(DisplayName = "Idle"),
	EAS_Aim UMETA(DisplayName = "Aim"),
	EAS_Teleporting UMETA(DisplayName = "Teleport	"),
	EAS_Striking UMETA(DisplayName = "Striking")
	//EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)    
enum class EFoundDirection : uint8
{
	EFD_None UMETA(DisplayName = "None"),
	EFD_Front UMETA(DisplayName = "Front"),
	EFD_back UMETA(DisplayName = "back	")

	
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HACKANDSLASHCPP_API UShadowStrikeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UShadowStrikeComponent();
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
private:
class AMyCharacterController* ACharacter;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Character Movement: Climbing",meta = (AllowPrivateAccess = "true"))
		//TArray<TEnumAsByte<EObjectTypeQuery> > pawn;

	class UCurveFloat* FloatC;
	void SlowMotionEffectGame();
	
	void SlowMotionEffectOff();
	FTimerHandle SlowInterpTimer;

	//UFUNCTION(BlueprintCallable)
	//void ZoomOut\Cam();
	//void ZoomOutCam();
	UFUNCTION(BlueprintCallable)
	void ZoomInCam(float Alpha);


	void ToggleWarithMaterial(float Alpha);

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	bool bAiming;
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	// class UCurveFloat* slowCurve;	
	// 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	// class UCurveFloat* ZoomIn;
	// 		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	// class UCurveFloat* ZoomOut;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EActionState CurrActionState;
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EFoundDirection foundDirectionState;

	// UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Launch Settings",meta = (AllowPrivateAccess = "true"))
	// TArray<TEnumAsByte<EObjectTypeQuery> > collsionToPawn;

	void PerformTargetfromCenterOfScreen();
	bool bDetectEnemy;

	EFoundDirection CheckforValidTelePortationLocation(AActor* target);
	bool DirectionCapsuleTrace(AActor* target ,EFoundDirection Direction);
	FVector CapsuleLocation;
	bool FloorTrace(FVector capsuleLocation );
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class AActor* currentTarget;
	class IStrikeInterface* strike;
	void TeleportTarget();

	float ReturnStateValue(EFoundDirection Direction);

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Weapon Collision",meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery> > pawnObjectType;
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	TArray<AActor*>  ValidChainStrikeActors;
	bool bXpressed;
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	AActor* ChainStrikeTarget;	
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	AActor* UpdatedActor;	
	// UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	// TArray<AActor*>  LFoundActor;
	// UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	//  	AActor* LCurrntActor ;
	float CurrntDistace = 1000.0f;
	//Once the strike happen, start multi sphere trace to detect possible targets around the player
	
	//UFUNCTION(BlueprintCallable)
	void TraceEnemiesForChainAttack();

	//Check if the target is in 1. Within vision range 2.the line of sight to that character is not blocked then add to a actor array
	AActor* SelectNextTarget();

	bool CheckforlineSight(AActor* target ,  FName SocketName);

	//We have to also get the target based on the closest distance to player
	AActor* FilterActorbasedOnDistance(TArray<AActor*>  LFoundActors);

	UFUNCTION(BlueprintCallable)
	void StartInitialStrike();
	UFUNCTION(BlueprintCallable)
	void StartChainShadowStrike();

	//UFUNCTION(BlueprintCallable)
	void OnChainTimerExpired();

		void ToggleTargetIndicatorsInteface(AActor* Target , bool show);
		//Rotate the player towards the target smoothly
		void SmoothRotateToTarget(AActor* Target);

			void DirectRotateToTarget(AActor* Target);


			UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ShadownStrikeAnim")
	class UAnimMontage* finishingAttackMontageBack;   //HitMontage animation

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ShadownStrikeAnim")
	class UAnimMontage* VicTinAttackMontageBack;   //HitMontage animation

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ShadownStrikeAnim")
	class UAnimMontage* finishingAttackMontageFront;   //HitMontage animation

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ShadownStrikeAnim")
	class UAnimMontage* VicTinAttackMontageFront;   //HitMontage animation

	UFUNCTION()
	void FinishedEnemyPlay(AActor* target , EFoundDirection Directions);

	void AnimMontagePlayShadow( UAnimMontage* AttackMontageBack );

	UFUNCTION()
	void MoveToTargetFinished();
	// /void ToggleCollsion();
#pragma region DashEffect
	   UPROPERTY(EditAnywhere, Category = "Dash")
    float DashDistance;

    UPROPERTY(EditAnywhere, Category = "Dash")
    float DashCooldown;

    UPROPERTY(EditAnywhere, Category = "Dash")
    float DashSpeedMultiplier;

	  bool bCanDash;
	  FTimerHandle CooldownTimerHandle;

    FVector DashDirection;

    UFUNCTION()
    void PerformDash();

	UPROPERTY()
		class UNiagaraComponent* TrailSystemComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BlinkNigara")
	 class UNiagaraSystem* dashTrailSystem;

	 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BlinkNigara")
	 class UNiagaraSystem* teleportSystem;
	 UFUNCTION()
    void BlinkDashFX();

    UFUNCTION()
    void EndDashEffect();

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class USoundCue* StrikeSound;   // hit shound to enemy

			UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class USoundCue* DashSoundCue;   // hit shound to enemy


#pragma endregion
		
	FORCEINLINE EActionState GetState() const { return CurrActionState ;}
	FORCEINLINE void SetActionState(EActionState NewcurrentState) { CurrActionState =  NewcurrentState; } 
	FORCEINLINE void SetFoundDirectionsState(EFoundDirection NewcurrentState) { foundDirectionState =  NewcurrentState; }

	FORCEINLINE  EActionState GetShadowStrikeState() const { return CurrActionState;}   
};
