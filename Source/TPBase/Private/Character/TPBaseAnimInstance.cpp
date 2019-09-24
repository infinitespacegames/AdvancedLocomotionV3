// Copyright is Bullshit!  Do as you will with these files.

#include "TPBaseAnimInstance.h"
#include "TPBaseCharacter.h"
#include "TPBaseMovement.h"

#include <Animation/AnimMontage.h>
#include <Components/CapsuleComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <Curves/CurveFloat.h>
#include <DrawDebugHelpers.h>



// Default constructor
UTPBaseAnimInstance::UTPBaseAnimInstance() {
	Stance = EStance::eStanding;
	GaitMode = EGaitMode::eWalking;
	LocomotionMode = ELocomotionMode::eGrounded;
	RotationMode = ERotationMode::eLookingMode;
	CameraMode = ECameraMode::eThirdPerson;
	WalkSpeed = 165.0;
	RunSpeed = 380.0f;
	SprintSpeed = 650.0f;
	CrouchSpeed = 150.0f;
	bShowTraces = false;
	bRightFootForward = false;
}


// Called at start of play 
void UTPBaseAnimInstance::NativeInitializeAnimation() {

	// Grab references
	Character = Cast<ATPBaseCharacter>(GetOwningActor());
	if (!Character) { return; }

	Capsule = Character->GetCapsuleComponent();
	MovementComponent = Character->GetCharacterMovement();

	// Only updates if connected to character
	UpdateAnimationState();

	// Idle entry state
	if (Stance == EStance::eCrouching) {
		IdleEntryState = bRightFootForward ? EIdleEntryState::eCRF_Idle : EIdleEntryState::eCLF_Idle;
	}
	else {
		IdleEntryState = EIdleEntryState::eN_Idle;
	}
}


// Called every frame 
void UTPBaseAnimInstance::NativeUpdateAnimation(float DeltaTimeX) {
	if (!Character || DeltaTimeX == 0.0f) { return; }

	// Self updates state from character state
	UpdateAnimationState();

	// Duh?
	CalculateAimOffset();

	// Ragdoll
	if (bIsRagdoll) {
		DoWhileRagdoll();
		return;
	}

	// Animate according to state
	switch (LocomotionMode) {
	case ELocomotionMode::eGrounded:
		Speed = CharacterVelocity.Size();
		if (bIsMoving) {
			DoWhileMoving();
		}
		else {
			DoTurnInPlace();
		}

		switch (ActiveLocomotionState) {
		case ELocomotionState::eNotMoving:
			if (bIsMoving) {
				CalculateStartPosition();
			} 
			break;
		case ELocomotionState::eMoving:
			CalculateGroundedLeaningValues();
			break;
		case ELocomotionState::ePivot:
			CalculatePivot();
			break;
		}
		break;
	case ELocomotionMode::eFalling:
		DoWhileFalling();
		break;
	}
}



////////////////////////////////////////////////////////////////////
//
//   Animation core functions, can be overriden in blueprint or C++
//
////////////////////////////////////////////////////////////////////

// Performed while moving
void UTPBaseAnimInstance::DoWhileMoving_Implementation() {
	CalculateGaitValue();
	CalculatePlayRates(CrouchSpeed, WalkSpeed, RunSpeed, SprintSpeed);
	CalculateMovementDirection(-90.0f, 90.0f, 5.0f);
}


// Performed while falling 
void UTPBaseAnimInstance::DoWhileFalling_Implementation() {
	Speed = FVector2D(CharacterVelocity).Size();
	FlailBlendAlpha = (FlailAlphaCurve) ? FlailAlphaCurve->GetFloatValue(-CharacterVelocity.Z) : 1.0f;
	CalculateAirLeaningValues();
	CalculateLandPredictionAlpha();
}


