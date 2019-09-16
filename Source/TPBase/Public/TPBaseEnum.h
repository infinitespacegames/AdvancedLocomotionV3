// Copyright is Bullshit!  Do as you will with these files.

#pragma once

#include "TPBaseEnum.generated.h"



/**
 * Enumeration for compass main cardinal directions
 */
UENUM(BlueprintType)
enum class ECardinalDirection : uint8 {
	eNorth UMETA(DisplayName = "North"),
	eEast UMETA(DisplayName = "East"),
	eSouth UMETA(DisplayName = "South"),
	eWest UMETA(DisplayName = "West")
};


/**
 * Enumeration for character stance
 */
UENUM(BlueprintType)
enum class EStance : uint8 {
	eStanding UMETA(DisplayName = "Standing"),
	eCrouching UMETA(DisplayName = "Crouching")
};


/**
 * Enumeration for camera view mode
 */
UENUM(BlueprintType)
enum class ECameraMode : uint8 {
	eFirstPerson UMETA(DisplayName = "FirstPerson"),
	eThirdPerson UMETA(DisplayName = "ThirdPerson")
};


/**
 * Enumeration for character rotation mode
 */
UENUM(BlueprintType)
enum class ERotationMode : uint8 {
	eVelocityMode UMETA(DisplayName = "VelocityMode"),
	eLookingMode UMETA(DisplayName = "LookingMode")
};


/**
 * Enumeration for character gait mode
 */
UENUM(BlueprintType)
enum class EGaitMode : uint8 {
	eNone UMETA(DisplayName = "None"),
	eWalking UMETA(DisplayName = "Walking"),
	eRunning UMETA(DisplayName = "Running"),
	eSprinting UMETA(DisplayName = "Sprinting")
};


/**
 * Enumeration for character locomotion mode
 */
UENUM(BlueprintType)
enum class ELocomotionMode : uint8 {
	eNone UMETA(DisplayName = "None"),
	eGrounded UMETA(DisplayName = "Grounded"),
	eFalling UMETA(DisplayName = "Falling"),
	eRagdoll UMETA(DisplayName = "Ragdoll")
};


/**
 * Enumeration for character locomotion state
 */
UENUM(BlueprintType)
enum class ELocomotionState : uint8 {
	eNone UMETA(DisplayName = "None"),
	eNotMoving UMETA(DisplayName = "NotMoving"),
	eMoving UMETA(DisplayName = "Moving"),
	ePivot UMETA(DisplayName = "Pivot"),
	eStopping UMETA(DisplayName = "Stopping")
};


/**
  Enumeration for idle entry state
*/
UENUM(BlueprintType)
enum class EIdleEntryState : uint8 {
	eN_Idle UMETA(DisplayName = "N_Idle"),
	eLF_Idle UMETA(DisplayName = "LF_Idle"),
	eRF_Idle UMETA(DisplayName = "RF_Idle"),
	eCLF_Idle UMETA(DisplayName = "CLF_Idle"),
	eCRF_Idle UMETA(DisplayName = "CRF_Idle")
};


/**
 * Enumeration for character movement
 */
UENUM(BlueprintType)
enum class EMovementDirection : uint8 {
	eForward UMETA(DisplayName = "Forward"),
	eBackward UMETA(DisplayName = "Backward")
};


/**
 * Enumeration for character footstep type
 */
UENUM(BlueprintType)
enum class EFootstepType : uint8 {
	eStep UMETA(DisplayName = "Stepping"),
	eWalk UMETA(DisplayName = "Walking"),
	eRun UMETA(DisplayName = "Running"),
	eSprint UMETA(DisplayName = "Sprinting"),
	ePivot UMETA(DisplayName = "Pivoting"),
	eJump UMETA(DisplayName = "Jumping"),
	eLand UMETA(DisplayName = "Landing")
};

