// Copyright is Bullshit!  Do as you will with these files.

#pragma once

#include "TPBase.h"
#include "TPBaseEnum.h"

#include <Animation/AnimNotifies/AnimNotify.h>
#include "AnimNotify_Footstep.generated.h"

class USoundBase;


/**
 *  Implements custom footstep AnimNotify
 */
UCLASS()
class TPBASE_API UAnimNotify_Footstep : public UAnimNotify {

	GENERATED_BODY()

public:

	UAnimNotify_Footstep();
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

protected:

	UPROPERTY(EditAnywhere, Category = "Parameters")
	USoundBase* FootstepCue;

	UPROPERTY(EditAnywhere, Category = "Parameters")
	EFootstepType FootstepType;

	UPROPERTY(EditAnywhere, Category = "Parameters")
	float VolumeMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Parameters")
	float PitchMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Parameters")
	FName AttachPointName = "root";

	UPROPERTY(EditAnywhere, Category = "Parameters")
	bool PlayReversed = false;

};
