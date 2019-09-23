// Copyright is Bullshit!  Do as you will with these files.

#include "TPBaseCharacter.h"
#include "TPBaseAnimInstance.h"
#include "TPBaseAnimInstanceIK.h"

#include "HealthComponent.h"
#include "PlayerInfoWidget.h"
#include "PointOfInterest.h"

#include <Camera/CameraComponent.h>
#include <Components/ArrowComponent.h>
#include <Components/InputComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include <Components/TimelineComponent.h>
#include <ConstructorHelpers.h>
#include <DrawDebugHelpers.h>
#include <Engine.h>
#include <Engine/Engine.h>
#include <GameFramework/SpringArmComponent.h>
#include <Kismet/KismetSystemLibrary.h>
#include <UnrealNetwork.h>



// Base constructor
ATPBaseCharacter::ATPBaseCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UTPBaseMovement>(ACharacter::CharacterMovementComponentName)) {

	// Set this character to call Tick() every frame.
	PrimaryActorTick.bCanEverTick = true;

	// Capsule and debug arrows
	GetCapsuleComponent()->InitCapsuleSize(30.f, HalfHeight);
	UCharacterMovementComponent* MC = GetCharacterMovement();
	if (ensure(MC)) {
		MC->CrouchedHalfHeight = CrouchedHalfHeight;
		MC->bCanWalkOffLedgesWhenCrouching = true;
		MC->bOrientRotationToMovement = false;
		MC->bUseControllerDesiredRotation = false;
		MC->NavAgentProps.bCanCrouch = true;
	}
	CreateArrowComponents();

	// Override default movement settings
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	JumpMaxHoldTime = 0.5f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	TP_SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("TP_SpringArm"));
	TP_SpringArm->SetupAttachment(RootComponent);
	TP_SpringArm->TargetArmLength = 325.0f;
	TP_SpringArm->AddLocalOffset(FVector(0.0f, 0.0f, 45.0f));
	TP_SpringArm->bUsePawnControlRotation = true;

	// Create a follow camera
	TP_Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("TP_Camera"));
	TP_Camera->SetupAttachment(TP_SpringArm, USpringArmComponent::SocketName);
	TP_Camera->bUsePawnControlRotation = false;

	// Create a first person camera
	FP_Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("FP_Camera"));
	FP_Camera->SetupAttachment(RootComponent);
	FP_Camera->SetRelativeTransform(FTransform(FRotator(0.0f, 90.0f, -90.0f), FVector(5.0f, 14.0f, 0.0f)));
	FP_Camera->SetFieldOfView(100.0f);
	FP_Camera->bUsePawnControlRotation = true;
	FP_Camera->bAutoActivate = false;

	// Health Component
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));

	// Point of Interest Component
	PointOfInterest = CreateDefaultSubobject<UPointOfInterest>(TEXT("PointOfInterest Component"));

	// Curves for view transitioning 
	CameraLerpCurves.SetNum(4);
	auto RichCurve = new FRichCurve();
	auto Key = RichCurve->AddKey(0.0f, 0.0f);
	RichCurve->AddKey(1.0f, 1.0f);
	RichCurve->SetKeyInterpMode(Key, RCIM_Cubic);
	CameraLerpCurves[0] = CreateDefaultSubobject<UCurveFloat>("Default");
	CameraLerpCurves[0]->FloatCurve = *RichCurve;

	// Defaults
	FPCameraSocketName = "head";
	PelvisBoneName = "pelvis";
	RagdollPoseName = "RagdollPose";
	CameraMode = ECameraMode::eThirdPerson;
	CameraGaitMode = EGaitMode::eWalking;
	bRightShoulder = true;
	bShowSettings = false;
	bShowTraces = false;
	bRightFootForward = false;
	bToggleSprint = true;
}


void ATPBaseCharacter::BeginPlay() {
	Super::BeginPlay();

	// Attach FP camera to mesh instead of capsule
	FP_Camera->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, FPCameraSocketName);

	// Ensure current values are set
	GetMesh()->AddTickPrerequisiteActor(this);
	AddTickPrerequisiteComponent(GetTPBaseMovement());

	// Setup camera lerp functionality
	CameraLerpTimeline = NewObject<UTimelineComponent>(this, FName("CameraLerpAnimation"));
	CameraLerpTimeline->CreationMethod = EComponentCreationMethod::UserConstructionScript;
	CameraLerpTimeline->SetNetAddressable();
	CameraLerpTimeline->SetLooping(false);
	CameraLerpTimeline->SetTimelineLengthMode(TL_LastKeyFrame);
	CameraLerpTimeline->SetPlaybackPosition(0.0f, false);
	CameraLerpTimeline->RegisterComponent();

	// Bind for callback
	BlueprintCreatedComponents.Add(CameraLerpTimeline);
	onCameraLerpCallback.BindUFunction(this, FName{ TEXT("CameraLerpCallback") });

	// Initialize rotators
	FRotator MyRotation = GetActorRotation().GetNormalized();
	LookRotation = MyRotation;
	CharacterRotation = MyRotation;
	TargetRotation = MyRotation;
	PrevVelocityRotation = MyRotation;
	PrevMovementRotation = MyRotation;

	// Update debug display
	UpdateArrowComponents();
	UpdateCapsuleVisibility();
	 
	// Update character
	UpdateCamera(CameraLerpCurves[2]);

	// Health component delegate
	HealthComponent->OnHealthChanged.AddDynamic(this, &ATPBaseCharacter::OnHealthChanged);
}


// Called every frame
void ATPBaseCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	// Update camera lerp
	if (CameraLerpTimeline) {
		CameraLerpTimeline->TickComponent(DeltaTime, LEVELTICK_TimeOnly, NULL);
	}

	// Store
	bool bPrevMoving = bIsMoving;

	// Calculate needed state variables
	CalculateStateVariables();

	// Handles character rotation adjustments
	ManageCharacterRotation();

	// Movement debugging display
	UpdateArrowComponents();
	UpdateCapsuleVisibility();

	// Character state information
	PrintCharacterInfo();

}


// Replicated properties to client
void ATPBaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ATPBaseCharacter, bShowTraces, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ATPBaseCharacter, bShowSettings, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ATPBaseCharacter, bRightFootForward, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ATPBaseCharacter, bIsDead, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ATPBaseCharacter, bRagdollGrounded, COND_SkipOwner);
}
 


////////////////////////////////////////////////////////////////////
//
//   Native event handling
//
////////////////////////////////////////////////////////////////////

// Handles native begin crouch event 
void ATPBaseCharacter::OnStartCrouch(float HHAdjust, float SHHAdjust) {
	Super::OnStartCrouch(HHAdjust, SHHAdjust);

	// Preserve camera relative location as it's attached to capsule
	TP_Camera->AddRelativeLocation(FVector(0.0f, 0.0f, SHHAdjust));

	// Update camera
	UpdateCamera(CameraLerpCurves[2]);

	// Update Arrows
	UpdateArrowPositions();
}


// Handles native end crouch event 
void ATPBaseCharacter::OnEndCrouch(float HHAdjust, float SHHAdjust) {
	Super::OnEndCrouch(HHAdjust, SHHAdjust);

	// Preserve camera relative location as it's attached to capsule
	TP_Camera->AddRelativeLocation(FVector(0.0f, 0.0f, -SHHAdjust));

	// Update camera
	UpdateCamera(CameraLerpCurves[2]);

	// Update Arrows
	UpdateArrowPositions();
}


// Handle native landing event 
void ATPBaseCharacter::Landed(const FHitResult& Hit) {
	Super::Landed(Hit);

	if (bHasMovementInput) {
		GetCharacterMovement()->BrakingFrictionFactor = 0.5f;
	}
	else {
		GetCharacterMovement()->BrakingFrictionFactor = 3.0f;
	}

	// delay 0.2
	FTimerHandle DelayTimer;
	GetWorld()->GetTimerManager().SetTimer(DelayTimer, [this]() {
		GetCharacterMovement()->BrakingFrictionFactor = 0.0f;
	}, 1.0f, false, 0.2);
}


// Handles native movement mode change event 
void ATPBaseCharacter::OnMovementModeChanged(EMovementMode PrevMode, uint8 PrevCustomMode) {
	Super::OnMovementModeChanged(PrevMode, PrevCustomMode);

	auto CharMov = GetTPBaseMovement();
	if (!CharMov) { return; }

	PrevLocomotionMode = CharMov->ConvertMovementMode(PrevMode);
}


// Handles native health changed event 
void ATPBaseCharacter::OnHealthChanged(UHealthComponent* HealthComp, float HealthValue, float ShieldValue, float Damage,
	const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser) {

	if (HasAuthority()) {
		if (HealthValue <= 0.0f && !bIsDead) {
			EnterRagdoll();

			// Delay character disconnect
			FTimerHandle DelayTimer;
			GetWorld()->GetTimerManager().SetTimer(DelayTimer, [this]() {
				SetIsDead(true);
				GetMovementComponent()->StopMovementImmediately();
				GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				DetachFromControllerPendingDestroy();
				SetLifeSpan(10.0f);
			}, 10.0f, false, 1.0f);
		}
	}
}



