// Copyright is Bullshit!  Do as you will with these files.

#pragma once

#include "TPBase.h"
#include "TPBaseEnum.h"

#include <GameFramework/CharacterMovementComponent.h>
#include "TPBaseMovement.generated.h"

class FNetworkPredictionData_Client;
class FNetworkPredictionData_Server;

 

/**
 *  Implements a custom character movement component
 */
UCLASS()
class TPBASE_API UTPBaseMovement : public UCharacterMovementComponent {

	GENERATED_BODY()

	friend class FSavedMove_TPBaseCharacter;

public:

	/** Base constructor */
	UTPBaseMovement(const FObjectInitializer& ObjectInitializer);

	///////////////////////////////////////////////////////////////////////////////////////
	//
	//  Parent override functions
	//
	///////////////////////////////////////////////////////////////////////////////////////

	/** Called every frame */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Called for property replication */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	/** Set input flags on character from saved inputs - Client only */
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;

	/** Client prediction data */
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

	/** Server prediction data */
	virtual FNetworkPredictionData_Server* GetPredictionData_Server() const override;

	/** Stores movement input before consuming */
	virtual FVector ConsumeInputVector() override;

	/** Max speed based on state */
	virtual float GetMaxSpeed() const override;
	
	/** Max acceleration based on state */
	virtual float GetMaxAcceleration() const override;
	
	/** Max deceleration based on state */
	virtual float GetMaxBrakingDeceleration() const override;


	///////////////////////////////////////////////////////////////////////////////////////
	//
	//  Getter functions
	//
	///////////////////////////////////////////////////////////////////////////////////////
	UFUNCTION(BlueprintCallable, Category = "TPBase")
	bool IsRunning() const { return bWantsToRun; }

	UFUNCTION(BlueprintCallable, Category = "TPBase")
	bool IsSprinting() const { return bWantsToSprint; }

	UFUNCTION(BlueprintCallable, Category = "TPBase")
	bool IsAiming() const { return bWantsToAim; }

	UFUNCTION(BlueprintCallable, Category = "TPBase")
	FVector GetMovementInput() const { return MovementInput; }
	
	UFUNCTION(BlueprintCallable, Category = "TPBase")
	FRotator GetControlRotation() const { return ControlRotation; };

	/** Crouch speed configuration input */
	UFUNCTION(BlueprintCallable, Category = "TPBase")
	float GetCrouchSpeed() const { return CrouchSpeed; };

	/** Walk speed configuration input */
	UFUNCTION(BlueprintCallable, Category = "TPBase")
	float GetWalkSpeed() const { return WalkSpeed; };

	/** Run speed configuration input */
	UFUNCTION(BlueprintCallable, Category = "TPBase")
	float GetRunSpeed() const { return RunSpeed; };

	/** Sprint speed configuration input */
	UFUNCTION(BlueprintCallable, Category = "TPBase")
	float GetSprintSpeed() const { return SprintSpeed; };

	/** Walk acceleration configuration input */
	UFUNCTION(BlueprintCallable, Category = "TPBase")
	float GetWalkAcceleration() const { return WalkAcceleration; };

	/** Run acceleration configuration input */
	UFUNCTION(BlueprintCallable, Category = "TPBase")
	float GetRunAcceleration() const { return RunAcceleration; };

	/** Walk deceleration configuration input */
	UFUNCTION(BlueprintCallable, Category = "TPBase")
	float GetWalkDeceleration() const { return WalkDeceleration; };

	/** Run deceleration configuration input */
	UFUNCTION(BlueprintCallable, Category = "TPBase")
	float GetRunDeceleration() const { return RunDeceleration; };

	/** Walk ground friction configuration input */
	UFUNCTION(BlueprintCallable, Category = "TPBase")
	float GetWalkGroundFriction() const { return WalkGroundFriction; };

	/** Run ground friction configuration input */
	UFUNCTION(BlueprintCallable, Category = "TPBase")
	float GetRunGroundFriction() const { return RunGroundFriction; };

	/** Speed determined by gait, locomotion mode not considered */
	UFUNCTION(BlueprintCallable, Category = "TPBase")
	float GetGroundSpeed() const;

	/** Crouched speed based on state */
	UFUNCTION(BlueprintCallable, Category = "TPBase")
	float GetCrouchedSpeed() const;
	
	/** Friction based on state */
	UFUNCTION(BlueprintCallable, Category = "TPBase")
	float GetGroundFriction() const;

	/** Stance of character: TODO: Remove, just use IsCrouching */
	UFUNCTION(BlueprintCallable, Category = "TPBase")
	EStance GetStance() const;

	/** Current gait mode based on state*/
	UFUNCTION(BlueprintCallable, Category = "TPBase")
	EGaitMode GetGaitMode() const;

	/** Current locomotion mode based on state*/
	UFUNCTION(BlueprintCallable, Category = "TPBase")
	ELocomotionMode GetLocomotionMode() const;

	/** Current locomotion mode based on state*/
	UFUNCTION(BlueprintCallable, Category = "TPBase")
	ELocomotionMode ConvertMovementMode(EMovementMode Mode) const;

	UFUNCTION(BlueprintCallable, Category = "TPBase")
	ERotationMode GetRotationMode() const { return RotationMode; };


public:
	///////////////////////////////////////////////////////////////////////////////////////
	//
	//  Networked setter functions
	//
	///////////////////////////////////////////////////////////////////////////////////////