// Performed while turning
void UTPBaseAnimInstance::DoTurnInPlace_Implementation() {
	if (!Character || Character->IsPlayingRootMotion()) { return; }
	if (RotationMode == ERotationMode::eVelocityMode) { return; }

	if (bIsAiming) {
		if (Stance == EStance::eCrouching) {
			if (bRightFootForward) {
				DoResponsiveTurn(60.0f, 1.5f, CRF_Turn_90);
			}
			else {
				DoResponsiveTurn(60.0f, 1.5f, CLF_Turn_90);
			}
		}
		else {
			if (bRightFootForward) {
				DoResponsiveTurn(60.0f, 1.5f, RF_Turn_90);
			}
			else {
				DoResponsiveTurn(60.0f, 1.5f, LF_Turn_90);
			}
		}
	}
	else {
		if (CameraMode == ECameraMode::eFirstPerson) {
			if (Stance == EStance::eCrouching) {
				if (bRightFootForward) {
					DoResponsiveTurn(70.0f, 1.5f, CRF_Turn_90);
				}
				else {
					DoResponsiveTurn(70.0f, 1.5f, CLF_Turn_90);
				}
			}
			else {
				DoResponsiveTurn(70.0f, 1.5f, N_Turn_90);
			}
		}
		else {
			if (Stance == EStance::eCrouching) {
				if (bTurningInPlace) { return; }
				FTurnAnimations Turn = bRightFootForward ? CRF_Turn_90 : CLF_Turn_90;
				DoDelayedTurn(100.0f, 60.0f, 0.5f, 1.25f, Turn, 130.0f, 0.0f, 1.5f, Turn);
			}
			else {
				if (bTurningInPlace) { return; }
				DoDelayedTurn(100.0f, 60.0f, 0.5f, 1.5f, N_Turn_90, 130.0f, 0.0f, 1.25f, N_Turn_180);
			}
		}
	}
}


// Performs a delayed turn
void UTPBaseAnimInstance::DoDelayedTurn_Implementation(float MaxCameraSpeed,
	float YawLimit1, float Delay1, float PlayRate1, FTurnAnimations TurnAnims1, 
	float YawLimit2, float Delay2, float PlayRate2, FTurnAnimations TurnAnims2) {

	float PlayRate, Delta;
	UAnimMontage *Turn, *Turn1, *Turn2;
	
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	if (AimYawDelta > 0.0f) {
		Turn1 = TurnAnims1.RightTurn;
		Turn2 = TurnAnims2.RightTurn;
	}
	else {
		Turn1 = TurnAnims1.LeftTurn;
		Turn2 = TurnAnims2.LeftTurn;
	}

	if (FMath::Abs(AimYawDelta) >= YawLimit2) {
		Turn = Turn2;
		PlayRate = PlayRate2;
	}
	else {
		Turn = Turn1;
		PlayRate = PlayRate1;
	}

	Delta = FMath::GetMappedRangeValueClamped(
		FVector2D(YawLimit1, YawLimit2),
		FVector2D(Delay1, Delay2),
		FMath::Abs(AimYawDelta)
	);

	if (FMath::Abs(AimYawRate) < MaxCameraSpeed && FMath::Abs(AimYawDelta) > YawLimit1) {
		TurnInPlaceDelay += DeltaTime;
	}
	else {
		TurnInPlaceDelay = 0.0f;
		return;
	}

	bShouldTurnInPlace = TurnInPlaceDelay > Delta;
	if (Montage_IsPlaying(Turn)) { return; }
	Montage_Play(Turn, PlayRate);
}


// Performs a responsive turn
void UTPBaseAnimInstance::DoResponsiveTurn_Implementation(float YawLimit, float PlayRate, FTurnAnimations TurnAnims) {

	float DeltaTime = GetWorld()->GetDeltaSeconds();
	UAnimMontage* Turn;

	bShouldTurnInPlace = FMath::Abs(AimYawDelta) > YawLimit;
	if (!bShouldTurnInPlace) { return; }

	Turn = AimYawDelta > 0 ? TurnAnims.RightTurn : TurnAnims.LeftTurn;

	if (bTurningInPlace) {
		if (bTurningRight) {
			if (AimYawDelta > 0.0f) { return; }
		}
		else {
			if (AimYawDelta < 0.0f) { return; }
		}
	}

	float Mult = FMath::GetMappedRangeValueClamped(
		FVector2D(120.0f, 400.0f),
		FVector2D(1.0f, 2.0f),
		FMath::Abs(AimYawDelta)
	);

	if (Montage_IsPlaying(Turn)) { return; }
	Montage_Play(Turn, Mult * PlayRate);
}


// Calculates animation gait value based on character state 
void UTPBaseAnimInstance::CalculateGaitValue_Implementation() {
	if (Speed > RunSpeed) {
		GaitValue = FMath::GetMappedRangeValueClamped(FVector2D(RunSpeed, SprintSpeed), FVector2D(2.0f, 3.0f), Speed);
	}
	else {
		if (Speed > WalkSpeed) {
			GaitValue = FMath::GetMappedRangeValueClamped(FVector2D(WalkSpeed, RunSpeed), FVector2D(1.0f, 2.0f), Speed);
		}
		else {
			GaitValue = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, WalkSpeed), FVector2D(0.0f, 1.0f), Speed);
		}
	}
}