////////////////////////////////////////////////////////////////////
//
//   Player input handling
//
////////////////////////////////////////////////////////////////////

// Called to bind functionality to input
void ATPBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	// Axis mappings
	PlayerInputComponent->BindAxis("MoveForward", this, &ATPBaseCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATPBaseCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turn-rate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAxis("TurnRate", this, &ATPBaseCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ATPBaseCharacter::LookUpAtRate);

	// Action mappings
	PlayerInputComponent->BindAction("StanceAction", IE_Pressed, this, &ATPBaseCharacter::StanceAction);

	PlayerInputComponent->BindAction("JumpAction", IE_Pressed, this, &ATPBaseCharacter::JumpAction);
	PlayerInputComponent->BindAction("JumpAction", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("SprintAction", IE_Pressed, this, &ATPBaseCharacter::BeginSprint);
	PlayerInputComponent->BindAction("SprintAction", IE_Released, this, &ATPBaseCharacter::EndSprint);

	PlayerInputComponent->BindAction("WalkAction", IE_Pressed, this, &ATPBaseCharacter::WalkAction);

	PlayerInputComponent->BindAction("AimAction", IE_Pressed, this, &ATPBaseCharacter::BeginAiming);
	PlayerInputComponent->BindAction("AimAction", IE_Released, this, &ATPBaseCharacter::EndAiming);

	PlayerInputComponent->BindAction("SelectRotationMode", IE_Released, this, &ATPBaseCharacter::SwitchRotationMode);

	PlayerInputComponent->BindAction("RagdollAction", IE_Pressed, this, &ATPBaseCharacter::RagdollAction);

	PlayerInputComponent->BindAction("SelectForwardFoot", IE_Released, this, &ATPBaseCharacter::SwitchForwardFoot);

	// First / third person camera
	PlayerInputComponent->BindAction("CameraAction", IE_Pressed, this, &ATPBaseCharacter::BeginSwitchCamera);
	PlayerInputComponent->BindAction("CameraAction", IE_Released, this, &ATPBaseCharacter::EndSwitchCamera);

	// Character information debug
	FInputActionBinding DebugInfoBinding("ShowDebugInfo", IE_Pressed);
	DebugInfoBinding.ActionDelegate.GetDelegateForManualSet().BindLambda([this]() {
		SetShowSettings(!bShowSettings);
	});
	PlayerInputComponent->AddActionBinding(DebugInfoBinding);

	// Traces debug
	FInputActionBinding DebugTracesBinding("ShowDebugTraces", IE_Pressed);
	DebugTracesBinding.ActionDelegate.GetDelegateForManualSet().BindLambda([this]() {
		SetShowTraces(!bShowTraces);
	});
	PlayerInputComponent->AddActionBinding(DebugTracesBinding);

	// Crouch speed decrement
	FInputKeyBinding CrouchSpeedDecBinding(FInputChord("Left", EModifierKey::Alt), IE_Pressed);
	CrouchSpeedDecBinding.KeyDelegate.GetDelegateForManualSet().BindLambda([this]() {
		SetCrouchSpeed(FMath::Clamp(GetCrouchingSpeed() - 5.0f, 5.0f, 500.0f));
	});
	PlayerInputComponent->KeyBindings.Add(CrouchSpeedDecBinding);

	// Crouch speed increment
	FInputKeyBinding CrouchSpeedIncBinding(FInputChord("Right", EModifierKey::Alt), IE_Pressed);
	CrouchSpeedIncBinding.KeyDelegate.GetDelegateForManualSet().BindLambda([this]() {
		SetCrouchSpeed(FMath::Clamp(GetCrouchingSpeed() + 5.0f, 5.0f, 500.0f));
	});
	PlayerInputComponent->KeyBindings.Add(CrouchSpeedIncBinding);

	// Walk speed decrement
	FInputKeyBinding WalkSpeedDecBinding(FInputChord("Left", EModifierKey::Control), IE_Pressed);
	WalkSpeedDecBinding.KeyDelegate.GetDelegateForManualSet().BindLambda([this]() {
		SetWalkSpeed(FMath::Clamp(GetWalkingSpeed() - 5.0f, 5.0f, GetRunningSpeed() - 5.0f));
	});
	PlayerInputComponent->KeyBindings.Add(WalkSpeedDecBinding);

	// Walk speed increment
	FInputKeyBinding WalkSpeedIncBinding(FInputChord("Right", EModifierKey::Control), IE_Pressed);
	WalkSpeedIncBinding.KeyDelegate.GetDelegateForManualSet().BindLambda([this]() {
		SetWalkSpeed(FMath::Clamp(GetWalkingSpeed() + 5.0f, 5.0f, GetRunningSpeed() - 5.0f));
	});
	PlayerInputComponent->KeyBindings.Add(WalkSpeedIncBinding);

	// Run speed decrement
	FInputKeyBinding RunSpeedDecBinding(FInputChord("Left", EModifierKey::None), IE_Pressed);
	RunSpeedDecBinding.KeyDelegate.GetDelegateForManualSet().BindLambda([this]() {
		SetRunSpeed(FMath::Clamp(GetRunningSpeed() - 5.0f, GetWalkingSpeed() + 5.0f, GetSprintingSpeed() - 5.0f));
	});
	PlayerInputComponent->KeyBindings.Add(RunSpeedDecBinding);

	// Run speed increment
	FInputKeyBinding RunSpeedIncBinding(FInputChord("Right", EModifierKey::None), IE_Pressed);
	RunSpeedIncBinding.KeyDelegate.GetDelegateForManualSet().BindLambda([this]() {
		SetRunSpeed(FMath::Clamp(GetRunningSpeed() + 5.0f, GetWalkingSpeed() + 5.0f, GetSprintingSpeed() - 5.0f));
	});
	PlayerInputComponent->KeyBindings.Add(RunSpeedIncBinding);

	// Sprint speed decrement
	FInputKeyBinding SprintSpeedDecBinding(FInputChord("Left", EModifierKey::Shift), IE_Pressed);
	SprintSpeedDecBinding.KeyDelegate.GetDelegateForManualSet().BindLambda([this]() {
		SetSprintSpeed(FMath::Clamp(GetSprintingSpeed() - 5.0f, GetRunningSpeed() + 5.0f, 5000.0f));
	});
	PlayerInputComponent->KeyBindings.Add(SprintSpeedDecBinding);

	// Sprint speed increment
	FInputKeyBinding SprintSpeedIncBinding(FInputChord("Right", EModifierKey::Shift), IE_Pressed);
	SprintSpeedIncBinding.KeyDelegate.GetDelegateForManualSet().BindLambda([this]() {
		SetSprintSpeed(FMath::Clamp(GetSprintingSpeed() + 5.0f, GetRunningSpeed() + 5.0f, 5000.0f));
	});
	PlayerInputComponent->KeyBindings.Add(SprintSpeedIncBinding);

	// Jump velocity decrement
	FInputKeyBinding JumpVelocityDecBinding(FInputChord("Down", EModifierKey::None), IE_Pressed);
	JumpVelocityDecBinding.KeyDelegate.GetDelegateForManualSet().BindLambda([this]() {
		float Current = GetCharacterMovement()->JumpZVelocity;
		GetCharacterMovement()->JumpZVelocity = FMath::Clamp(Current - 50.0f, 50.0f, 3000.0f);
	});
	PlayerInputComponent->KeyBindings.Add(JumpVelocityDecBinding);

	// Jump velocity increment
	FInputKeyBinding JumpVelocityIncBinding(FInputChord("Up", EModifierKey::None), IE_Pressed);
	JumpVelocityIncBinding.KeyDelegate.GetDelegateForManualSet().BindLambda([this]() {
		float Current = GetCharacterMovement()->JumpZVelocity;
		GetCharacterMovement()->JumpZVelocity = FMath::Clamp(Current + 50.0f, 50.0f, 3000.0f);
	});
	PlayerInputComponent->KeyBindings.Add(JumpVelocityIncBinding);

	// Jump air control decrement
	FInputKeyBinding JumpControlDecBinding(FInputChord("Down", EModifierKey::Control), IE_Pressed);
	JumpControlDecBinding.KeyDelegate.GetDelegateForManualSet().BindLambda([this]() {
		float Current = GetCharacterMovement()->AirControl;
		GetCharacterMovement()->AirControl = FMath::Clamp(Current - 0.1f, 0.0f, 2.0f);
	});
	PlayerInputComponent->KeyBindings.Add(JumpControlDecBinding);

	// Jump air control increment
	FInputKeyBinding JumpControlIncBinding(FInputChord("Up", EModifierKey::Control), IE_Pressed);
	JumpControlIncBinding.KeyDelegate.GetDelegateForManualSet().BindLambda([this]() {
		float Current = GetCharacterMovement()->AirControl;
		GetCharacterMovement()->AirControl = FMath::Clamp(Current + 0.1f, 0.0f, 2.0f);
	});
	PlayerInputComponent->KeyBindings.Add(JumpControlIncBinding);

	// Jump hold time decrement
	FInputKeyBinding JumpHoldDecBinding(FInputChord("Down", EModifierKey::Shift), IE_Pressed);
	JumpHoldDecBinding.KeyDelegate.GetDelegateForManualSet().BindLambda([this]() {
		JumpMaxHoldTime = FMath::Clamp(JumpMaxHoldTime - 0.25f, 0.0f, 3.0f);
	});
	PlayerInputComponent->KeyBindings.Add(JumpHoldDecBinding);

	// Jump hold time increment
	FInputKeyBinding JumpHoldIncBinding(FInputChord("Up", EModifierKey::Shift), IE_Pressed);
	JumpHoldIncBinding.KeyDelegate.GetDelegateForManualSet().BindLambda([this]() {
		JumpMaxHoldTime = FMath::Clamp(JumpMaxHoldTime + 0.25f, 0.0f, 3.0f);
	});
	PlayerInputComponent->KeyBindings.Add(JumpHoldIncBinding);

	// Restart level
	FInputKeyBinding RestartLevelBinding(FInputChord("R", EModifierKey::None), IE_Pressed);
	RestartLevelBinding.KeyDelegate.GetDelegateForManualSet().BindLambda([this]() {
		if (!GetWorld()) { return; }
		GetWorld()->GetFirstPlayerController()->ConsoleCommand(TEXT("RestartLevel"));
	});
	PlayerInputComponent->KeyBindings.Add(RestartLevelBinding);

	// Show bones
	FInputKeyBinding ShowBonesBinding(FInputChord("B", EModifierKey::None), IE_Pressed);
	ShowBonesBinding.KeyDelegate.GetDelegateForManualSet().BindLambda([this]() {
		if (!GetWorld()) { return; }
		GetWorld()->GetFirstPlayerController()->ConsoleCommand(TEXT("Show Bones"));
	});
	PlayerInputComponent->KeyBindings.Add(ShowBonesBinding);

	// Show collision
	FInputKeyBinding ShowCollisionBinding(FInputChord("V", EModifierKey::None), IE_Pressed);
	ShowCollisionBinding.KeyDelegate.GetDelegateForManualSet().BindLambda([this]() {
		if (!GetWorld()) { return; }
		GetWorld()->GetFirstPlayerController()->ConsoleCommand(TEXT("Show Collision"));
	});
	PlayerInputComponent->KeyBindings.Add(ShowCollisionBinding);

	// Show mesh
	FInputKeyBinding ShowMeshBinding(FInputChord("M", EModifierKey::None), IE_Pressed);
	ShowMeshBinding.KeyDelegate.GetDelegateForManualSet().BindLambda([this]() {
		if (GetMesh()->IsVisible()) {
			GetMesh()->SetVisibility(false, false);
		}
		else {
			GetMesh()->SetVisibility(true, true);
		}

	});
	PlayerInputComponent->KeyBindings.Add(ShowMeshBinding);

}


//Handle forward/backward control input
void ATPBaseCharacter::MoveForward(float Value) {
	if (!Controller || Value == 0.0f) { return; }

	// find out which way is forward
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get forward vector
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, Value);

	// wiggle ragdoll, just for fun
	if (bIsRagdoll) {
		FVector Axis = (GetMesh()->GetSocketLocation("head") - GetMesh()->GetSocketLocation(PelvisBoneName)).GetSafeNormal();
		FVector Torque = 1000.0f * Value * Axis;
		GetMesh()->AddTorqueInRadians(Torque, PelvisBoneName, true);
	}
}


