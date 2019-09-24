// Copyright is Bullshit!  Do as you will with these files.

#include "TPBaseMovement.h"
#include <GameFramework/Character.h>
#include <UnrealNetwork.h>



// Base constructor
UTPBaseMovement::UTPBaseMovement(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {

	// Set this character to call Tick() every frame.
	PrimaryComponentTick.bCanEverTick = true;

	bReplicates = true;

	BaseTurnRate = 150.0f;
	BaseLookUpRate = 150.0f;
	CrouchSpeed = 150.0f;
	WalkSpeed = 165.0;
	RunSpeed = 380.0f;
	SprintSpeed = 650.0f;
	WalkAcceleration = 800.0f;
	RunAcceleration = 1000.0f;
	WalkDeceleration = 800.0f;
	RunDeceleration = 800.0f;
	WalkGroundFriction = 8.0f;
	RunGroundFriction = 6.0f;
	RotationMode = ERotationMode::eLookingMode;

}


// Called every frame
void UTPBaseMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PawnOwner && PawnOwner->IsLocallyControlled()) {
		ServerSetControlRotation();
	}

	// Update based on state
	MaxWalkSpeedCrouched = GetCrouchedSpeed();
	GroundFriction = GetGroundFriction();

	// Custom acceleration based on movement differential
	if (IsMovingOnGround() && !IsCrouching()) {
		if (IsRunning() || IsSprinting()) {
			SetCustomAcceleration();
		}
	}
}


// Called for property replication 
void UTPBaseMovement::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	DOREPLIFETIME(UTPBaseMovement, bWantsToRun);
	DOREPLIFETIME(UTPBaseMovement, bWantsToSprint);
	DOREPLIFETIME(UTPBaseMovement, bWantsToAim);
	DOREPLIFETIME(UTPBaseMovement, MovementInput);
	DOREPLIFETIME(UTPBaseMovement, ControlRotation);
	DOREPLIFETIME(UTPBaseMovement, CrouchSpeed);
	DOREPLIFETIME(UTPBaseMovement, WalkSpeed);
	DOREPLIFETIME(UTPBaseMovement, RunSpeed);
	DOREPLIFETIME(UTPBaseMovement, SprintSpeed);
	DOREPLIFETIME(UTPBaseMovement, WalkAcceleration);
	DOREPLIFETIME(UTPBaseMovement, RunAcceleration);
	DOREPLIFETIME(UTPBaseMovement, WalkDeceleration);
	DOREPLIFETIME(UTPBaseMovement, RunDeceleration);
	DOREPLIFETIME(UTPBaseMovement, WalkGroundFriction);
	DOREPLIFETIME(UTPBaseMovement, RunGroundFriction);
	DOREPLIFETIME(UTPBaseMovement, RotationMode);
}


// Set input flags on character from saved inputs - Client only
void UTPBaseMovement::UpdateFromCompressedFlags(uint8 Flags) {
	Super::UpdateFromCompressedFlags(Flags);

	//The Flags parameter contains the compressed input flags that are stored in the saved move.
	//UpdateFromCompressed flags simply copies the flags from the saved move into the movement component.
	//It basically just resets the movement component to the state when the move was made so it can simulate from there.

	bWantsToRun = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
	bWantsToSprint = (Flags & FSavedMove_Character::FLAG_Custom_1) != 0;
	bWantsToAim = (Flags & FSavedMove_Character::FLAG_Custom_2) != 0;
}


// Client prediction data
FNetworkPredictionData_Client* UTPBaseMovement::GetPredictionData_Client() const {
	//check(PawnOwner != NULL);

	if (!ClientPredictionData) {
		UTPBaseMovement* MutableThis = const_cast<UTPBaseMovement*>(this);

		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_TPBaseCharacter(*this);
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
	}
	return ClientPredictionData;
}


// Server prediction data
FNetworkPredictionData_Server* UTPBaseMovement::GetPredictionData_Server() const {
	//check(PawnOwner != NULL);

	if (!ServerPredictionData) {
		UTPBaseMovement* MutableThis = const_cast<UTPBaseMovement*>(this);
		MutableThis->ServerPredictionData = new FNetworkPredictionData_Server_TPBaseCharacter(*this);
	}
	return ServerPredictionData;
}


// Stores movement input before consuming 
FVector UTPBaseMovement::ConsumeInputVector() {
	FVector Input = Super::ConsumeInputVector();

	if (PawnOwner && PawnOwner->IsLocallyControlled()) {
		ServerSetInputVector(Input);
	}

	return Input;
}


// Get max speed based on state 
float UTPBaseMovement::GetMaxSpeed() const {
	if (!IsMovingOnGround()) { Super::GetMaxSpeed(); }
	return GetGroundSpeed();
}


