// Copyright is Bullshit!  Do as you will with these files.

#pragma once

#include "TPBase.h"
#include "TPBaseEnum.h"
#include "UObject/Interface.h"
#include "TPBaseInterfaceABP.generated.h"



/**
 * This class does not need to be modified.
 */
UINTERFACE(MinimalAPI)
class UTPBaseInterfaceABP : public UInterface {

	GENERATED_BODY()
};


/**
 *  Defines the interface between the character and animation blueprint.
 *  These functions must be overridden by animation instance class and should 
 *  be called each frame by the implementing class to provide the needed
 *  information for the animation blueprint.  All over-ridable in blueprint.
 */
class TPBASE_API ITPBaseInterfaceABP {

	GENERATED_BODY()

public: 

	/** Called to set display debug traces */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|ABP_Interface")
	void OnSetShowTraces(bool bShow);

	/** Called to update animation ragdoll state */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|ABP_Interface")
	void OnSetIsRagdoll(bool bRagdoll);

	/** Called to update animation aiming state */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|ABP_Interface")
	void OnSetIsAiming(bool bAiming);

	/** Called to update animation moving state */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|ABP_Interface")
	void OnSetIsMoving(bool bMoving);

	/** Called to update animation movement input state */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|ABP_Interface")
	void OnSetHasMovementInput(bool bHasInput);

	/** Called to update animation forward foot state */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|ABP_Interface")
	void OnSetForwardFoot(bool bRightFoot);


	/** Called to update animation crouching speed */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|ABP_Interface")
	void OnSetCrouchSpeed(float NewSpeed);

	/** Called to update animation walking speed */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|ABP_Interface")
	void OnSetWalkSpeed(float NewSpeed);

	/** Called to update animation running speed */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|ABP_Interface")
	void OnSetRunSpeed(float NewSpeed);

	/** Called to update animation sprinting speed */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|ABP_Interface")
	void OnSetSprintSpeed(float NewSpeed);

	/** Called to update animation character velocity */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|ABP_Interface")
	void OnSetCharacterVelocity(FVector NewVelocity);


	/** Called to update animation camera view mode */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|ABP_Interface")
	void OnSetCameraMode(ECameraMode NewMode);

	/** Called to update animation stance */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|ABP_Interface")
	void OnSetStance(EStance NewStance);

	/** Called to update animation rotation mode */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|ABP_Interface")
	void OnSetRotationMode(ERotationMode NewMode);

	/** Called to update animation locomotion mode */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|ABP_Interface")
	void OnSetLocomotionMode(ELocomotionMode NewMode);

	/** Called to update animation gait mode */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|ABP_Interface")
	void OnSetGaitMode(EGaitMode NewMode);


	/** Called to update animation character rotation */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|ABP_Interface")
	void OnSetCharacterRotation(FRotator NewRotation);

	/** Called to update animation looking rotation */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|ABP_Interface")
	void OnSetLookRotation(FRotator NewRotation);

	/** Called to update animation previous velocity rotation */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|ABP_Interface")
	void OnSetPrevVelocityRotation(FRotator NewRotation);

	/** Called to update animation previous movement rotation */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|ABP_Interface")
	void OnSetPrevMovementRotation(FRotator NewRotation);

	/** Called to update animation yaw differential */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|ABP_Interface")
	void OnSetYawDifferential(float NewValue);

	/** Called to update animation movement differential */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|ABP_Interface")
	void OnSetMovementDifferential(float NewValue);

	/** Called to update animation rotation differential */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|ABP_Interface")
	void OnSetRotationDifferential(float NewValue);

	/** Called to update animation aim yaw rate */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|ABP_Interface")
	void OnSetAimYawRate(float NewValue);

	/** Called to update animation aim yaw delta */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|ABP_Interface")
	void OnSetAimYawDelta(float NewValue);

};