// Calculates animation play rates based on character state 
void UTPBaseAnimInstance::CalculatePlayRates_Implementation(float Crouch, float Walk, float Run, float Sprint) {

	float Low = FMath::GetMappedRangeValueUnclamped(FVector2D(0.0f, Walk), FVector2D(0.0f, 1.0f), Speed);
	float Med = FMath::GetMappedRangeValueUnclamped(FVector2D(0.0f, Run), FVector2D(0.0f, 1.0f), Speed);
	float High = FMath::GetMappedRangeValueUnclamped(FVector2D(0.0f, Sprint), FVector2D(0.0f, 1.0f), Speed);

	float ClampedSpeed;
	if (GaitValue > 2.0f) {
		ClampedSpeed = FMath::GetMappedRangeValueClamped(FVector2D(2.0f, 3.0f), FVector2D(Med, High), GaitValue);
	}
	else {
		ClampedSpeed = FMath::GetMappedRangeValueClamped(FVector2D(1.0f, 2.0f), FVector2D(Low, Med), GaitValue);
	}

	float Scale = 1.0f;
	if (Capsule) {
		Scale = Capsule->GetComponentScale().Z;
	}
	N_PlayRate = ClampedSpeed / Scale;

	ClampedSpeed = FMath::GetMappedRangeValueUnclamped(FVector2D(0.0f, Crouch), FVector2D(0.0f, 1.0f), Speed);
	C_PlayRate = ClampedSpeed / Scale;
}


// Calculates animation movement direction based on character state 
void UTPBaseAnimInstance::CalculateMovementDirection_Implementation(float ThresholdMin, float ThresholdMax, float Tolerance) {
	if (MovementDirection == EMovementDirection::eForward) {
		if (YawDifferential >= ThresholdMin - Tolerance && YawDifferential <= ThresholdMax + Tolerance) {
			MovementDirection = EMovementDirection::eForward;
		}
		else {
			MovementDirection = EMovementDirection::eBackward;
		}
	}
	else {
		if (YawDifferential >= ThresholdMin + Tolerance && YawDifferential <= ThresholdMax - Tolerance) {
			MovementDirection = EMovementDirection::eForward;
		}
		else {
			MovementDirection = EMovementDirection::eBackward;
		}
	}
}


// Calculates the aim offset of the character 
void UTPBaseAnimInstance::CalculateAimOffset_Implementation() {

	// Get time
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	// Looking mode
	if (RotationMode == ERotationMode::eLookingMode) {
		FRotator Delta = (LookRotation - CharacterRotation).GetNormalized();

		float InterpSpeed = FMath::GetMappedRangeValueClamped(
			FVector2D(0.0f, 180.0f),
			FVector2D(30.0f, 5.0f),
			FMath::Abs(Delta.Yaw - AimOffset.X)
		); 
		AimOffset = FMath::Vector2DInterpTo(AimOffset, FVector2D(Delta.Yaw, Delta.Pitch), DeltaTime, InterpSpeed);
	}

	// Velocity mode
	else {
		if (bHasMovementInput) {
			FRotator Delta = (PrevMovementRotation - CharacterRotation).GetNormalized();
			float InterpSpeed = FMath::GetMappedRangeValueClamped(
				FVector2D(0.0f, 180.0f),
				FVector2D(15.0f, 5.0f),
				FMath::Abs(Delta.Yaw - AimOffset.X)
			);
			float YawClamped = FMath::Clamp(Delta.Yaw, -90.0f, 90.0f);
			AimOffset = FMath::Vector2DInterpTo(AimOffset, FVector2D(YawClamped, Delta.Pitch), DeltaTime, InterpSpeed);
		}

		// Reset if no input
		else {
			AimOffset = FMath::Vector2DInterpTo(AimOffset, FVector2D(0.0f, 0.0f), DeltaTime, 4.0f);
		}
	}
}