	UFUNCTION(BlueprintCallable, Category = "TPBase")
	void WantsToRun(bool bRunning) { bWantsToRun = bRunning; };

	UFUNCTION(BlueprintCallable, Category = "TPBase")
	void WantsToSprint(bool bSprinting) { bWantsToSprint = bSprinting; };

	UFUNCTION(BlueprintCallable, Category = "TPBase")
	void WantsToAim(bool bAiming) { bWantsToAim = bAiming; };

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetCrouchSpeed(float NewValue);

	UFUNCTION(BlueprintCallable, Category = "TPBase")
	void SetCrouchSpeed(float NewValue);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetWalkSpeed(float NewValue);

	UFUNCTION(BlueprintCallable, Category = "TPBase")
	void SetWalkSpeed(float NewValue);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetRunSpeed(float NewValue);

	UFUNCTION(BlueprintCallable, Category = "TPBase")
	void SetRunSpeed(float NewValue);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetSprintSpeed(float NewValue);

	UFUNCTION(BlueprintCallable, Category = "TPBase")
	void SetSprintSpeed(float NewValue);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetWalkAcceleration(float NewValue);

	UFUNCTION(BlueprintCallable, Category = "TPBase")
	void SetWalkAcceleration(float NewValue);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetRunAcceleration(float NewValue);

	UFUNCTION(BlueprintCallable, Category = "TPBase")
	void SetRunAcceleration(float NewValue);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetWalkDeceleration(float NewValue);

	UFUNCTION(BlueprintCallable, Category = "TPBase")
	void SetWalkDeceleration(float NewValue);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetRunDeceleration(float NewValue);

	UFUNCTION(BlueprintCallable, Category = "TPBase")
	void SetRunDeceleration(float NewValue);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetWalkGroundFriction(float NewValue);

	UFUNCTION(BlueprintCallable, Category = "TPBase")
	void SetWalkGroundFriction(float NewValue);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetRunGroundFriction(float NewValue);

	UFUNCTION(BlueprintCallable, Category = "TPBase")
	void SetRunGroundFriction(float NewValue);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetRotationMode(ERotationMode NewMode);

	UFUNCTION(BlueprintCallable, Category = "TPBase")
	void SetRotationMode(ERotationMode NewMode);

	UFUNCTION(BlueprintCallable, Category = "TPBase")
	void SetCustomAcceleration();



	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: TPBase")
	float BaseLookUpRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: TPBase")
	float BaseTurnRate;


protected:
	///////////////////////////////////////////////////////////////////////////////////////
	//
	//  Utility functions
	//
	///////////////////////////////////////////////////////////////////////////////////////

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetInputVector(const FVector& Input);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetControlRotation();


protected:
	////////////////////////////////////////////////////////////////////
	//
	//   User configuration variables
	//
	////////////////////////////////////////////////////////////////////

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: TPBase")
	float CrouchSpeed;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: TPBase")
	float WalkSpeed;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: TPBase")
	float RunSpeed;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: TPBase")
	float SprintSpeed;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: TPBase")
	float WalkAcceleration;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: TPBase")
	float RunAcceleration;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: TPBase")
	float WalkDeceleration;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: TPBase")
	float RunDeceleration;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: TPBase")
	float WalkGroundFriction;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: TPBase")
	float RunGroundFriction;


protected:
	////////////////////////////////////////////////////////////////////
	//
	//   Internal state variables
	//
	////////////////////////////////////////////////////////////////////

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement: TPBase|Internal")
	FVector_NetQuantize100 MovementInput;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement: TPBase|Internal")
	FRotator ControlRotation;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement: TPBase|Internal")
	ERotationMode RotationMode;

private:

	/** Custom state flags */
	UPROPERTY(Replicated)
	uint8 bWantsToRun : 1;

	UPROPERTY(Replicated)
	uint8 bWantsToSprint : 1;

	UPROPERTY(Replicated)
	uint8 bWantsToAim : 1;

};



/** 
 *  Implements custom saved move class
 */
class FSavedMove_TPBaseCharacter : public FSavedMove_Character {

public:

	typedef FSavedMove_Character Super;

	/** Resets all saved variables */
	virtual void Clear() override;

	/** Store input commands in the compressed flags */
	virtual uint8 GetCompressedFlags() const override;

	/** This is used to check whether or not two moves can be combined into one
	    Basically you just check to make sure that the saved variables are the same */
	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const override;

	/** Sets up the move before sending it to the server */
	virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;
	
	/** Sets variables on character movement component before making a predictive correction */
	virtual void PrepMoveFor(class ACharacter* Character) override;

	uint8 bSavedWantsToRun : 1;
	uint8 bSavedWantsToSprint : 1;
	uint8 bSavedWantsToAim : 1;
};



/**
 *  Implements custom client prediction data class
 */
class FNetworkPredictionData_Client_TPBaseCharacter : public FNetworkPredictionData_Client_Character {

public:

	typedef FNetworkPredictionData_Client_Character Super;

	FNetworkPredictionData_Client_TPBaseCharacter(const UCharacterMovementComponent& ClientMovement);

	virtual FSavedMovePtr AllocateNewMove() override;
};



/**
 *  Implements cusstom server prediction data class
 */
class FNetworkPredictionData_Server_TPBaseCharacter : public FNetworkPredictionData_Server_Character {

public:

	typedef FNetworkPredictionData_Server_Character Super;

	FNetworkPredictionData_Server_TPBaseCharacter(const UCharacterMovementComponent& ServerMovement);
};