// Max acceleration based on state
float UTPBaseMovement::GetMaxAcceleration() const {
	if (!IsMovingOnGround()) { return Super::GetMaxAcceleration(); }
	if (IsRunning() || IsSprinting()) { return RunAcceleration; }
	return WalkAcceleration;
}


// Max deceleration based on state 
float UTPBaseMovement::GetMaxBrakingDeceleration() const {
	if (!IsMovingOnGround()) { return Super::GetMaxBrakingDeceleration(); }
	if (IsRunning() || IsSprinting()) { return RunDeceleration; }
	return WalkDeceleration;
}



///////////////////////////////////////////////////////////////////////////////////////
//
//  Getter functions
//
///////////////////////////////////////////////////////////////////////////////////////

// 	Speed determined by gait, locomotion mode not considered 
float UTPBaseMovement::GetGroundSpeed() const {
	
	if (IsCrouching()) { return GetCrouchedSpeed(); }
	
	if (IsAiming()) {
		if (IsSprinting()) { return RunSpeed; }
		return WalkSpeed;
	}
	
	if (IsSprinting()) { 
		if (RotationMode == ERotationMode::eVelocityMode) { return SprintSpeed; }
		return FMath::Abs((MovementInput.Rotation() - ControlRotation).GetNormalized().Yaw) > 50.0f ? RunSpeed : SprintSpeed;
	}
	
	if (IsRunning()) { return RunSpeed; }
	return WalkSpeed;
}


// Get character crouched speed based on state
float UTPBaseMovement::GetCrouchedSpeed() const {
	if (IsSprinting()) { return CrouchSpeed + 50.0f; }
	if (IsRunning()) { return CrouchSpeed; }
	return CrouchSpeed - 50.0f;
}
 

// Get character deceleration based on state
float UTPBaseMovement::GetGroundFriction() const {
	if (IsRunning() || IsSprinting()) { return RunGroundFriction; }
	return WalkGroundFriction;
}


// Stance of character, depreciate
EStance UTPBaseMovement::GetStance() const {
	if (IsCrouching()) { return EStance::eCrouching; }
	return EStance::eStanding;
}


// Get current gait mode based on state
EGaitMode UTPBaseMovement::GetGaitMode() const {
	if (IsSprinting()) { return EGaitMode::eSprinting; }
	if (IsRunning()) { return EGaitMode::eRunning; }
	return EGaitMode::eWalking;
}


// Current locomotion mode based on state 
ELocomotionMode UTPBaseMovement::GetLocomotionMode() const {
	
	// UE4 Movement mode conversion
	switch (MovementMode) {
	case EMovementMode::MOVE_None:
	case EMovementMode::MOVE_Flying:
	case EMovementMode::MOVE_Swimming:
	case EMovementMode::MOVE_Custom:
		break;
	case EMovementMode::MOVE_Walking:
	case EMovementMode::MOVE_NavWalking:
		return ELocomotionMode::eGrounded;
	case EMovementMode::MOVE_Falling:
		return ELocomotionMode::eFalling;
	}
	return ELocomotionMode::eNone;
}


ELocomotionMode UTPBaseMovement::ConvertMovementMode(EMovementMode Mode) const {

	// UE4 Movement mode conversion
	switch (Mode) {
	case EMovementMode::MOVE_None:
	case EMovementMode::MOVE_Flying:
	case EMovementMode::MOVE_Swimming:
	case EMovementMode::MOVE_Custom:
		break;
	case EMovementMode::MOVE_Walking:
	case EMovementMode::MOVE_NavWalking:
		return ELocomotionMode::eGrounded;
	case EMovementMode::MOVE_Falling:
		return ELocomotionMode::eFalling;
	}
	return ELocomotionMode::eNone;
}



///////////////////////////////////////////////////////////////////////////////////////
//
//  Networked setter functions
//
///////////////////////////////////////////////////////////////////////////////////////

// Crouch speed
void UTPBaseMovement::ServerSetCrouchSpeed_Implementation(float NewValue) {
	CrouchSpeed = NewValue;
}


bool UTPBaseMovement::ServerSetCrouchSpeed_Validate(float NewValue) {
	return true;
}


void UTPBaseMovement::SetCrouchSpeed(float NewValue) {
	if (PawnOwner && PawnOwner->IsLocallyControlled()) {
		ServerSetCrouchSpeed(NewValue);
	}
}


// Walk speed
void UTPBaseMovement::ServerSetWalkSpeed_Implementation(float NewValue) {
	WalkSpeed = NewValue;
}


bool UTPBaseMovement::ServerSetWalkSpeed_Validate(float NewValue) {
	return true;
}


void UTPBaseMovement::SetWalkSpeed(float NewValue) {
	if (PawnOwner && PawnOwner->IsLocallyControlled()) {
		ServerSetWalkSpeed(NewValue);
	}
}


