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

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;


	UPROPERTY(EditAnywhere, Category = "Parameters")
	USoundBase* Sound;

	UPROPERTY(EditAnywhere, Category = "Parameters")
	float VolumeMultiplier;

	UPROPERTY(EditAnywhere, Category = "Parameters")
	float PitchMultiplier;

	UPROPERTY(EditAnywhere, Category = "Parameters")
	FName AttachPointName;

	UPROPERTY(EditAnywhere, Category = "Parameters")
	bool PlayReversed;

	UPROPERTY(EditAnywhere, Category = "Parameters")
	EFootstepType FootstepType;

};
