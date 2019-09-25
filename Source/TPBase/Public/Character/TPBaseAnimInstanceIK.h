// Copyright is Bullshit!  Do as you will with these files.

#pragma once

#include "TPBase.h"
#include "TPBaseInterfaceABPIK.h"

#include <Animation/AnimInstance.h>
#include "TPBaseAnimInstanceIK.generated.h"

class ATPBaseCharacter;



/**
 *  Implements interface between character and IK animation blueprint
 */
UCLASS()
class TPBASE_API UTPBaseAnimInstanceIK : public UAnimInstance, public ITPBaseInterfaceABPIK {

	GENERATED_BODY()

public:
	
	/** Setup default properties */
	UTPBaseAnimInstanceIK();
	
	/** Called at start of play */
	virtual void NativeInitializeAnimation() override;

	/** Called every frame */
	virtual void NativeUpdateAnimation(float DeltaTimeX) override;


protected:
	////////////////////////////////////////////////////////////////////
	//
	//   Inverse kinematic functions, can be overriden in blueprint or C++
	//
	////////////////////////////////////////////////////////////////////
	
	/** Handles IK for normal modes */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|Animation")
	void FootIK();
	virtual void FootIK_Implementation();

	/** Handles IK when ragdolling */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|Animation")
	void RagdollIK();
	virtual void RagdollIK_Implementation();


public:
	////////////////////////////////////////////////////////////////////
	//
	//   Interface functions to needed character variables
	//
	////////////////////////////////////////////////////////////////////

	/** Calls all interface functions to synchronize animation state with character*/
	UFUNCTION(BlueprintCallable, Category = "TPBase|ABPIK_Interface")
	void UpdateAnimationState();

	/** Called to update animation debug traces state */
	virtual void OnSetShowTraces_Implementation(bool bShow) { bShowTraces = bShow; };

	/** Called to update animation debug traces state */
	virtual void OnSetIsRagdoll_Implementation(bool bRagdoll) { bIsRagdoll = bRagdoll; };

	/** Called to update animation stance */
	virtual void OnSetStance_Implementation(EStance NewStance) { Stance = NewStance; };

	/** Called to update animation locomotion mode */
	virtual void OnSetLocomotionMode_Implementation(ELocomotionMode NewMode) { LocomotionMode = NewMode; };


protected:
	////////////////////////////////////////////////////////////////////
	//
	//   User variables
	//
	////////////////////////////////////////////////////////////////////
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TPBase")
	ATPBaseCharacter* Character;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TPBase")
	FName LeftFootBoneName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TPBase")
	FName RightFootBoneName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "TPBase")
	float FootIKAlpha;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "TPBase")
	float RotationInterpolationSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "TPBase")
	float Z_InterpolationSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "TPBase|Linetrace")
	float TraceHeightAboveFoot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "TPBase|Linetrace")
	float TraceHeightBelowFoot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "TPBase|Limits")
	FVector StandingMinLimits;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "TPBase|Limits")
	FVector StandingMaxLimits;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "TPBase|Limits")
	FVector CrouchingMinLimits;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "TPBase|Limits")
	FVector CrouchingMaxLimits;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|CharacterState")
	bool bShowTraces;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|CharacterState")
	bool bIsRagdoll;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|CharacterState")
	EStance Stance;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|CharacterState")
	ELocomotionMode LocomotionMode;


protected:
	////////////////////////////////////////////////////////////////////
	//
	//   Internal animation blueprint variables
	//
	////////////////////////////////////////////////////////////////////

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	bool bEnableFootIK;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	float PelvisOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	FVector LeftFootOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	FVector RightFootOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	FTransform LeftFootTransform;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TPBase|InternalVariables")
	FTransform RightFootTransform;

};