// Run speed
void UTPBaseMovement::ServerSetRunSpeed_Implementation(float NewValue) {
	RunSpeed = NewValue;
}


bool UTPBaseMovement::ServerSetRunSpeed_Validate(float NewValue) {
	return true;
}


void UTPBaseMovement::SetRunSpeed(float NewValue) {
	if (PawnOwner && PawnOwner->IsLocallyControlled()) {
		ServerSetRunSpeed(NewValue);
	}
}


// Sprint speed
void UTPBaseMovement::ServerSetSprintSpeed_Implementation(float NewValue) {
	SprintSpeed = NewValue;
}


bool UTPBaseMovement::ServerSetSprintSpeed_Validate(float NewValue) {
	return true;
}


void UTPBaseMovement::SetSprintSpeed(float NewValue) {
	if (PawnOwner && PawnOwner->IsLocallyControlled()) {
		ServerSetSprintSpeed(NewValue);
	}
}


// Walk acceleration
void UTPBaseMovement::ServerSetWalkAcceleration_Implementation(float NewValue) {
	WalkAcceleration = NewValue;
}


bool UTPBaseMovement::ServerSetWalkAcceleration_Validate(float NewValue) {
	return true;
}


void UTPBaseMovement::SetWalkAcceleration(float NewValue) {
	if (PawnOwner && PawnOwner->IsLocallyControlled()) {
		ServerSetWalkAcceleration(NewValue);
	}
}


// Run acceleration
void UTPBaseMovement::ServerSetRunAcceleration_Implementation(float NewValue) {
	RunAcceleration = NewValue;
}


bool UTPBaseMovement::ServerSetRunAcceleration_Validate(float NewValue) {
	return true;
}


void UTPBaseMovement::SetRunAcceleration(float NewValue) {
	if (PawnOwner && PawnOwner->IsLocallyControlled()) {
		ServerSetRunAcceleration(NewValue);
	}
}


// Walk deceleration
void UTPBaseMovement::ServerSetWalkDeceleration_Implementation(float NewValue) {
	WalkDeceleration = NewValue;
}


bool UTPBaseMovement::ServerSetWalkDeceleration_Validate(float NewValue){
	return true;
}


void UTPBaseMovement::SetWalkDeceleration(float NewValue) {
	if (PawnOwner && PawnOwner->IsLocallyControlled()) {
		ServerSetWalkDeceleration(NewValue);
	}
}


// Run deceleration
void UTPBaseMovement::ServerSetRunDeceleration_Implementation(float NewValue) {
	RunDeceleration = NewValue;
}


bool UTPBaseMovement::ServerSetRunDeceleration_Validate(float NewValue) {
	return true;

}


void UTPBaseMovement::SetRunDeceleration(float NewValue) {
	if (PawnOwner && PawnOwner->IsLocallyControlled()) {
		ServerSetRunDeceleration(NewValue);
	}
}


// Walk ground friction
void UTPBaseMovement::ServerSetWalkGroundFriction_Implementation(float NewValue) {
	WalkGroundFriction = NewValue;
}


bool UTPBaseMovement::ServerSetWalkGroundFriction_Validate(float NewValue) {
	return true;
}


void UTPBaseMovement::SetWalkGroundFriction(float NewValue) {
	if (PawnOwner && PawnOwner->IsLocallyControlled()) {
		ServerSetWalkGroundFriction(NewValue);
	}
}


// Run ground friction
void UTPBaseMovement::ServerSetRunGroundFriction_Implementation(float NewValue) {
	RunGroundFriction = NewValue;
}


bool UTPBaseMovement::ServerSetRunGroundFriction_Validate(float NewValue) {
	return true;
}


void UTPBaseMovement::SetRunGroundFriction(float NewValue) {
	if (PawnOwner && PawnOwner->IsLocallyControlled()) {
		ServerSetRunGroundFriction(NewValue);
	}
}


// Get custom acceleration based on input and velocity for pivot 
void UTPBaseMovement::SetCustomAcceleration() {

	float MovementDifferential = (MovementInput.Rotation() - Velocity.Rotation()).GetNormalized().Yaw;
	float Mult1 = FMath::GetMappedRangeValueClamped(FVector2D(45.0f, 130.0f), FVector2D(1.0, 0.2), FMath::Abs(MovementDifferential));
	float Mult2 = FMath::GetMappedRangeValueClamped(FVector2D(45.0f, 130.0f), FVector2D(1.0, 0.4), FMath::Abs(MovementDifferential));

	MaxAcceleration = GetRunAcceleration() * Mult1;
	GroundFriction = GetRunGroundFriction() * Mult2;
}


void UTPBaseMovement::ServerSetRotationMode_Implementation(ERotationMode NewMode) {
	RotationMode = NewMode;
}


