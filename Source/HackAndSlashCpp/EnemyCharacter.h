// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "StrikeInterface.h"
#include "EnemyCharacter.generated.h"


// USTRUCT(BlueprintType)
// struct FExecutaionAtack
// {
// 	GENERATED_BODY()


// 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Finisher")
// 	class UAnimMontage* ExecutionMonatge;   //Combat animation
	
// 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Finisher")
// 	class UAnimMontage* TargetMontage;   //Combat animation
// };


UENUM(BlueprintType)
enum class E_DamageTypecp : uint8
{
    DMG_Left UMETA(DisplayName = "Left"),
    DMG_Right UMETA(DisplayName = "Right"),
    DMG_Middle UMETA(DisplayName = "Middle"),
    DMG_KnockBack UMETA(DisplayName = "KnockBack")
};


UCLASS()
class HACKANDSLASHCPP_API AEnemyCharacter : public ACharacter ,public IStrikeInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
		//take damage from player
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser) override;
	// New function for handling AnyDamage
    UFUNCTION(BlueprintCallable, Category = "Damage")
    void AnyDamage(float DamageAmount, TSubclassOf<UDamageType> DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	/**
	* Play montage functions
	*/
	void PlayHitReactMontage(const FName& SectionName , AActor* DamageCauser);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Collision")
	//TSubclassOf<UDamageType> DamageTypeMain;
	public:
	
#pragma region Buffer


	UFUNCTION(BlueprintCallable)
	void BufferTimer(float Value);

	float BufferAmount;


	UFUNCTION(BlueprintImplementableEvent)
	void StartBuffer();

	UFUNCTION(BlueprintImplementableEvent)
	void StopBuffer();
#pragma endregion	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
   	class USceneComponent* SpawnBloodCompo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitReactionAttack")
	class UAnimMontage* GroundHitReactioMontage;   //HitMontage animation

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitReactionAttack")
	class UAnimMontage* AirHitReactioMontage;   //HitMontage animation

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitReactionAttack")
	class UAnimMontage* knockDownMonatge;   //HitMontage animation

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitReactionAttack")
	class UAnimMontage* finishingAttackMontage;   //HitMontage animation

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon State", meta = (AllowPrivateAccess = "true"))
	//class USkeletalMeshComponent* weaponKatana;
	class USkeletalMeshComponent * weaponKatana;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitReactionAttack")
	class UAnimMontage* AttackMontage;   //HitMontage animation

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitReactionAttack")
	class UAnimMontage* DefenseMontage;   //defense animation

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitReactionAttack")
	class UAnimMontage* EnemyDodgeMontage;   //defense animation
	
	UFUNCTION(BlueprintCallable)
	void SwordTrace();

	UFUNCTION(BlueprintCallable)
	void PlayMontageAttack();

	UPROPERTY()
	FVector DesiredLocation ;
	UFUNCTION(BlueprintImplementableEvent, Category = "Launch Settings")
	void launchThisEnemy();

	UFUNCTION(BlueprintCallable, Category = "Launch Settings")
	void ResetAirComabat();

	UFUNCTION(BlueprintCallable, Category = "Launch Settings")
	void KnockDownEnemy();

	UFUNCTION()
	void FinishedEnemy(AActor* player);
	
	class UAnimInstance* AnimInstance ;

	//UPROPERTY(EditDefaultsOnly , BlueprintReadOnly)
	//TArray<FExecutaionAtack> finishingMontage;
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "")
	class UWidgetComponent* targetIndicator;

	void OntargetIndicatorShow(bool canShow);
	FName ExecutionName;

	bool isDead;

	UFUNCTION(BlueprintCallable)
	void ActivateDeath();

	UFUNCTION(BlueprintCallable)
	void DisableCollision();

	UFUNCTION(BlueprintCallable)
	void AttachWeaponHand(class USkeletalMeshComponent *AttachInhand);


	class AEnemyAIController* EnemyController;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "blockPressed")
	class AMyCharacterController* character;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "blockPressed")
	bool Strafe = false;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "blockPressed")
	bool bDefense;

	void SetDefense();

	
	UFUNCTION()
	void ResetSetDefenseFalse();

	FTimerHandle  DefenseTimer;

	virtual void ToggleTargetIndicator(bool bShow) override;
	virtual void PlayCharacterAnimMontage(class UAnimMontage* MontageToPlay , FName Section ) override;
	virtual void SetDeadState(bool Died) override;
	virtual bool IsDied() override;
};
