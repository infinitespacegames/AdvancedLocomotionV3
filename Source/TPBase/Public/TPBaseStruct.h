// Copyright is Bullshit!  Do as you will with these files.

#pragma once

#include "TPBaseEnum.h"
#include "TPBaseStruct.generated.h"



/**
 *  Structure for holding relevant camera settings
 */
USTRUCT(BlueprintType)
struct TPBASE_API FCameraSettings {

	GENERATED_BODY() 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings")
	float FieldOfView = 95.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings")
	float TargetArmLength = 325.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings")
	float LagSpeed = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings")
	FVector SocketOffset = FVector(0.0f, 0.0f, 45.0f);

	/* Initializer */
	void Setup(float ArmLength, float Lag, FVector Offset, float FOV) {
		TargetArmLength = ArmLength;
		LagSpeed = Lag;
		SocketOffset = Offset;
		FieldOfView = FOV;
	}
};

 
/**
 *  Structure for holding multiple settings based on gait
 */
USTRUCT(BlueprintType)
struct TPBASE_API FCameraGaitSettings {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings")
	FCameraSettings Walk;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings")
	FCameraSettings Run;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings")
	FCameraSettings Sprint;
};


/**
 *  Structure for holding multiple settings based on stance
 */
USTRUCT(BlueprintType)
struct TPBASE_API FCameraStanceSettings {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings")
	FCameraGaitSettings Standing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings")
	FCameraSettings Crouching;
};


/**
 * Structure for holding settings for all animation states
 */
USTRUCT(BlueprintType)
struct TPBASE_API FCameraTargetSettings {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings")
	FCameraStanceSettings VelocityMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings")
	FCameraStanceSettings LookingMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings")
	FCameraSettings Aiming;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings")
	FCameraSettings Ragdoll;

	// Camera defaults
	FCameraTargetSettings() {
		VelocityMode.Standing.Walk.Setup(300.0f, 10.0f, FVector(0.0f, 0.0f, 45.0f), 90.0f);
		VelocityMode.Standing.Run.Setup(325.0f, 8.0f, FVector(0.0f, 0.0f, 45.0f), 95.0f);
		VelocityMode.Standing.Sprint.Setup(400.0f, 6.0f, FVector(0.0f, 0.0f, 30.0f), 110.0f);
		VelocityMode.Crouching.Setup(275.0f, 10.0f, FVector(0.0f, 20.0f, 45.0f), 90.0f);
		LookingMode.Standing.Walk.Setup(275.0f, 15.0f, FVector(0.0f, 70.0f, 60.0f), 90.0f);
		LookingMode.Standing.Run.Setup(300.0f, 15.0f, FVector(0.0f, 60.0f, 60.0f), 95.0f);
		LookingMode.Standing.Sprint.Setup(325.0f, 15.0f, FVector(0.0f, 50.0f, 50.0f), 110.0f);
		LookingMode.Crouching.Setup(250.0f, 15.0f, FVector(0.0f, 60.0f, 45.0f), 90.0f);
		Aiming.Setup(200.0f, 20.0f, FVector(0.0f, 70.0f, 45.0f), 45.0f);
		Ragdoll.Setup(350.0f, 50.0f, FVector(0.0f, 0.0f, -20.0f), 90.0f);
	}
};


/**
 *  Structure for holding relevant pivot information
 */
USTRUCT(BlueprintType)
struct TPBASE_API FPivotParameters {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PivotParameters")
	float PivotDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PivotParameters")
	EMovementDirection CompletedDirection = EMovementDirection::eForward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PivotParameters")
	float CompletedTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PivotParameters")
	EMovementDirection InterruptedDirection = EMovementDirection::eBackward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PivotParameters")
	float InterruptedTime;

};


/**
 *  Structure for holding turning montages
 */
USTRUCT(BlueprintType)
struct TPBASE_API FTurnAnimations {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LeftTurnAnimation")
	UAnimMontage* LeftTurn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RightTurnAnimation")
	UAnimMontage* RightTurn;
};

