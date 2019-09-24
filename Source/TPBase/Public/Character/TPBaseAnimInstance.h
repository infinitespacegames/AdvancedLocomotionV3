// Copyright is Bullshit!  Do as you will with these files.

#pragma once

#include "TPBase.h"
#include "TPBaseStruct.h"
#include "TPBaseInterfaceABP.h"

#include <Animation/AnimInstance.h>
#include "TPBaseAnimInstance.generated.h"

class ATPBaseCharacter;
class UCapsuleComponent;
class UCharacterMovementComponent;



/**
 * Implements interface between character and animation blueprint
 */
UCLASS()
class TPBASE_API UTPBaseAnimInstance : public UAnimInstance, public ITPBaseInterfaceABP {

	GENERATED_BODY()

public:

	/** Default constructor */
	UTPBaseAnimInstance();

	/** Called at start of play */
	virtual void NativeInitializeAnimation() override;

	/** Called every frame */
	virtual void NativeUpdateAnimation(float DeltaTimeX) override;


protected:
	////////////////////////////////////////////////////////////////////
	//
	//   Animation core functions, can be overriden in blueprint or C++
	//
	////////////////////////////////////////////////////////////////////

	/** Temporary function for blueprint */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|Animation")
	void DoWhileMoving();
	virtual void DoWhileMoving_Implementation();

	/** Temporary function for blueprint */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|Animation")
	void DoWhileFalling();
	virtual void DoWhileFalling_Implementation();

	/** Temporary function for blueprint */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|Animation")
	void DoTurnInPlace();
	virtual void DoTurnInPlace_Implementation();

	/** Temporary function for blueprint */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|Animation")
	void DoWhileRagdoll();
	virtual void DoWhileRagdoll_Implementation();

	/** Performs a delayed turn */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|Animation")
	void DoDelayedTurn(float MaxCameraSpeed,
		float YawLimit1, float Delay1, float PlayRate1, FTurnAnimations TurnAnims1,
		float YawLimit2, float Delay2, float PlayRate2, FTurnAnimations TurnAnims2);
	virtual void DoDelayedTurn_Implementation(float MaxCameraSpeed,
		float YawLimit1, float Delay1, float PlayRate1, FTurnAnimations TurnAnims1,
		float YawLimit2, float Delay2, float PlayRate2, FTurnAnimations TurnAnims2);

	/** Performs a responsive turn */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|Animation")
	void DoResponsiveTurn(float YawLimit, float PlayRate, FTurnAnimations TurnAnims);
	virtual void DoResponsiveTurn_Implementation(float YawLimit, float PlayRate, FTurnAnimations TurnAnims);

	/** Calculates gait value based on character state */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|Animation")
	void CalculateGaitValue();
	virtual void CalculateGaitValue_Implementation();

	/** Calculates animation play rates based on character state */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|Animation")
	void CalculatePlayRates(float Crouch, float Walk, float Run, float Sprint);
	virtual void CalculatePlayRates_Implementation(float Crouch, float Walk, float Run, float Sprint);

	/** Calculates movement direction based on character state */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|Animation")
	void CalculateMovementDirection(float ThresholdMin = -90.0f, float ThresholdMax = 90.0f, float Tolerance = 5.0f);
	virtual void CalculateMovementDirection_Implementation(float ThresholdMin = -90.0f, float ThresholdMax = 90.0f, float Tolerance = 5.0f);

	/** Calculates the aim offset of the character */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|Animation")
	void CalculateAimOffset();
	virtual void CalculateAimOffset_Implementation();

	/** Calculates pivot information */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|Animation")
	void CalculatePivot();
	virtual void CalculatePivot_Implementation();

	/** Calculates the animation start position */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|Animation")
	void CalculateStartPosition();
	virtual void CalculateStartPosition_Implementation();

	/** Calculates the animation lean angle when grounded */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|Animation")
	void CalculateGroundedLeaningValues();
	virtual void CalculateGroundedLeaningValues_Implementation();

	/** Sets the animation lean angle when in air */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|Animation")
	void CalculateAirLeaningValues();
	virtual void CalculateAirLeaningValues_Implementation();

	/** Sets the animation landing prediction alpha value */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|Animation")
	void CalculateLandPredictionAlpha();
	virtual void CalculateLandPredictionAlpha_Implementation();


public:
	////////////////////////////////////////////////////////////////////
	//
	//   Animation notification events
	//
	//	TODO: Make notifies blueprint native
	////////////////////////////////////////////////////////////////////

	/** Animation notify for pivot */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TPBase|Notifications")
	void Pivot_Notify(const FPivotParameters& Params);

	/** Animation notify for turning in place */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TPBase|Notifications")
	void TurnInPlace_Notify(UAnimMontage* TurnAnim, bool bShouldTurn, bool bIsTurning, bool bRightTurn);

	/** Animation notify for idle entry */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TPBase|Notifications")
	void IdleEntry_Notify(EIdleEntryState NewIdleState);