// Calculates pivot information
void UTPBaseAnimInstance::CalculatePivot_Implementation() {
	bool Trigger = FMath::IsNearlyEqual(YawDifferential, PivotParameters.PivotDirection, 45.0f);
	StartPosition = Trigger ? PivotParameters.InterruptedTime : PivotParameters.CompletedTime;
	MovementDirection = Trigger ? PivotParameters.InterruptedDirection : PivotParameters.CompletedDirection;
}


// Calculates montage starting position
void UTPBaseAnimInstance::CalculateStartPosition_Implementation() {
	if (Stance == EStance::eCrouching) {
		StartPosition = (YawDifferential > 0.0f) ? 0.25f : 0.5f;
	}
	else {
		if (bIsAiming) {
			StartPosition = (YawDifferential > 0.0f) ? 0.187f : 1.0f;
		}
		else {
			StartPosition = (YawDifferential > 0.0f) ? 0.3f : 0.867f;
		}
	}
}


// Calculates the lean value while grounded
void UTPBaseAnimInstance::CalculateGroundedLeaningValues_Implementation() {
	if (!MovementComponent) { return; }

	float DeltaTime = GetWorld()->GetDeltaSeconds();
	float VelocityDifference = (PrevVelocityRotation - PrevVelocityRotationABP).GetNormalized().Yaw / DeltaTime;
	PrevVelocityRotationABP = PrevVelocityRotation;
	  
	float LeanRotation =
		FMath::GetMappedRangeValueClamped(FVector2D(WalkSpeed, RunSpeed), FVector2D(0.0f, 1.0f), Speed) *
		FMath::GetMappedRangeValueClamped(FVector2D(-200.0f, 200.0f), FVector2D(-1.0f, 1.0f), VelocityDifference);

	float AccelerationDifferential = (Speed - PreviousSpeed) / DeltaTime;
	PreviousSpeed = Speed;

	float LeanAcceleration;
	if (AccelerationDifferential > 0.0f) {
		LeanAcceleration = FMath::GetMappedRangeValueClamped(
			FVector2D(0.0f, MovementComponent->GetMaxAcceleration()),
			FVector2D(0.0f, 1.0f),
			FMath::Abs(AccelerationDifferential)
		);
	}
	else {
		LeanAcceleration = FMath::GetMappedRangeValueClamped(
			FVector2D(0.0f, MovementComponent->BrakingDecelerationWalking),
			FVector2D(0.0f, -1.0f),
			FMath::Abs(AccelerationDifferential)
		);
	}

	LeanAcceleration *= FMath::GetMappedRangeValueClamped(
		FVector2D(WalkSpeed, RunSpeed),
		FVector2D(0.0f, 1.0f),
		Speed
	);

	FVector  LeanVector = FVector(LeanRotation, LeanAcceleration, 0.0f);
	LeanGrounded = FVector2D(LeanVector.RotateAngleAxis(YawDifferential, FVector(0.0f, 0.0f, -1.0f)));
}


// Calculates the lean value while in air
void UTPBaseAnimInstance::CalculateAirLeaningValues_Implementation() {
	if (!MovementComponent) { return; }

	float ZVelocity = MovementComponent->JumpZVelocity;

	ZVelocity = FMath::GetMappedRangeValueClamped(
		FVector2D(ZVelocity, ZVelocity * -2.0f),
		FVector2D(1.0f, -1.0f),
		ZVelocity
	);
	LeanInAir = ZVelocity * FMath::Clamp(Speed / RunSpeed, 0.0f, 1.0f);
}