// Handle left/right control input
void ATPBaseCharacter::MoveRight(float Value) {
	if (!Controller || Value == 0.0f) { return; }

	// find out which way is forward
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get forward right
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(Direction, Value);

	// wiggle ragdoll, just for fun
	if (bIsRagdoll) {
		FVector Axis = (GetMesh()->GetSocketLocation("head") - GetMesh()->GetSocketLocation(PelvisBoneName)).GetSafeNormal();
		FVector Torque = 1000.0f * Value * Axis;
		GetMesh()->AddTorqueInRadians(Torque, PelvisBoneName, true);
	}
}


// Handle turn-rate control input
void ATPBaseCharacter::TurnAtRate(float Rate) {
	if (FMath::Abs(Rate) < 0.001) { return; }
	
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * GetTPBaseMovement()->BaseTurnRate * GetWorld()->GetDeltaSeconds());
}


// Handle lookup using absolute delta
void ATPBaseCharacter::LookUpAtRate(float Rate) {

	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * GetTPBaseMovement()->BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}


// Handle jump control input 
void ATPBaseCharacter::JumpAction() {
	auto CharMov = GetTPBaseMovement();
	if (!CharMov) { return; }

	if (CharMov->IsCrouching()) {
		UnCrouch();
	}
	else {
		if (!IsPlayingRootMotion()) {
			Jump();
		}
	}
}


// Handle stance change control input 
void ATPBaseCharacter::StanceAction() {
	auto CharMov = GetTPBaseMovement();
	if (!CharMov) { return; }

	if (CharMov->IsMovingOnGround()) {
		if (CharMov->IsCrouching()) {
			UnCrouch();
		}
		else {
			Crouch();
		}
	}
}


// Handle walk control input 
void ATPBaseCharacter::WalkAction() {

	auto CharMov = GetTPBaseMovement();
	if (CharMov) {
		if (IsSprinting()) {
			if (IsRunning()) { CameraGaitMode = EGaitMode::eRunning; }
			else { CameraGaitMode = EGaitMode::eWalking; }
			CharMov->WantsToSprint(false);
		}
		else {
			if (IsRunning()) { CameraGaitMode = EGaitMode::eWalking; }
			else { CameraGaitMode = EGaitMode::eRunning; }
			CharMov->WantsToRun(!IsRunning());
		}

		// Update camera
		if (IsAiming()) {
			UpdateCamera(CameraLerpCurves[2]);
		}
		else {
			UpdateCamera(CameraLerpCurves[1]);
		}
	}
}


// Handle sprint control input pressed
void ATPBaseCharacter::BeginSprint() {

	auto CharMov = GetTPBaseMovement();
	if (CharMov) {
		if (bToggleSprint) {
			if (IsSprinting()) {
				if (IsRunning()) { CameraGaitMode = EGaitMode::eRunning; }
				else { CameraGaitMode = EGaitMode::eWalking; }
			}
			else { CameraGaitMode = EGaitMode::eSprinting; }
			CharMov->WantsToSprint(!IsSprinting());
		}
		else {
			CameraGaitMode = EGaitMode::eSprinting;
			CharMov->WantsToSprint(true);
		}
		
		// Update camera
		if (IsAiming()) {
			UpdateCamera(CameraLerpCurves[2]);
		}
		else {
			UpdateCamera(CameraLerpCurves[1]);
		}
	}
}


// Handle sprint control input release
void ATPBaseCharacter::EndSprint() {

	auto CharMov = GetTPBaseMovement();
	if (CharMov && !bToggleSprint) {
		if (IsRunning()) { CameraGaitMode = EGaitMode::eRunning; }
		else { CameraGaitMode = EGaitMode::eWalking; }
		CharMov->WantsToSprint(false);
		
		// Update camera
		if (IsAiming()) {
			UpdateCamera(CameraLerpCurves[2]);
		}
		else {
			UpdateCamera(CameraLerpCurves[1]);
		}
	}
}


// Handle aim down sights control input 
void ATPBaseCharacter::BeginAiming() {
	SetAiming(true);
}


// Handle aim down sights control input 
void ATPBaseCharacter::EndAiming() {
	SetAiming(false);
}


// Handle rotation mode control input 
void ATPBaseCharacter::SwitchRotationMode() {
	auto CharMov = GetTPBaseMovement();
	if (!CharMov) { return; }
	if (GetRotationMode() == ERotationMode::eLookingMode) {
		SetRotationMode(ERotationMode::eVelocityMode);
	}
	else {
		SetRotationMode(ERotationMode::eLookingMode);
	}
}


// Handle ragdoll mode control input
void ATPBaseCharacter::RagdollAction() {
	
	if (bIsRagdoll) {
		ExitRagdoll();
	}
	else {
		EnterRagdoll();
	}
}


// Handle forward foot control input 
void ATPBaseCharacter::SwitchForwardFoot() {
	SetForwardFoot(!bRightFootForward);
}


// Handle camera action control input 
void ATPBaseCharacter::BeginSwitchCamera() {
	if (!GetWorld()) { return; }

	// If input held for delay, switches camera mode
	float InputDelay = 0.5f;
	GetWorldTimerManager().SetTimer(InputTimer, this, &ATPBaseCharacter::SwitchCamera, 10.0f, false, InputDelay);
}


