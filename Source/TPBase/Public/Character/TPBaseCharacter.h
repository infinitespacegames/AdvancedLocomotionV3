// Copyright is Bullshit!  Do as you will with these files.

#pragma once

#include "TPBase.h"
#include "TPBaseEnum.h"
#include "TPBaseStruct.h"
#include "TPBaseMovement.h"

#include <GameFramework/Character.h>
#include <Components/TimelineComponent.h>
#include "TPBaseCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UTimelineComponent;
class UPlayerInfoWidget;
class UHealthComponent;
class UPointOfInterest;

 
UCLASS()
class TPBASE_API ATPBaseCharacter : public ACharacter {

	GENERATED_BODY()

public:

	/** Base constructor */
	ATPBaseCharacter(const FObjectInitializer& ObjectInitializer);

	/** Called every frame */
	virtual void Tick(float DeltaTime) override;

	/** Called for property replication */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	/** Called to bind functionality to input */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;


private:
	////////////////////////////////////////////////////////////////////
	//
	//   Native event handling
	//
	////////////////////////////////////////////////////////////////////

	/** Handles native begin crouch event */
	virtual void OnStartCrouch(float HHAdjust, float SHHAdjust) override;

	/** Handles native end crouch event */
	virtual void OnEndCrouch(float HHAdjust, float SHHAdjust) override;

	/** Handle native landing event */
	virtual void Landed(const FHitResult& Hit) override;

	/** Handles native movement mode change event */
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PrevCustomMode) override;

	/** Handles health changed event */
	UFUNCTION()
	void OnHealthChanged(UHealthComponent* HealthComp, float HealthValue, float ShieldValue, float Damage,
		const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);


private:
	////////////////////////////////////////////////////////////////////
	//
	//   Player input handling
	//
	////////////////////////////////////////////////////////////////////

	/** Handle forward/backward control input */
	void MoveForward(float Value);

	/** Handle right/left */
	void MoveRight(float Value);

	/** Handle turn-rate control input */
	void TurnAtRate(float Rate);

	/** Handle lookup-rate control input */
	void LookUpAtRate(float Rate);

	/** Handle jump control input */
	void JumpAction();

	/** Handle stance change control input */
	void StanceAction();

	/** Handle walk control input */
	void WalkAction();

	/** Handle sprint control input */
	void BeginSprint();

	/** Handle sprint control input */
	void EndSprint();

	/** Handle aim down sights control input */
	void BeginAiming();

	/** Handle aim down sights control input */
	void EndAiming();

	/** Handle rotation mode control input */
	void SwitchRotationMode();

	/** Handle ragdoll mode control input*/
	void RagdollAction();

	/** Handle forward foot control input */
	void SwitchForwardFoot();

	/** Handle camera action control input */
	void BeginSwitchCamera();

	/** Handle camera action control input */
	void EndSwitchCamera();

	/** Handle camera action control input */
	void SwitchCamera();


public:
	////////////////////////////////////////////////////////////////////
	//
	//   Character getter functions, based on state
	//
	////////////////////////////////////////////////////////////////////

	/** Get camera settings based on character animation state */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Getters")
	FCameraSettings GetCameraTargetSettings();

	/** Get character velocity based on animation state */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Getters")
	FVector GetCharacterVelocity();

	/** Determine rotation rate from character state */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Getters")
	float GetCharacterRotationRate(float SlowSpeed, float SlowRate, float FastSpeed, float FastRate);

	/** Get current aiming state */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Setters")
	bool IsAiming() const;

	/** Get current running state */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Setters")
	bool IsRunning() const;

	/** Get current sprinting state */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Setters")
	bool IsSprinting() const;

	/** Get current rotation mode */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Setters")
	EStance GetStance() const;

	/** Get current rotation mode */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Setters")
	EGaitMode GetGaitMode() const;

	/** Get current rotation mode */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Setters")
	ERotationMode GetRotationMode() const;

	/** Get current locomotion mode */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Setters")
	ELocomotionMode GetLocomotionMode();

	/** Get crouched speed configuration input */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Getters")
	float GetCrouchingSpeed() const;

	/** Get walking speed configuration input */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Getters")
	float GetWalkingSpeed() const;

	/** Get running speed configuration input */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Getters")
	float GetRunningSpeed() const;

	/** Get sprinting speed configuration input */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Getters")
	float GetSprintingSpeed() const;

	/** Get walk acceleration configuration input */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Getters")
	float GetWalkAcceleration() const;
	
	/** Get run acceleration configuration input */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Getters")
	float GetRunAcceleration() const;
	
	/** Get walk deceleration configuration input */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Getters")
	float GetWalkDeceleration() const;
	
	/** Get run deceleration configuration input */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Getters")
	float GetRunDeceleration() const;
	
	/** Get walk ground friction configuration input */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Getters")
	float GetWalkGroundFriction() const;
	
	/** Get run ground friction configuration input */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Getters")
	float GetRunGroundFriction() const;