	/** Animation notify for idle transitions */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TPBase|Notifications")
	void IdleTransition_Notify(UAnimSequenceBase* Animation, float PlayRate, float StartTime);

	/** Animation notify for entering locomotion state moving */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TPBase|Notifications")
	void AnimNotify_Entered_Moving();

	/** Animation notify for leaving locomotion state moving */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TPBase|Notifications")
	void AnimNotify_Left_Moving();

	/** Animation notify for entering locomotion state not moving */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TPBase|Notifications")
	void AnimNotify_Entered_NotMoving();

	/** Animation notify for leaving locomotion state not moving */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TPBase|Notifications")
	void AnimNotify_Left_NotMoving();

	/** Animation notify for entering locomotion state pivot */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TPBase|Notifications")
	void AnimNotify_Entered_Pivot();

	/** Animation notify for leaving locomotion state pivot */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TPBase|Notifications")
	void AnimNotify_Left_Pivot();

	/** Animation notify for entering locomotion state stopping */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TPBase|Notifications")
	void AnimNotify_Entered_Stopping();

	/** Animation notify for leaving locomotion state stopping */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TPBase|Notifications")
	void AnimNotify_Left_Stopping();

	/** Animation notify for landing */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TPBase|Notifications")
	void AnimNotify_Land();


public:
	////////////////////////////////////////////////////////////////////
	//
	//   Interface functions to needed character variables
	//
	////////////////////////////////////////////////////////////////////

	/** Calls all interface functions to synchronize animation state with character*/
	UFUNCTION(BlueprintCallable, Category = "TPBase|ABP_Interface")
	void UpdateAnimationState();

	/** Called to update animation debug traces state */
	virtual void OnSetShowTraces_Implementation(bool bShow) { bShowTraces = bShow; };

	/** Called to update animation ragdoll state */
	virtual void OnSetIsRagdoll_Implementation(bool bRagdoll) { bIsRagdoll = bRagdoll; };

	/** Called to update animation aiming state */
	virtual void OnSetIsAiming_Implementation(bool bAiming) { bIsAiming = bAiming; };

	/** Called to update animation moving state */
	virtual void OnSetIsMoving_Implementation(bool bMoving) { bIsMoving = bMoving; };

	/** Called to update animation movement input state */
	virtual void OnSetHasMovementInput_Implementation(bool bHasInput) { bHasMovementInput = bHasInput; };

	/** Called to update animation forward foot state */
	virtual void OnSetForwardFoot_Implementation(bool bRightFoot) { bRightFootForward = bRightFoot; };


	/** Called to update animation crouched speed */
	virtual void OnSetCrouchSpeed_Implementation(float NewSpeed) { CrouchSpeed = NewSpeed; };

	/** Called to update animation walking speed */
	virtual void OnSetWalkSpeed_Implementation(float NewSpeed) { WalkSpeed = NewSpeed; };

	/** Called to update animation running speed */
	virtual void OnSetRunSpeed_Implementation(float NewSpeed) { RunSpeed = NewSpeed; };

	/** Called to update animation sprinting speed */
	virtual void OnSetSprintSpeed_Implementation(float NewSpeed) { SprintSpeed = NewSpeed; };

	/** Called to update animation character velocity */
	virtual void OnSetCharacterVelocity_Implementation(FVector NewVelocity) { CharacterVelocity = NewVelocity; };


	/** Called to update animation camera view mode */
	virtual void OnSetCameraMode_Implementation(ECameraMode NewMode) { CameraMode = NewMode; };

	/** Called to update animation stance */
	virtual void OnSetStance_Implementation(EStance NewStance);

	/** Called to update animation rotation mode */
	virtual void OnSetRotationMode_Implementation(ERotationMode NewMode) { RotationMode = NewMode; };

	/** Called to update animation locomotion mode */
	virtual void OnSetLocomotionMode_Implementation(ELocomotionMode NewMode);

	/** Called to update animation gait mode*/
	virtual void OnSetGaitMode_Implementation(EGaitMode NewMode) { GaitMode = NewMode; };


	/** Called to update animation character rotation */
	virtual void OnSetCharacterRotation_Implementation(FRotator NewRotation) { CharacterRotation = NewRotation; };

	/** Called to update animation looking rotation */
	virtual void OnSetLookRotation_Implementation(FRotator NewRotation) { LookRotation = NewRotation; };

	/** Called to update animation previous velocity rotation */
	virtual void OnSetPrevVelocityRotation_Implementation(FRotator NewRotation) { PrevVelocityRotation = NewRotation; };

	/** Called to update animation previous movement rotation */
	virtual void OnSetPrevMovementRotation_Implementation(FRotator NewRotation) { PrevMovementRotation = NewRotation; };

	/** Called to update animation yaw differential */
	virtual void OnSetYawDifferential_Implementation(float NewValue) { YawDifferential = NewValue; };

	/** Called to update animation movement differential */
	virtual void OnSetMovementDifferential_Implementation(float NewValue) { MovementDifferential = NewValue; };