// Handle camera action control input 
void ATPBaseCharacter::EndSwitchCamera() {
	if (!GetWorld()) { return; }

	// Switch mode if time sufficient
	if (InputTimer.IsValid()) {
		float time = GetWorldTimerManager().GetTimerRemaining(InputTimer);
		GetWorldTimerManager().ClearTimer(InputTimer);
		if (time == -1) { return; }
	}

	// Switch shoulder view
	if (CameraMode == ECameraMode::eThirdPerson && GetRotationMode() == ERotationMode::eLookingMode) {
		SetRightShoulder(!bRightShoulder);
		UpdateCamera(CameraLerpCurves[3]);
	}
}


// Handle camera action control input 
void ATPBaseCharacter::SwitchCamera() {
	if (CameraMode == ECameraMode::eFirstPerson) {
		SetCameraMode(ECameraMode::eThirdPerson);
	}
	else {
		SetCameraMode(ECameraMode::eFirstPerson);
	}
}



////////////////////////////////////////////////////////////////////
//
//   Character getter functions, based on state
//
////////////////////////////////////////////////////////////////////

// Get camera settings based on character animation state
FCameraSettings ATPBaseCharacter::GetCameraTargetSettings() const {

	// Ragdolling
	if (bIsRagdoll) { return CameraTargets.Ragdoll;	}

	// Aiming
	if (IsAiming()) { return CameraTargets.Aiming; }

	// Locomotion mode
	ELocomotionMode LocomotionMode = GetLocomotionMode();
	if(LocomotionMode == ELocomotionMode::eGrounded || LocomotionMode == ELocomotionMode::eFalling) {

		// Looking rotation mode
		float MySpeed = GetCharacterVelocity().Size();
		if (GetRotationMode() == ERotationMode::eLookingMode) {
			if (GetStance() == EStance::eCrouching) { return CameraTargets.LookingMode.Crouching; }
			else {
				if (CameraGaitMode == EGaitMode::eSprinting) { return CameraTargets.LookingMode.Standing.Sprint; }
				if (CameraGaitMode == EGaitMode::eRunning) { return CameraTargets.LookingMode.Standing.Run; }
				return CameraTargets.LookingMode.Standing.Walk;
			}
		}

		// Velocity rotation mode
		else {
			if (GetStance() == EStance::eCrouching) { return CameraTargets.VelocityMode.Crouching; }
			else {
				if (CameraGaitMode == EGaitMode::eSprinting) { return CameraTargets.VelocityMode.Standing.Sprint; }
				if (CameraGaitMode == EGaitMode::eRunning) { return CameraTargets.VelocityMode.Standing.Run; }
				return CameraTargets.VelocityMode.Standing.Walk;
			}
		}
	}

	// Default
	return CameraTargets.LookingMode.Standing.Walk;
}


// Get character velocity based on animation state 
FVector ATPBaseCharacter::GetCharacterVelocity() const {
	if (bIsRagdoll) { return GetMesh()->GetPhysicsLinearVelocity(PelvisBoneName); }
	return GetVelocity();
}


// Determine rotation rate from character state 
float ATPBaseCharacter::GetCharacterRotationRate(float SlowSpeed, float SlowRate, float FastSpeed, float FastRate) {
	float Length2D = GetCharacterVelocity().Size2D();
	float RotationRate;

	float MappedLength;
	if (Length2D > SlowSpeed) {
		MappedLength = FMath::GetMappedRangeValueUnclamped(FVector2D(SlowSpeed, FastSpeed), FVector2D(SlowRate, FastRate), Length2D);
	}
	else {
		MappedLength = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, SlowSpeed), FVector2D(0.0f, SlowRate), Length2D);
	}
	RotationRate = FMath::Clamp(RotationRateMultiplier * MappedLength, 0.1f, 15.0f);

	if (RotationRateMultiplier != 1.0f) {
		RotationRateMultiplier = FMath::Clamp(RotationRateMultiplier + GetWorld()->GetDeltaSeconds(), 0.0f, 1.0f);
	}

	return RotationRate;
}


// Get current aiming state 
bool ATPBaseCharacter::IsAiming() const {
	auto CharMov = GetTPBaseMovement();
	if (!CharMov) { return false; }
	return CharMov->IsAiming();
}


// Get current running state 
bool ATPBaseCharacter::IsRunning() const {
	auto CharMov = GetTPBaseMovement();
	if (!CharMov) { return false; }
	return CharMov->IsRunning();
}


// Get current sprinting state 
bool ATPBaseCharacter::IsSprinting() const {
	auto CharMov = GetTPBaseMovement();
	if (!CharMov) { return false; }
	return CharMov->IsSprinting();
}


// Get current stance 
EStance ATPBaseCharacter::GetStance() const {
	auto CharMov = GetTPBaseMovement();
	if (!CharMov) { return EStance::eStanding; }
	return CharMov->GetStance();
}


// Get current gait mode 
EGaitMode ATPBaseCharacter::GetGaitMode() const {
	auto CharMov = GetTPBaseMovement();
	if (!CharMov) { return EGaitMode::eNone; }
	return CharMov->GetGaitMode();
}


// Get current rotation mode 
ERotationMode ATPBaseCharacter::GetRotationMode() const {
	auto CharMov = GetTPBaseMovement();
	if (!CharMov) { return ERotationMode::eLookingMode; }
	return CharMov->GetRotationMode();
}


// Get current locomotion mode 
ELocomotionMode ATPBaseCharacter::GetLocomotionMode() const {
	auto CharMov = GetTPBaseMovement();
	if (!CharMov) { return ELocomotionMode::eNone; }
	return CharMov->GetLocomotionMode();
}


// Get crouched speed configuration input 
float ATPBaseCharacter::GetCrouchingSpeed() const {
	auto CharMov = GetTPBaseMovement();
	if (!CharMov) { return 0.0f; }
	return CharMov->GetCrouchSpeed();
}


// Get walking speed configuration input 
float ATPBaseCharacter::GetWalkingSpeed() const {
	auto CharMov = GetTPBaseMovement();
	if (!CharMov) { return 0.0f; }
	return CharMov->GetWalkSpeed();
}


// Get running speed configuration input 
float ATPBaseCharacter::GetRunningSpeed() const {
	auto CharMov = GetTPBaseMovement();
	if (!CharMov) { return 0.0f; }
	return CharMov->GetRunSpeed();
}


// Get sprinting speed configuration input 
float ATPBaseCharacter::GetSprintingSpeed() const {
	auto CharMov = GetTPBaseMovement();
	if (!CharMov) { return 0.0f; }
	return CharMov->GetSprintSpeed();
}


// Get walk acceleration configuration input 
float ATPBaseCharacter::GetWalkAcceleration() const {
	auto CharMov = GetTPBaseMovement();
	if (!CharMov) { return 0.0f; }
	return CharMov->GetWalkAcceleration();
}


// Get run acceleration configuration input 
float ATPBaseCharacter::GetRunAcceleration() const {
	auto CharMov = GetTPBaseMovement();
	if (!CharMov) { return 0.0f; }
	return CharMov->GetRunAcceleration();
}


// Get walk deceleration configuration input 
float ATPBaseCharacter::GetWalkDeceleration() const {
	auto CharMov = GetTPBaseMovement();
	if (!CharMov) { return 0.0f; }
	return CharMov->GetWalkDeceleration();
}


// Get run deceleration configuration input 
float ATPBaseCharacter::GetRunDeceleration() const {
	auto CharMov = GetTPBaseMovement();
	if (!CharMov) { return 0.0f; }
	return CharMov->GetRunDeceleration();
}


// Get walk ground friction configuration input 
float ATPBaseCharacter::GetWalkGroundFriction() const {
	auto CharMov = GetTPBaseMovement();
	if (!CharMov) { return 0.0f; }
	return CharMov->GetWalkGroundFriction();
}


// Get run ground friction configuration input 
float ATPBaseCharacter::GetRunGroundFriction() const {
	auto CharMov = GetTPBaseMovement();
	if (!CharMov) { return 0.0f; }
	return CharMov->GetRunGroundFriction();
}


////////////////////////////////////////////////////////////////////
//
//   Character setter functions
//
////////////////////////////////////////////////////////////////////

// Set the character rotation 
void ATPBaseCharacter::SetCharacterRotation(FRotator NewRotation, bool bDoInterp, float InterpSpeed) {

	TargetRotation = NewRotation.GetNormalized();
	RotationDifferential = (TargetRotation - CharacterRotation).GetNormalized().Yaw;
	if (bDoInterp && InterpSpeed != 0.0f) {
		CharacterRotation = FMath::RInterpTo(CharacterRotation, NewRotation, GetWorld()->GetDeltaSeconds(), InterpSpeed);
		CharacterRotation.Normalize();
	}
	else { 
		CharacterRotation = NewRotation.GetNormalized();
	}
	SetActorRotation(CharacterRotation);
}