public:
	////////////////////////////////////////////////////////////////////
	//
	//   Character setter functions
	//
	////////////////////////////////////////////////////////////////////

	/** Set the character rotation */
	//UFUNCTION(Server, Reliable, WithValidation)
	//void 
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Setters")
	void SetCharacterRotation(FRotator NewRotation, bool bDoInterp, float InterpSpeed);

	/** Add given amount to the character rotation */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Setters")
	void AddCharacterRotation(FRotator AdditiveRotation);

	/** Limit amount of yaw per frame */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Setters")
	void LimitCharacterRotation(float AimYawLimit, float InterpSpeed);

	/** Called to update camera view mode */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Setters")
	void SetCameraMode(ECameraMode NewMode);

	/** Interface callback for updating forward foot */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetForwardFoot(bool bRightFoot);

	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Setters")
	void SetForwardFoot(bool bRightFoot);

	/** Interface callback for updating dead state */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetIsDead(bool bDead);
	
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Setters")
	void SetIsDead(bool bDead);

	/** Interface callback for updating camera view shoulder */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Setters")
	void SetRightShoulder(bool bRight);

	/** Interface callback for updating aiming state */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Setters")
	void SetAiming(bool bAiming);

	/** Called to update character rotation mode */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Setters")
	void SetRotationMode(ERotationMode NewMode);

	/** Called to update character locomotion mode */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Setters")
	void SetLocomotionMode(ELocomotionMode NewMode);

	/** Called to update character crouched speed */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Setters")
	void SetCrouchSpeed(float NewSpeed);

	/** Called to update character walking speed */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Setters")
	void SetWalkSpeed(float NewSpeed);

	/** Called to update character running speed */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Setters")
	void SetRunSpeed(float NewSpeed);

	/** Called to update character sprinting speed */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Setters")
	void SetSprintSpeed(float NewSpeed);

public:
	////////////////////////////////////////////////////////////////////
	//
	//   Character utility functions
	//
	////////////////////////////////////////////////////////////////////

	/** Callback for camera lerp curves*/
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Utility")
	void CameraLerpCallback(float Alpha);

	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Utility")
	void UpdateCapsuleVisibility();

	/** Called to display debug traces */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetShowSettings(bool bShow);

	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Utility")
	void SetShowSettings(bool bShow);

	/** Called to display debug traces */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetShowTraces(bool bShow);

	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Utility")
	void SetShowTraces(bool bShow);
	
	/** Displays character state information on screen */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Utility")
	void PrintCharacterInfo();

	/** Interface callback for updating camera  position */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Utility")
	void UpdateCamera(UCurveFloat* LerpCurve);

	/** Called to enter character into ragdoll state */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastEnterRagdoll();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerEnterRagdoll();
	
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Utility")
	void EnterRagdoll();

	/** Called to exit character from ragdoll state */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastExitRagdoll();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerExitRagdoll();
	
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Utility")
	void ExitRagdoll();

	/** Handle animNotify turn in place event */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Utility")
	void DelayedRotationNotify(FRotator AdditiveRotation, float DelayTime);

	/** Handle animNotify camera shake event */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Utility")
	void CameraShakeNotify(TSubclassOf<UCameraShake> ShakeClass, float ShakeScale);

	/** Create arrow scene components */
	void CreateArrowComponents();

	/** Called to update arrow positions */
	void UpdateArrowPositions();

	/** Called to update arrow vectors */
	void UpdateArrowComponents(bool bAlwaysUpdate = false);