bool UTPBaseMovement::ServerSetRotationMode_Validate(ERotationMode NewMode) {
	return true;
}


void UTPBaseMovement::SetRotationMode(ERotationMode NewMode) {
	if (PawnOwner && PawnOwner->IsLocallyControlled()) {
		ServerSetRotationMode(NewMode);
	}
}



///////////////////////////////////////////////////////////////////////////////////////
//
//  Utility functions
//
///////////////////////////////////////////////////////////////////////////////////////

// Store current control rotation
bool UTPBaseMovement::ServerSetControlRotation_Validate() {
	return true;
}


void UTPBaseMovement::ServerSetControlRotation_Implementation() {
	ControlRotation = PawnOwner->GetControlRotation().GetNormalized();
}


// Store current movement input
bool UTPBaseMovement::ServerSetInputVector_Validate(const FVector& Input) {
	return true;
}


void UTPBaseMovement::ServerSetInputVector_Implementation(const FVector& Input) {
	MovementInput = Input;
}



///////////////////////////////////////////////////////////////////////////////////////
//
//  FSavedMove_TPBaseCharacter functions
//
///////////////////////////////////////////////////////////////////////////////////////

// Resets all saved variables
void FSavedMove_TPBaseCharacter::Clear() {
	Super::Clear();

	//Clear variables back to their default values.
	bSavedWantsToRun = false;
	bSavedWantsToSprint = false;
	bSavedWantsToAim = false;
}


// Store input commands in the compressed flags 
uint8 FSavedMove_TPBaseCharacter::GetCompressedFlags() const {
	uint8 Result = Super::GetCompressedFlags();

	if (bSavedWantsToRun) {
		Result |= FLAG_Custom_0;
	}

	if (bSavedWantsToSprint) {
		Result |= FLAG_Custom_1;
	}

	if (bSavedWantsToAim) {
		Result |= FLAG_Custom_2;
	}

	return Result;
}


// This is used to check whether or not two moves can be combined into one
bool FSavedMove_TPBaseCharacter::CanCombineWith(const FSavedMovePtr & NewMove, ACharacter * Character, float MaxDelta) const {
	
	//This pretty much just tells the engine if it can optimize by combining saved moves. 
	//There doesn't appear to be any problem with leaving it out, but it seems that it's good practice to implement this anyways.

	if (bSavedWantsToRun != ((FSavedMove_TPBaseCharacter*)& NewMove)->bSavedWantsToRun) {
		return false;
	}

	if (bSavedWantsToSprint != ((FSavedMove_TPBaseCharacter*)& NewMove)->bSavedWantsToSprint) {
		return false;
	}

	if (bSavedWantsToAim != ((FSavedMove_TPBaseCharacter*)& NewMove)->bSavedWantsToAim) {
		return false;
	}

	return Super::CanCombineWith(NewMove, Character, MaxDelta);
}


// Sets up the move before sending it to the server 
void FSavedMove_TPBaseCharacter::SetMoveFor(ACharacter * Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) {
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	// Take the input from the player and store it in the saved move.
	UTPBaseMovement* CharMov = Cast<UTPBaseMovement>(Character->GetCharacterMovement());
	if (CharMov) {
		bSavedWantsToRun = CharMov->bWantsToRun;
		bSavedWantsToSprint = CharMov->bWantsToSprint;
		bSavedWantsToAim = CharMov->bWantsToAim;
	}
}


// Sets variables on character movement component before making a predictive correction 
void FSavedMove_TPBaseCharacter::PrepMoveFor(class ACharacter* Character) {
	Super::PrepMoveFor(Character);

	UTPBaseMovement* CharMov = Cast<UTPBaseMovement>(Character->GetCharacterMovement());
	if (CharMov) {

	}
}



///////////////////////////////////////////////////////////////////////////////////////
//
//  FNetworkPredictionData_Client_TPBaseCharacter functions
//
///////////////////////////////////////////////////////////////////////////////////////

FNetworkPredictionData_Client_TPBaseCharacter::FNetworkPredictionData_Client_TPBaseCharacter(const UCharacterMovementComponent & ClientMovement)
	: Super(ClientMovement) {

}


FSavedMovePtr FNetworkPredictionData_Client_TPBaseCharacter::AllocateNewMove() {
	return FSavedMovePtr(new FSavedMove_TPBaseCharacter());
}



///////////////////////////////////////////////////////////////////////////////////////
//
//  FNetworkPredictionData_Server_TPBaseCharacter functions
//
///////////////////////////////////////////////////////////////////////////////////////

FNetworkPredictionData_Server_TPBaseCharacter::FNetworkPredictionData_Server_TPBaseCharacter(const UCharacterMovementComponent& ServerMovement)
	: Super(ServerMovement) {

}