// Add given amount to the character rotation 
void ATPBaseCharacter::AddCharacterRotation(FRotator AdditiveRotation) {

	TargetRotation = (AdditiveRotation + TargetRotation).GetNormalized();
	RotationDifferential = (TargetRotation - CharacterRotation).GetNormalized().Yaw;
	CharacterRotation = (AdditiveRotation + CharacterRotation).GetNormalized();
	SetActorRotation(CharacterRotation);
}


// Limit amount of yaw per frame 
void ATPBaseCharacter::LimitCharacterRotation(float AimYawLimit, float InterpSpeed) {

	float Yaw = LookRotation.Yaw + ((AimYawDelta > 0.0f) ? -AimYawLimit : AimYawLimit);

	if (FMath::Abs(AimYawDelta) > AimYawLimit) {
		SetCharacterRotation(FRotator(0.0f, Yaw, 0.0f), true, InterpSpeed);
	}
}


// Called to update camera view mode 
void ATPBaseCharacter::SetCameraMode(ECameraMode NewMode) {
	if (CameraMode == NewMode) { return; }

	// Should toggle
	switch (NewMode) {
	case ECameraMode::eFirstPerson:
		CameraMode = NewMode;
		FP_Camera->SetActive(true);
		TP_Camera->SetActive(false);
		if (GetRotationMode() == ERotationMode::eVelocityMode) {
			SetRotationMode(ERotationMode::eLookingMode);
		}
		break;

	case ECameraMode::eThirdPerson:
		CameraMode = NewMode;
		FP_Camera->SetActive(false);
		TP_Camera->SetActive(true);
		break;
	}
}


// Interface callback for updating forward foot 
bool ATPBaseCharacter::ServerSetForwardFoot_Validate(bool bRightFoot) {
	return true;
}

void ATPBaseCharacter::ServerSetForwardFoot_Implementation(bool bRightFoot) {
	SetForwardFoot(bRightFoot);
}

void ATPBaseCharacter::SetForwardFoot(bool bRightFoot) {
	if (!HasAuthority()) {
		ServerSetForwardFoot(bRightFoot);
	}
	bRightFootForward = bRightFoot;
}


// Interface callback for updating dead state 
void ATPBaseCharacter::ServerSetIsDead_Implementation(bool bDead) {
	SetIsDead(bDead);
}

bool ATPBaseCharacter::ServerSetIsDead_Validate(bool bDead) {
	return true;
}

void ATPBaseCharacter::SetIsDead(bool bDead) {
	if (!HasAuthority()) {
		ServerSetIsDead(bDead);
	}
	bIsDead = bDead;
}


// Interface callback for updating camera view shoulder 
void ATPBaseCharacter::SetRightShoulder(bool bRight) {
	bRightShoulder = bRight;
}


// Interface callback for updating aiming state
void ATPBaseCharacter::SetAiming(bool bAiming) {

	auto CharMov = GetTPBaseMovement();
	if (!CharMov) { return; }

	if (CharMov->IsAiming() == bAiming) {
		return; 
	}
	CharMov->WantsToAim(bAiming);

	// Store rotation mode for switch back
	ERotationMode RotationMode = GetRotationMode();
	static ERotationMode LastRotationMode = RotationMode;
	if (bAiming) {
		LastRotationMode = RotationMode;
		if (RotationMode == ERotationMode::eVelocityMode) {
			SetRotationMode(ERotationMode::eLookingMode);
		}
	}
	else {
		if (RotationMode != LastRotationMode) {
			SetRotationMode(LastRotationMode);
		}
	}

	// Update camera
	if (bAiming) {
		UpdateCamera(CameraLerpCurves[3]);
	}
	else {
		UpdateCamera(CameraLerpCurves[1]);
	}
}


// Called to update character rotation mode 
void ATPBaseCharacter::SetRotationMode(ERotationMode NewMode) {
	ERotationMode RotationMode = GetRotationMode();
	if (RotationMode == NewMode) { return; }

	auto CharMov = GetTPBaseMovement();
	if (!CharMov) { return; }

	if (bIsMoving) {
		RotationRateMultiplier = 0.0f;
	}

	if (RotationMode == ERotationMode::eVelocityMode && CameraMode == ECameraMode::eFirstPerson) {
		SetCameraMode(ECameraMode::eThirdPerson);
	}
	UpdateCamera(CameraLerpCurves[1]);
}


// Called to update character locomotion mode 
void ATPBaseCharacter::SetLocomotionMode(ELocomotionMode NewMode) {
	ELocomotionMode LocomotionMode = GetLocomotionMode();
	if (LocomotionMode == NewMode) { return; }

	PrevLocomotionMode = LocomotionMode;
	LocomotionMode = NewMode;

	// Set Jump rotation and update camera
	switch (PrevLocomotionMode) {
	case ELocomotionMode::eGrounded:
		if (bIsMoving) {
			JumpRotation = PrevVelocityRotation;
		}
		else {
			JumpRotation = CharacterRotation;
		}
		RotationOffset = 0.0f;
		break; 
	case ELocomotionMode::eRagdoll:
		JumpRotation = CharacterRotation;
		break;
	}
	UpdateCamera(CameraLerpCurves[2]);
}


// Called to update character crouching speed 
void ATPBaseCharacter::SetCrouchSpeed(float NewSpeed) {
	auto CharMov = GetTPBaseMovement();
	if (CharMov && CharMov->GetCrouchSpeed() != NewSpeed) {
		CharMov->SetCrouchSpeed(NewSpeed);
	}
}


// Called to update character walking speed 
void ATPBaseCharacter::SetWalkSpeed(float NewSpeed) {
	auto CharMov = GetTPBaseMovement();
	if (CharMov && CharMov->GetWalkSpeed() != NewSpeed) {
		CharMov->SetWalkSpeed(NewSpeed);
	}
}


// Called to update character running speed 
void ATPBaseCharacter::SetRunSpeed(float NewSpeed) {
	auto CharMov = GetTPBaseMovement();
	if (CharMov && CharMov->GetRunSpeed() != NewSpeed) {
		CharMov->SetRunSpeed(NewSpeed);
	}
}


// Called to update character sprinting speed 
void ATPBaseCharacter::SetSprintSpeed(float NewSpeed) {
	auto CharMov = GetTPBaseMovement();
	if (CharMov && CharMov->GetSprintSpeed() != NewSpeed) {
		CharMov->SetSprintSpeed(NewSpeed);
	}
}



////////////////////////////////////////////////////////////////////
//
//   Character utility functions
//
////////////////////////////////////////////////////////////////////

// Camera lerp from current to target positions
void ATPBaseCharacter::CameraLerpCallback(float Alpha) {
	FCameraSettings Target = GetCameraTargetSettings();

	float FOV = FMath::Lerp(CurrentCameraSettings.FieldOfView, Target.FieldOfView, Alpha);
	float TAL = FMath::Lerp(CurrentCameraSettings.TargetArmLength, Target.TargetArmLength, Alpha);
	float LAG = FMath::Lerp(CurrentCameraSettings.LagSpeed, Target.LagSpeed, Alpha);

	if (!bRightShoulder) {
		Target.SocketOffset *= FVector(1.0f, -1.0f, 1.0f);
	}
	FVector SO = FMath::Lerp(CurrentCameraSettings.SocketOffset, Target.SocketOffset, Alpha);

	TP_Camera->SetFieldOfView(FOV);
	TP_SpringArm->TargetArmLength = TAL;
	TP_SpringArm->CameraLagSpeed = LAG;
	TP_SpringArm->SocketOffset = SO;
}


// Updates capsule and arrow visibility 
void ATPBaseCharacter::UpdateCapsuleVisibility() {
	GetCapsuleComponent()->SetHiddenInGame(!bShowSettings);
	Arrows->SetHiddenInGame(!bShowSettings, true);
}


// Called to display debug settings 
bool ATPBaseCharacter::ServerSetShowSettings_Validate(bool bShow) {
	return true;
}

void ATPBaseCharacter::ServerSetShowSettings_Implementation(bool bShow) {
	SetShowSettings(bShow);
}

void ATPBaseCharacter::SetShowSettings(bool bShow) {
	if (!HasAuthority()) {
		ServerSetShowSettings(bShow);
	}
	bShowSettings = bShow;
}


// Called to display debug traces 
bool ATPBaseCharacter::ServerSetShowTraces_Validate(bool bShow) {
	return true;
}

void ATPBaseCharacter::ServerSetShowTraces_Implementation(bool bShow) {
	SetShowTraces(bShow);
}

void ATPBaseCharacter::SetShowTraces(bool bShow) {
	if (!HasAuthority()) {
		ServerSetShowTraces(bShow);
	}
	bShowTraces = bShow;
}