public:
	////////////////////////////////////////////////////////////////////
	//
	//   Character calculation functions
	//
	////////////////////////////////////////////////////////////////////

	/** Determines the looking yaw offset */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Utility")
	FRotator CalculateLookingDirection(float NEAngle, float NWAngle, float SEAngle, float SWAngle, float Buffer, float InterpSpeed);

	/** Determines if within range of given cardinal direction with a tolerance */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Utility")
	float WithinCardinalRange(float Value, float Min, float Max, float Tol, ECardinalDirection Cardinal);

	/** Determines what it says it does :) */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Utility")
	bool RagdollLineTrace(FVector InLocation, FRotator InRotation, FVector& OutLocation, FRotator& OutRotation);

	/** Calculates the character current state */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Utility")
	void CalculateStateVariables();


	////////////////////////////////////////////////////////////////////
	//
	//   Rotation management functions
	//
	////////////////////////////////////////////////////////////////////

	/** Handles adjustments to character rotation */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Utility")
	void ManageCharacterRotation();

	/** Handles the manipulation of the ragdoll state */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Utility")
	void ManageRagdoll();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRagdollUpdate(bool bGrounded, FVector DollLocation, FVector NewLocation, FRotator NewRotation);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRagdollUpdate(bool bGrounded, FVector DollLocation, FVector NewLocation, FRotator NewRotation);

	/** Handle animNotify turn in place event */
	UFUNCTION(BlueprintCallable, Category = "TPBase|Character|Utility")
	void DelayedRotation(FRotator AdditiveRotation, float DelayTime);


public:
	////////////////////////////////////////////////////////////////////
	//
	//   Character state variables
	//
	////////////////////////////////////////////////////////////////////

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TPBase")
	FName FPCameraSocketName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TPBase")
	FName PelvisBoneName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TPBase")
	FName RagdollPoseName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TPBase")
	float HalfHeight = 90.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TPBase")
	float CrouchedHalfHeight = 60.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TPBase|Camera")
	bool bRightShoulder;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TPBase|Camera")
	ECameraMode CameraMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TPBase|Camera")
	FCameraTargetSettings CameraTargets;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TPBase|Camera")
	TArray<UCurveFloat*> CameraLerpCurves;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "TPBase|Debug")
	bool bShowSettings;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "TPBase|Debug")
	bool bShowTraces;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "TPBase|SpawnState")
	bool bRightFootForward;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TPBase|Movement")
	bool bToggleSprint;


public:
	////////////////////////////////////////////////////////////////////
	//
	//   Internally used replicated character variables
	//
	////////////////////////////////////////////////////////////////////

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "TPBase|InternalVariables")
	bool bIsDead;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	bool bIsMoving;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	bool bHasMovementInput;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	bool bIsRagdoll;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	bool bRagdollGrounded;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	ECardinalDirection CardinalDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	ELocomotionMode PrevLocomotionMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	EGaitMode CameraGaitMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	float AimYawDelta;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	float AimYawRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	float MovementDifferential;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	float RotationDifferential;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	float YawDifferential;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	float RotationOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	float RotationRateMultiplier;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	FVector MovementInput;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	FVector RagdollLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	FRotator CharacterRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	FRotator LookRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	FRotator TargetRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	FRotator JumpRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	FRotator PrevVelocityRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	FRotator PrevMovementRotation;

	UPROPERTY()
	FCameraSettings CurrentCameraSettings;


protected:
	////////////////////////////////////////////////////////////////////
	//
	//   Character components
	//
	////////////////////////////////////////////////////////////////////

	/** 3P camera boom */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USpringArmComponent* TP_SpringArm;

	/** Third person following camera view */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCameraComponent* TP_Camera;

	/** First person camera view */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCameraComponent* FP_Camera;

	/** Scene arrow components */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* Arrows;

	/** Looking direction vector */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UArrowComponent* LookingRotationArrow;

	/** Target direction vector */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UArrowComponent* TargetRotationArrow;

	/** Character direction vector */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UArrowComponent* CharacterRotationArrow;

	/** Movement input direction vector */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UArrowComponent* MovementInputArrow;

	/** Previous movement input vector */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UArrowComponent* PrevMovementInputArrow;

	/** Velocity direction vector */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UArrowComponent* VelocityArrow;

	/** Previous velocity vector */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UArrowComponent* PrevVelocityArrow;

	/** Point of interest icon for maps */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UPointOfInterest* PointOfInterest;

	/** Manages health statistics */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UHealthComponent* HealthComponent;

	/** Player character info display widget */
	UPROPERTY(BlueprintReadWrite)
	UPlayerInfoWidget* PlayerInfoWidget;


private:

	/** General timer for held inputs */
	FTimerHandle InputTimer;

	/** Used in playing blueprint curves */
	FOnTimelineFloat onCameraLerpCallback;

	/** Camera timeline */
	UPROPERTY()
	UTimelineComponent* CameraLerpTimeline;

public:

	FORCEINLINE UTPBaseMovement* GetTPBaseMovement() const { return Cast<UTPBaseMovement>(GetCharacterMovement()); };

};