// Calculates the land prediction alpha value
void UTPBaseAnimInstance::CalculateLandPredictionAlpha_Implementation() {
	if (!MovementComponent) { return; }

	float DeltaTime = GetWorld()->GetDeltaSeconds();
	
	if (CharacterVelocity.Z >= 0.0f) {
		LandPredictionAlpha = FMath::FInterpTo(LandPredictionAlpha, 0.0f, DeltaTime, 10.0f);
		return;
	}

	auto Owner = TryGetPawnOwner();
	if (!Owner) { return; }

	// Debug parameters
	float Radius = 10.0f;
	FVector TraceStart = Owner->GetActorLocation();
	if (Capsule) {
		TraceStart.Z -= Capsule->GetScaledCapsuleHalfHeight_WithoutHemisphere();
		Radius = Capsule->GetScaledCapsuleRadius();
	}

	float Factor = FMath::GetMappedRangeValueClamped(
		FVector2D(0.0f, -4000.0f), 
		FVector2D(50.0f, 2000.0f), 
		CharacterVelocity.Z
	);
	FVector Vel = FVector(FVector2D(CharacterVelocity), FMath::Clamp(CharacterVelocity.Z, -4000.0f, -200.0f));
	Vel.Normalize();
	
	FVector TraceEnd = TraceStart + (Factor * Vel);

	// Setup parameters
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Owner);
	QueryParams.bTraceComplex = false;
	FHitResult Hit;
	bool bHasHit = GetWorld()->SweepSingleByChannel(
		Hit,
		TraceStart,
		TraceEnd,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(Radius)
	);

	if (bHasHit && Hit.ImpactNormal.Z >= MovementComponent->GetWalkableFloorZ()) {
		float Time = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 1.0f), FVector2D(1.0f, 0.0f), Hit.Time);
		float Target = LandAlphaCurve->GetFloatValue(Time);
		LandPredictionAlpha = FMath::Clamp(FMath::FInterpTo(LandPredictionAlpha, Target, DeltaTime, 20.0f), 0.0f, 1.0f);

		if (bShowTraces) {
			DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 12, 5.0f, FColor::Green);
			DrawDebugLine(GetWorld(), TraceStart, Hit.ImpactPoint, FColor::Red, false, -1.0f, 0, 2.0f);
		}
	}
	else {
		LandPredictionAlpha = FMath::FInterpTo(LandPredictionAlpha, 0.0f, DeltaTime, 10.0f);
	}
}



////////////////////////////////////////////////////////////////////
//
//   Animation notification events
//
////////////////////////////////////////////////////////////////////

// Performed while ragdolling
void UTPBaseAnimInstance::DoWhileRagdoll_Implementation() {
	FlailRate = FMath::GetMappedRangeValueClamped(
		FVector2D(0.0f, 1000.0f), 
		FVector2D(0.0f, 1.25f), 
		CharacterVelocity.Size()
	);
}


// Animation notify for pivot
void UTPBaseAnimInstance::Pivot_Notify(const FPivotParameters& Params) {
	PivotParameters = Params;
}


// Animation notify for turning
void UTPBaseAnimInstance::TurnInPlace_Notify(UAnimMontage* TurnAnim, bool bShouldTurn, bool bIsTurning, bool bRightTurn) {
	ActiveTurningMontage = TurnAnim;
	bShouldTurnInPlace = bShouldTurn;
	bTurningInPlace = bIsTurning;
	bTurningRight = bRightTurn;
}


// Animation notify for idle entry
void UTPBaseAnimInstance::IdleEntry_Notify(EIdleEntryState NewIdleState) {
	IdleEntryState = NewIdleState;
}


// Animation notify for idle transitions 
void UTPBaseAnimInstance::IdleTransition_Notify(UAnimSequenceBase* Animation, float PlayRate, float StartTime) {
	PlaySlotAnimationAsDynamicMontage(Animation, "Transition", 0.2f, 0.2f, PlayRate, 1, 0.0f, StartTime);
}


// Animation notify for entering locomotion state moving
void UTPBaseAnimInstance::AnimNotify_Entered_Moving() {
	ActiveLocomotionState = ELocomotionState::eMoving;
}


// Animation notify for entering locomotion state not moving
void UTPBaseAnimInstance::AnimNotify_Entered_NotMoving() {
	ActiveLocomotionState = ELocomotionState::eNotMoving;
	MovementDirection = EMovementDirection::eForward;
}


// Animation notify for entering locomotion state pivot
void UTPBaseAnimInstance::AnimNotify_Entered_Pivot() {
	ActiveLocomotionState = ELocomotionState::ePivot;
}


// Animation notify for entering locomotion state stopping
void UTPBaseAnimInstance::AnimNotify_Entered_Stopping() {
	ActiveLocomotionState = ELocomotionState::eStopping;
	FeetPosition = FVector2D(GetCurveValue("FootDirection"), GetCurveValue("FootPosition"));
}
 

// Animation notify for landing 
void UTPBaseAnimInstance::AnimNotify_Land() {
	if (!AdditiveLand) { return; }
	PlaySlotAnimationAsDynamicMontage(AdditiveLand, "AdditiveLand", 0.0f, 0.2f, 1.25f, 1, 0.0f);
}


 
////////////////////////////////////////////////////////////////////
//
//   Interface functions to needed character variables
//
////////////////////////////////////////////////////////////////////