// Displays character state information on screen 
void ATPBaseCharacter::PrintCharacterInfo() {

	if (!IsLocallyControlled() || !PlayerInfoWidget) { return;	}

	TArray<FString> PlayerInfo;
	auto CharMov = GetTPBaseMovement();
	if (!CharMov) { return; }

	// No display
	if (!bShowSettings) {
		PlayerInfoWidget->SetColor(FColor::Black);
		PlayerInfo.Add(FString::Printf(TEXT("Press 'TAB' to show character info (%s)"), *GetName()));
		PlayerInfoWidget->SetPlayerInfo(PlayerInfo);
		return;
	}

	// Debug text
	float Speed = GetCharacterVelocity().Size();
	float JumpVelocity = GetCharacterMovement()->JumpZVelocity;
	float AirControl = GetCharacterMovement()->AirControl;

	PlayerInfoWidget->SetColor(FColor::Black);
	PlayerInfo.Add(FString::Printf(TEXT("Press 'TAB' to hide character info (%s)"), *GetName()));
	PlayerInfo.Add(TEXT("\nHealth State\n---------------------"));
	PlayerInfoWidget->SetPlayerInfo(PlayerInfo);

	PlayerInfo.Empty();
	PlayerInfoWidget->SetColor(FColor::Orange);
	PlayerInfo.Add(FString::Printf(TEXT("Health: %.0f"), HealthComponent->GetHealth()));
	PlayerInfo.Add(FString::Printf(TEXT("Shield: %.0f"), HealthComponent->GetShield()));
	PlayerInfoWidget->AddPlayerInfo(PlayerInfo);

	PlayerInfo.Empty();
	PlayerInfoWidget->SetColor(FColor::Black);
	PlayerInfo.Add(TEXT("\nLocomotion Settings\n---------------------"));
	PlayerInfoWidget->AddPlayerInfo(PlayerInfo);

	PlayerInfo.Empty();
	PlayerInfoWidget->SetColor(FColor::Yellow);
	PlayerInfo.Add(FString::Printf(TEXT("Aiming: %s"), IsAiming() ? *FString("True") : *FString("False")));
	PlayerInfo.Add(FString::Printf(TEXT("Rotation Mode: %s"), *Enum2Str("ERotationMode", GetRotationMode())));
	PlayerInfo.Add(FString::Printf(TEXT("Stance: %s"), *Enum2Str("EStance", GetStance())));
	PlayerInfo.Add(FString::Printf(TEXT("Gait Mode: %s - %.0f"), *Enum2Str("EGaitMode", GetGaitMode()), Speed));
	PlayerInfo.Add(FString::Printf(TEXT("Locomotion Mode: %s"), *Enum2Str("ELocomotionMode", GetLocomotionMode())));
	PlayerInfoWidget->AddPlayerInfo(PlayerInfo);

	PlayerInfo.Empty();
	PlayerInfoWidget->SetColor(FColor::Black);
	PlayerInfo.Add(TEXT("\nMovement Settings\n---------------------"));
	PlayerInfoWidget->AddPlayerInfo(PlayerInfo);

	PlayerInfo.Empty();
	PlayerInfoWidget->SetColor(FColor::Green);
	PlayerInfo.Add(FString::Printf(TEXT("Walking Speed: %.0f  [CTRL + Right/Left]"), GetWalkingSpeed()));
	PlayerInfo.Add(FString::Printf(TEXT("Running Speed: %.0f  [Right/Left]"), GetRunningSpeed()));
	PlayerInfo.Add(FString::Printf(TEXT("Sprinting Speed: %.0f  [SHIFT + Right/Left]"), GetSprintingSpeed()));
	PlayerInfo.Add(FString::Printf(TEXT("Crouching Speed: %.0f  [ALT + Right/Left]"), GetCrouchingSpeed()));
	PlayerInfoWidget->AddPlayerInfo(PlayerInfo);

	PlayerInfo.Empty();
	PlayerInfoWidget->SetColor(FColor::Blue);
	PlayerInfo.Add(FString::Printf(TEXT("\nJump Velocity: %.0f  [Up/Down]"), JumpVelocity));
	PlayerInfo.Add(FString::Printf(TEXT("Air Control: %.1f  [CTRL + Up/Down]"), AirControl));
	PlayerInfo.Add(FString::Printf(TEXT("Jump Max Hold Time: %.2f  [SHIFT + Up/Down]"), JumpMaxHoldTime));
	PlayerInfoWidget->AddPlayerInfo(PlayerInfo);

	PlayerInfo.Empty();
	PlayerInfoWidget->SetColor(FColor::Black);
	PlayerInfo.Add(TEXT("\nDebug Settings\n---------------------"));
	PlayerInfoWidget->AddPlayerInfo(PlayerInfo);

	PlayerInfo.Empty();
	PlayerInfoWidget->SetColor(FColor::Red);
	PlayerInfo.Add(FString::Printf(TEXT("Show Traces: %s  [T]"), bShowTraces ? *FString("True") : *FString("False")));
	PlayerInfo.Add(TEXT("Show Collision  [V]"));
	PlayerInfo.Add(TEXT("Show Bones  [B]"));
	PlayerInfo.Add(TEXT("Mesh Visibility  [M]"));
	PlayerInfoWidget->AddPlayerInfo(PlayerInfo);

	PlayerInfo.Empty();
	PlayerInfoWidget->SetColor(FColor::Black);
	PlayerInfo.Add(TEXT("\n---------------------"));
	PlayerInfoWidget->AddPlayerInfo(PlayerInfo);

	PlayerInfo.Empty();
	PlayerInfoWidget->SetColor(FColor::Magenta);
	PlayerInfo.Add(TEXT("Restart Level  [R]"));
	PlayerInfoWidget->AddPlayerInfo(PlayerInfo);
}


// Interface callback for updating camera position
void ATPBaseCharacter::UpdateCamera(UCurveFloat* LerpCurve) {

	CurrentCameraSettings.Setup(
		TP_SpringArm->TargetArmLength,
		TP_SpringArm->CameraLagSpeed,
		TP_SpringArm->SocketOffset,
		TP_Camera->FieldOfView);

	if (CameraLerpTimeline){
		if (LerpCurve) {
			CameraLerpTimeline->AddInterpFloat(LerpCurve, onCameraLerpCallback);
			CameraLerpTimeline->PlayFromStart();
		}
		else {
			CameraLerpTimeline->AddInterpFloat(CameraLerpCurves[0], onCameraLerpCallback);
			CameraLerpTimeline->PlayFromStart();
		}
	}
}


// Create arrow scene components
void ATPBaseCharacter::CreateArrowComponents() {

	float Height = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	// Arrow components
	Arrows = CreateDefaultSubobject<USceneComponent>(TEXT("Arrows"));
	if (Arrows) {
		Arrows->SetupAttachment(RootComponent);

		// Looking direction vector
		LookingRotationArrow = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("LookingRotationArrow"));
		if (LookingRotationArrow) {
			LookingRotationArrow->ArrowColor = FColor(0, 214, 255);
			LookingRotationArrow->ArrowSize = 0.75f;
			LookingRotationArrow->bUseInEditorScaling = false;
			LookingRotationArrow->SetRelativeLocation(FVector(0.0f, 0.0f, Height));
			LookingRotationArrow->SetupAttachment(Arrows);
		}

		// Target direction vector
		TargetRotationArrow = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("TargetRotationArrow"));
		if (TargetRotationArrow) {
			TargetRotationArrow->ArrowColor = FColor(255, 156, 0);
			TargetRotationArrow->ArrowSize = 0.75f;
			TargetRotationArrow->bUseInEditorScaling = false;
			TargetRotationArrow->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
			TargetRotationArrow->SetupAttachment(Arrows);
		}

		// Character direction vector
		CharacterRotationArrow = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("CharacterRotationArrow"));
		if (CharacterRotationArrow) {
			CharacterRotationArrow->ArrowColor = FColor(0, 255, 0);
			CharacterRotationArrow->ArrowSize = 0.75f;
			CharacterRotationArrow->bUseInEditorScaling = false;
			CharacterRotationArrow->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
			CharacterRotationArrow->SetupAttachment(Arrows);
		}

		// Movement input direction vector
		MovementInputArrow = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("MovementInputArrow"));
		if (MovementInputArrow) {
			MovementInputArrow->ArrowColor = FColor(255, 255, 0);
			MovementInputArrow->bUseInEditorScaling = false;
			MovementInputArrow->SetRelativeLocation(FVector(0.0f, 0.0f, -Height + 0.8f));
			MovementInputArrow->SetRelativeScale3D(FVector(1.0f, 1.75f, 0.0f));
			MovementInputArrow->SetupAttachment(Arrows);
		}

		// Previous movement input vector
		PrevMovementInputArrow = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("LastMovementInputRotationArrow"));
		if (PrevMovementInputArrow) {
			PrevMovementInputArrow->ArrowColor = FColor(137, 137, 0);
			PrevMovementInputArrow->bUseInEditorScaling = false;
			PrevMovementInputArrow->SetRelativeLocation(FVector(0.0f, 0.0f, -HalfHeight + 0.6f));
			PrevMovementInputArrow->SetRelativeScale3D(FVector(1.0f, 1.75f, 0.0f));
			PrevMovementInputArrow->SetupAttachment(Arrows);
		}

		// Velocity direction vector
		VelocityArrow = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("VelocityArrow"));
		if (VelocityArrow) {
			VelocityArrow->ArrowColor = FColor(255, 0, 255);
			VelocityArrow->bUseInEditorScaling = false;
			VelocityArrow->SetRelativeLocation(FVector(0.0f, 0.0f, -Height + 0.4f));
			VelocityArrow->SetRelativeScale3D(FVector(1.0f, 4.0f, 0.0f));
			VelocityArrow->SetupAttachment(Arrows);
		}

		// Previous velocity vector
		PrevVelocityArrow = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("LastVelocityRotationArrow"));
		if (PrevVelocityArrow) {
			PrevVelocityArrow->ArrowColor = FColor(137, 0, 137);
			PrevVelocityArrow->bUseInEditorScaling = false;
			PrevVelocityArrow->SetRelativeLocation(FVector(0.0f, 0.0f, -Height + 0.2f));
			PrevVelocityArrow->SetRelativeScale3D(FVector(1.0f, 4.0f, 0.0f));
			PrevVelocityArrow->SetupAttachment(Arrows);
		}
		Arrows->SetVisibility(true, true);
		Arrows->SetHiddenInGame(false, true);
	}
}