	/** Called to update animation rotation differential */
	virtual void OnSetRotationDifferential_Implementation(float NewValue) { RotationDifferential = NewValue; };

	/** Called to update animation aim yaw rate */
	virtual void OnSetAimYawRate_Implementation(float NewValue) { AimYawRate = NewValue; };

	/** Called to update animation aim yaw delta */
	virtual void OnSetAimYawDelta_Implementation(float NewValue) { AimYawDelta = NewValue; };


protected:
	////////////////////////////////////////////////////////////////////
	//
	//   Variables used within the animation blueprint
	//
	////////////////////////////////////////////////////////////////////

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TPBase|AnimGraph|CharacterState")
	bool bIsRagdoll;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TPBase|AnimGraph|CharacterState")
	bool bIsAiming;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TPBase|AnimGraph|CharacterState")
	bool bIsMoving;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TPBase|AnimGraph|CharacterState")
	bool bHasMovementInput;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TPBase|AnimGraph|CharacterState")
	bool bRightFootForward;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TPBase|AnimGraph|CharacterState")
	EStance Stance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TPBase|AnimGraph|CharacterState")
	ELocomotionMode LocomotionMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TPBase|AnimGraph|CharacterState")
	float YawDifferential;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TPBase|AnimGraph|CharacterState")
	float MovementDifferential;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TPBase|AnimGraph|CharacterState")
	float RotationDifferential;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TPBase|AnimGraph|CharacterState")
	FVector CharacterVelocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|AnimGraph|InternalVariables")
	bool bShouldTurnInPlace;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|AnimGraph|InternalVariables")
	EIdleEntryState IdleEntryState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|AnimGraph|InternalVariables")
	ELocomotionMode PrevLocomotionMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|AnimGraph|InternalVariables")
	float Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|AnimGraph|InternalVariables")
	FVector2D AimOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|AnimGraph|InternalVariables")
	FVector2D FeetPosition;


protected:
	////////////////////////////////////////////////////////////////////
	//
	//   Animation blueprint state variables retrieved from character
	//
	////////////////////////////////////////////////////////////////////

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TPBase|CharacterState")
	ATPBaseCharacter* Character;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TPBase|CharacterState")
	UCapsuleComponent* Capsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TPBase|CharacterState")
	UCharacterMovementComponent* MovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TPBase|CharacterState")
	bool bShowTraces;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TPBase|CharacterState")
	float CrouchSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TPBase|CharacterState")
	float WalkSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TPBase|CharacterState")
	float RunSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TPBase|CharacterState")
	float SprintSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TPBase|CharacterState")
	ECameraMode CameraMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TPBase|CharacterState")
	ERotationMode RotationMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TPBase|CharacterState")
	EGaitMode GaitMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TPBase|CharacterState")
	FRotator LookRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TPBase|CharacterState")
	FRotator CharacterRotation;

	UPROPERTY(VisibleAnywhere, Category = "TPBase|CharacterState")
	FRotator PrevMovementRotation;

	UPROPERTY(VisibleAnywhere, Category = "TPBase|CharacterState")
	FRotator PrevVelocityRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TPBase|CharacterState")
	float AimYawRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TPBase|CharacterState")
	float AimYawDelta;


protected:
	////////////////////////////////////////////////////////////////////
	//
	//   Internal animation blueprint variables
	//
	////////////////////////////////////////////////////////////////////

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	ELocomotionState ActiveLocomotionState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	EMovementDirection MovementDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	bool bTurningInPlace;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	bool bTurningRight;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	float TurnInPlaceDelay;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	float GaitValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	float PreviousSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	float StartPosition;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	float N_PlayRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	float C_PlayRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	float FlailRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	float FlailBlendAlpha;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	float LandPredictionAlpha;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	float LeanInAir;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	FVector2D LeanGrounded;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	FRotator PrevVelocityRotationABP;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	FPivotParameters PivotParameters;


public:
	////////////////////////////////////////////////////////////////////
	//
	//   Animation assets
	//
	////////////////////////////////////////////////////////////////////

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TPBase|AnimationAssets")
	FTurnAnimations N_Turn_90;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TPBase|AnimationAssets")
	FTurnAnimations N_Turn_180;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TPBase|AnimationAssets")
	FTurnAnimations LF_Turn_90;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TPBase|AnimationAssets")
	FTurnAnimations RF_Turn_90;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TPBase|AnimationAssets")
	FTurnAnimations CLF_Turn_90;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TPBase|AnimationAssets")
	FTurnAnimations CRF_Turn_90;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TPBase|AnimationAssets")
	UAnimMontage* GetUpBack;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TPBase|AnimationAssets")
	UAnimMontage* GetUpFront;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TPBase|AnimationAssets")
	UAnimSequenceBase* AdditiveLand;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TPBase|AnimationAssets")
	UCurveFloat* FlailAlphaCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TPBase|AnimationAssets")
	UCurveFloat* LandAlphaCurve;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "TPBase|AnimationAssets")
	UAnimMontage* ActiveTurningMontage; 

};