// Calls all interface functions to synchronize animation state with character
void UTPBaseAnimInstance::UpdateAnimationState() {

	// Character data
	if (!Character) { return; }

	ITPBaseInterfaceABP::Execute_OnSetShowTraces((UObject*)this, Character->bShowTraces);
	ITPBaseInterfaceABP::Execute_OnSetIsMoving((UObject*)this, Character->bIsMoving);
	ITPBaseInterfaceABP::Execute_OnSetHasMovementInput((UObject*)this, Character->bHasMovementInput);
	ITPBaseInterfaceABP::Execute_OnSetForwardFoot((UObject*)this, Character->bRightFootForward);
	ITPBaseInterfaceABP::Execute_OnSetCharacterVelocity((UObject*)this, Character->GetCharacterVelocity());
	ITPBaseInterfaceABP::Execute_OnSetCameraMode((UObject*)this, Character->CameraMode);
	ITPBaseInterfaceABP::Execute_OnSetCharacterRotation((UObject*)this, Character->CharacterRotation);
	ITPBaseInterfaceABP::Execute_OnSetLookRotation((UObject*)this, Character->LookRotation);
	ITPBaseInterfaceABP::Execute_OnSetPrevVelocityRotation((UObject*)this, Character->PrevVelocityRotation);
	ITPBaseInterfaceABP::Execute_OnSetPrevMovementRotation((UObject*)this, Character->PrevMovementRotation);
	ITPBaseInterfaceABP::Execute_OnSetYawDifferential((UObject*)this, Character->YawDifferential);
	ITPBaseInterfaceABP::Execute_OnSetMovementDifferential((UObject*)this, Character->MovementDifferential);
	ITPBaseInterfaceABP::Execute_OnSetRotationDifferential((UObject*)this, Character->RotationDifferential);
	ITPBaseInterfaceABP::Execute_OnSetAimYawRate((UObject*)this, Character->AimYawRate);
	ITPBaseInterfaceABP::Execute_OnSetAimYawDelta((UObject*)this, Character->AimYawDelta);
	ITPBaseInterfaceABP::Execute_OnSetIsRagdoll((UObject*)this, Character->bIsRagdoll);

	// Character movement component data
	auto CharMov = Character->GetTPBaseMovement();
	if (!CharMov) { return; }

	ITPBaseInterfaceABP::Execute_OnSetIsAiming((UObject*)this, CharMov->IsAiming());
	ITPBaseInterfaceABP::Execute_OnSetCrouchSpeed((UObject*)this, CharMov->GetCrouchSpeed());
	ITPBaseInterfaceABP::Execute_OnSetWalkSpeed((UObject*)this, CharMov->GetWalkSpeed());
	ITPBaseInterfaceABP::Execute_OnSetRunSpeed((UObject*)this, CharMov->GetRunSpeed());
	ITPBaseInterfaceABP::Execute_OnSetSprintSpeed((UObject*)this, CharMov->GetSprintSpeed());
	ITPBaseInterfaceABP::Execute_OnSetStance((UObject*)this, CharMov->GetStance());
	ITPBaseInterfaceABP::Execute_OnSetRotationMode((UObject*)this, CharMov->GetRotationMode());
	ITPBaseInterfaceABP::Execute_OnSetGaitMode((UObject*)this, CharMov->GetGaitMode());
	ITPBaseInterfaceABP::Execute_OnSetLocomotionMode((UObject*)this, CharMov->GetLocomotionMode());
}


// Called to update animation stance 
void UTPBaseAnimInstance::OnSetStance_Implementation(EStance NewStance) {
	if (Stance == NewStance) { return; }
	Stance = NewStance;

	// Stop turning montage if necessary
	if (bTurningInPlace) {
		Montage_Stop(1.0f, ActiveTurningMontage);
	}
}


// Called to update animation locomotion mode 
void UTPBaseAnimInstance::OnSetLocomotionMode_Implementation(ELocomotionMode NewMode) {
	if (LocomotionMode == NewMode) { return; }
	PrevLocomotionMode = LocomotionMode;
	LocomotionMode = NewMode;

	if (bIsRagdoll) {
		Montage_Stop(0.2f);
		return;
	}

	// Adjust animation variables
	switch (LocomotionMode) {
	case ELocomotionMode::eFalling:
		N_PlayRate = 0.0f;
		C_PlayRate = 0.0f;
		bShouldTurnInPlace = false;
		break;
	}
}