// Called to update arrow positions 
void ATPBaseCharacter::UpdateArrowPositions() {
	float Scale = GetCapsuleComponent()->GetScaledCapsuleHalfHeight() / HalfHeight;
	Arrows->SetRelativeScale3D(FVector(1.0f, 1.0f, Scale));
}


// Called to update arrow vectors 
void ATPBaseCharacter::UpdateArrowComponents(bool bAlwaysUpdate) {
	if (!Arrows->IsVisible() && !bAlwaysUpdate) { return; }
	
	// Character rotations
	CharacterRotationArrow->SetWorldRotation(CharacterRotation);
	LookingRotationArrow->SetWorldRotation(LookRotation);
	TargetRotationArrow->SetWorldRotation(TargetRotation);

	// Control rotations
	MovementInputArrow->SetWorldRotation(MovementInput.Rotation());
	FVector MScale(FMath::Clamp(MovementInput.Size(), 0.0f, 1.0f), 1.75f, 0.0f);
	MovementInputArrow->SetRelativeScale3D(MScale);

	PrevMovementInputArrow->SetWorldRotation(PrevMovementRotation);

	// Velocity rotation
	FVector MyVelocity = GetCharacterVelocity();
	VelocityArrow->SetWorldRotation(MyVelocity.Rotation());
	float ClampedLength = FMath::GetMappedRangeValueClamped(
		FVector2D(0.0f, GetCharacterMovement()->GetMaxSpeed()),
		FVector2D(0.0f, 1.0f),
		MyVelocity.Size()
	);
	VelocityArrow->SetRelativeScale3D(FVector(ClampedLength, 4.0f, 0.0f));

	PrevVelocityArrow->SetWorldRotation(FRotator(0.0f, PrevVelocityRotation.Yaw, 0.0f));
}



////////////////////////////////////////////////////////////////////
//
//   Character calculation / management functions
//
////////////////////////////////////////////////////////////////////

// Handles adjustments to character rotation, performed in Tick 
void ATPBaseCharacter::ManageCharacterRotation() {
	
	// Handle if ragdolling
	if (bIsRagdoll) {
		ManageRagdoll();
		return;
	}

	// Locomotion mode
	ERotationMode RotationMode = GetRotationMode();
	switch (GetLocomotionMode()) {

		// Handle if grounded
	case ELocomotionMode::eGrounded:
		if (bIsMoving) {
			float InterpSpeed;
			FRotator Looking = CalculateLookingDirection(60.0f, -60.0f, 120.0f, -120.0f, 5.0f, 5.0f);

			switch (RotationMode) {
			case ERotationMode::eVelocityMode:
				InterpSpeed = GetCharacterRotationRate(165.0f, 5.0f, 375.0f, 10.0f);
				SetCharacterRotation(FRotator(0.0f, PrevVelocityRotation.Yaw, 0.0f), true, InterpSpeed);
				break;
			case ERotationMode::eLookingMode:
				if (IsAiming()) {
					InterpSpeed = GetCharacterRotationRate(165.0f, 15.0f, 375.0f, 15.0f);
				}
				else {
					InterpSpeed = GetCharacterRotationRate(165.0f, 10.0f, 375.0f, 15.0f);
				}
				SetCharacterRotation(Looking, true, InterpSpeed);
				break;
			}
		}
		else if (!IsPlayingRootMotion() && RotationMode == ERotationMode::eLookingMode) {
			if (IsAiming()) {
				LimitCharacterRotation(90.0f, 15.0f);
			}
			else if (CameraMode == ECameraMode::eFirstPerson) {
				LimitCharacterRotation(90.0f, 15.0f);
			}
		}
		break;

		// Handle if falling
	case ELocomotionMode::eFalling:
		switch (RotationMode) {
		case ERotationMode::eVelocityMode:
			if (bIsMoving) {
				SetCharacterRotation(FRotator(0.0f, JumpRotation.Yaw, 0.0f), true, 10.0f);
			}
			break;
		case ERotationMode::eLookingMode:
			JumpRotation = LookRotation;
			SetCharacterRotation(FRotator(0.0f, JumpRotation.Yaw, 0.0f), true, 10.0f);
			break;
		}
		break;

	}
}


// Calculate the character current state
void ATPBaseCharacter::CalculateStateVariables() {

	CharacterRotation = GetActorRotation();

	// Difference between velocity and character
	FVector MyVelocity = GetCharacterVelocity();
	float MySpeed = MyVelocity.Size2D();
	bIsMoving = MySpeed > 1.0f;
	if (bIsMoving) {
		PrevVelocityRotation = MyVelocity.Rotation().GetNormalized();
		YawDifferential = (PrevVelocityRotation - CharacterRotation).GetNormalized().Yaw;
	}

	// Difference between movement input and velocity
	MovementInput = GetTPBaseMovement()->GetMovementInput();
	bHasMovementInput = MovementInput.Size2D() > 0.001f;
	if (bHasMovementInput) {
		PrevMovementRotation = MovementInput.Rotation().GetNormalized();
		MovementDifferential = (PrevMovementRotation - PrevVelocityRotation).GetNormalized().Yaw;
	}

	// Difference between looking and character
	float PrevAimYaw = LookRotation.Yaw;
	LookRotation = GetTPBaseMovement()->GetControlRotation();
	AimYawRate = (LookRotation.Yaw - PrevAimYaw) / GetWorld()->GetDeltaSeconds();
	AimYawDelta = (LookRotation - CharacterRotation).GetNormalized().Yaw;

	// TODO: Refactor to function
	// Camera gait mode
	if (CameraGaitMode == EGaitMode::eSprinting) {
		if (MySpeed < GetWalkingSpeed()) {
			CameraGaitMode = EGaitMode::eWalking;
			UpdateCamera(CameraLerpCurves[2]);
		}
		else if (MySpeed < GetRunningSpeed()) {
			CameraGaitMode = EGaitMode::eRunning;
			UpdateCamera(CameraLerpCurves[2]);
		}
	}
	else if (CameraGaitMode == EGaitMode::eRunning) {
		if (MySpeed > GetRunningSpeed()) {
			CameraGaitMode = EGaitMode::eSprinting;
			UpdateCamera(CameraLerpCurves[2]);
		}
		else if (MySpeed < GetWalkingSpeed()) {
			CameraGaitMode = EGaitMode::eWalking;
			UpdateCamera(CameraLerpCurves[2]);
		}
	}
	else {
		if (MySpeed > GetRunningSpeed()) {
			CameraGaitMode = EGaitMode::eSprinting;
			UpdateCamera(CameraLerpCurves[2]);
		}
		else if (MySpeed > GetWalkingSpeed()) {
			CameraGaitMode = EGaitMode::eRunning;
			UpdateCamera(CameraLerpCurves[2]);
		}
	}
}


// Determines the looking yaw offset 
FRotator ATPBaseCharacter::CalculateLookingDirection(float NEAngle, float NWAngle, float SEAngle, float SWAngle, float Buffer, float InterpSpeed) {

	// TODO: Remove CardinalDirection from global variables ?

	// Yaw deviation between look and control rotators
	float dYaw = bHasMovementInput ?
		(PrevMovementRotation - LookRotation).GetNormalized().Yaw :
		(PrevVelocityRotation - LookRotation).GetNormalized().Yaw;

		// Determine the sector of yaw deviation
		if (WithinCardinalRange(dYaw, NWAngle, NEAngle, Buffer, ECardinalDirection::eNorth)) {
			CardinalDirection = ECardinalDirection::eNorth;
		}
		else if (WithinCardinalRange(dYaw, NEAngle, SEAngle, Buffer, ECardinalDirection::eEast)) {
			CardinalDirection = ECardinalDirection::eEast;
		}
		else if (WithinCardinalRange(dYaw, SWAngle, NWAngle, Buffer, ECardinalDirection::eWest)) {
			CardinalDirection = ECardinalDirection::eWest;
		}
		else {
			CardinalDirection = ECardinalDirection::eSouth;
		}

	// Adjust based on cardinal direction
	switch (CardinalDirection) {
	case ECardinalDirection::eEast:
		dYaw -= 90.0f;
		break;
	case ECardinalDirection::eSouth:
		dYaw += (dYaw > 0) ? -180.0f : 180.0f;
		break;
	case ECardinalDirection::eWest:
		dYaw += 90.0f;
		break;
	}

	// Adjust for walking
	auto CharMov = GetTPBaseMovement();
	if (!IsAiming() && GetGaitMode() == EGaitMode::eWalking) {
		dYaw = 0.0f;
	}

	// Interpolate
	RotationOffset = FMath::FInterpTo(RotationOffset, dYaw, GetWorld()->GetDeltaSeconds(), InterpSpeed);

	FRotator Look(0.0f, LookRotation.Yaw + RotationOffset, 0.0f);
	return Look;
}


// Determines if within range of given cardinal direction with a tolerance 
float ATPBaseCharacter::WithinCardinalRange(float Value, float Min, float Max, float Tol, ECardinalDirection Cardinal) {
	bool bInRange;
	if (Cardinal == CardinalDirection) {
		bInRange = (Value >= Min - Tol) && (Value <= Max + Tol) ? true : false;
	}
	else {
		bInRange = (Value >= Min + Tol) && (Value <= Max - Tol) ? true : false;
	}
	return bInRange;
}


// Handle animNotify turn in place event 
void ATPBaseCharacter::DelayedRotation_Notify(FRotator AdditiveRotation, float DelayTime) {

	// No delay
	if (DelayTime == 0.0f) {
		AddCharacterRotation(AdditiveRotation);
		return;
	}

	// Delay character rotation
	FTimerHandle DelayTimer;
	GetWorld()->GetTimerManager().SetTimer(DelayTimer, [this, AdditiveRotation]() {
		AddCharacterRotation(AdditiveRotation);
		}, 1.0f, false, DelayTime);
}


// Handle animNotify camera shake event 
void ATPBaseCharacter::CameraShake_Notify(TSubclassOf<UCameraShake> ShakeClass, float ShakeScale) {
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC) {
		PC->ClientPlayCameraShake(ShakeClass, ShakeScale);
	}
}



////////////////////////////////////////////////////////////////////
//
//   Ragdoll management functions
//
////////////////////////////////////////////////////////////////////

// Handles the manipulation of the ragdoll state
void ATPBaseCharacter::ManageRagdoll() {

	// Velocity information
	FVector MyVelocity = GetCharacterVelocity();
	float ForceLimit = 0.0f;
	float DampingStrength = 0.25f;
	float SpringStrength = FMath::GetMappedRangeValueClamped(
		FVector2D(0.0f, 1000.0f),
		FVector2D(0.0f, 25000.0f),
		MyVelocity.Size());

	// Set mesh for ragdolling
	GetMesh()->SetAllMotorsAngularDriveParams(SpringStrength, DampingStrength, ForceLimit);

	if (IsLocallyControlled()) {
		if (MyVelocity.Z < -4000.0f) {
			GetMesh()->SetEnableGravity(false);
		}
		else {
			GetMesh()->SetEnableGravity(true);
		}

		// Set initial state
		FVector CharacterLocation;
		FRotator RagdollRotation;
		GetMesh()->GetSocketWorldLocationAndRotation(PelvisBoneName, RagdollLocation, RagdollRotation);

		// Perform collision line-trace
		bRagdollGrounded = RagdollLineTrace(RagdollLocation, RagdollRotation, CharacterLocation, CharacterRotation);

		// Update character from ragdoll
		TargetRotation = CharacterRotation;
		RotationDifferential = 0.0f;
		SetActorLocationAndRotation(CharacterLocation, CharacterRotation);

		// Network update
		ServerRagdollUpdate(bRagdollGrounded, RagdollLocation, CharacterLocation, CharacterRotation);
	}

	// Pushes pelvis into position, animation is on client
	else {
		FVector MyForce = (RagdollLocation - GetMesh()->GetSocketLocation(PelvisBoneName)) * 400.0f;
		GetMesh()->AddForce(MyForce, PelvisBoneName, true);
	}
}


// Called to update ragdoll state information 
void ATPBaseCharacter::MulticastRagdollUpdate_Implementation(bool bGrounded, FVector DollLocation, FVector NewLocation, FRotator NewRotation) {

	if (!IsLocallyControlled()) {
		RagdollLocation = DollLocation;
		bRagdollGrounded = bGrounded;
		TargetRotation = NewRotation.GetNormalized();
		CharacterRotation = TargetRotation;
		RotationDifferential = 0.0f;

		SetActorLocationAndRotation(NewLocation, CharacterRotation);
	}
}

void ATPBaseCharacter::ServerRagdollUpdate_Implementation(bool bGrounded, FVector DollLocation, FVector NewLocation, FRotator NewRotation) {
	MulticastRagdollUpdate(bGrounded, DollLocation, NewLocation, NewRotation);
}

bool ATPBaseCharacter::ServerRagdollUpdate_Validate(bool bGrounded, FVector DollLocation, FVector NewLocation, FRotator NewRotation) {
	return true;
}


// Called to enter character into ragdoll state 
void ATPBaseCharacter::MulticastEnterRagdoll_Implementation() {

	// Set ragdoll state
	bIsRagdoll = true;
	RagdollLocation = GetMesh()->GetSocketLocation(PelvisBoneName);

	// Set pawn movement
	SetReplicateMovement(false);
	GetCharacterMovement()->SetMovementMode(MOVE_None);

	// Update capsule and mesh
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionObjectType(ECC_PhysicsBody);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetAllBodiesBelowSimulatePhysics(PelvisBoneName, true, true);

	//UpdateCamera(CameraLerpCurves[2]);
}

void ATPBaseCharacter::ServerEnterRagdoll_Implementation() {
	EnterRagdoll();
}

bool ATPBaseCharacter::ServerEnterRagdoll_Validate() {
	return true;
}

void ATPBaseCharacter::EnterRagdoll() {
	if (!HasAuthority()) {
		ServerEnterRagdoll();
		return;
	}
	MulticastEnterRagdoll();
}


// Called to exit character from ragdoll state 
void ATPBaseCharacter::MulticastExitRagdoll_Implementation() {

	// Set pawn movement
	if (bRagdollGrounded) {
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
	else {
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	}
	GetCharacterMovement()->Velocity = GetCharacterVelocity();

	// Save pose for blending play get-up
	auto AInst = Cast<UTPBaseAnimInstance>(GetMesh()->GetAnimInstance());
	if (AInst) {
		AInst->SavePoseSnapshot(RagdollPoseName);

		// Play getup montage if grounded
		if (bRagdollGrounded) {
			if (GetMesh()->GetSocketRotation(PelvisBoneName).Roll > 0.0f) {
				AInst->Montage_Play(AInst->GetUpFront);
			}
			else {
				AInst->Montage_Play(AInst->GetUpBack);
			}
		}
	}

	// Update capsule and mesh
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetMesh()->SetAllBodiesSimulatePhysics(false);

	bIsRagdoll = false;
	SetReplicateMovement(true);
	UpdateCamera(CameraLerpCurves[2]);
}

void ATPBaseCharacter::ServerExitRagdoll_Implementation() {
	ExitRagdoll();
}

bool ATPBaseCharacter::ServerExitRagdoll_Validate() {
	return true;
}

void ATPBaseCharacter::ExitRagdoll() {
	if (!HasAuthority()) {
		ServerExitRagdoll();
		return;
	}
	MulticastExitRagdoll();
}


// Determines what it says it does :) 
bool ATPBaseCharacter::RagdollLineTrace(FVector InLocation, FRotator InRotation, FVector& OutLocation, FRotator& OutRotation) {

	// Keeps capsule facing proper direction if pelvis is inverted
	if (InRotation.Roll > 0.0f) {
		OutRotation = FRotator(0.0f, InRotation.Yaw, 0.0f);
	}
	else {
		OutRotation = FRotator(0.0f, InRotation.Yaw - 180.0f, 0.0f);
	}

	// Trace endpoint
	float HHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FVector TraceEnd = InLocation;
	TraceEnd.Z -= HHeight;

	// Setup parameters
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = false;

	// Trace world from location
	FHitResult Hit;
	OutLocation = InLocation;
	bool bGrounded = GetWorld()->LineTraceSingleByChannel(Hit, InLocation, TraceEnd, ECC_Visibility, QueryParams);
	if (bGrounded) {
		OutLocation = Hit.ImpactPoint;
		OutLocation.Z += HHeight;

		// Debug traces
		if (bShowTraces) {
			DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 10.0f, FColor::Red);
			DrawDebugLine(GetWorld(), InLocation, Hit.ImpactPoint, FColor::Red);
		}
	}

	return bGrounded;
}


