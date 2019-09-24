// Copyright is Bullshit!  Do as you will with these files.

#pragma once

#include "TPBase.h"

#include <Animation/AnimNotifies/AnimNotifyState.h>
#include "AnimNotify_TurnInPlace.generated.h"

class UCurveFloat;


/**
 *  Implements custom turn-in-place AnimNotifyState
 */
UCLASS()
class TPBASE_API UAnimNotify_TurnInPlace : public UAnimNotifyState {

	GENERATED_BODY()

	/** Called at beginning of notify */
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	
	/** Called each frame */
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;

	/** Called at end of notify */
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;


	UPROPERTY(EditAnywhere, Category = "Parameters")
	bool bIsRightTurn;
	
	UPROPERTY(EditAnywhere, Category = "Parameters")
	UCurveFloat* CurveAsset;

	UPROPERTY(EditAnywhere, Category = "Parameters")
	UAnimMontage* Montage;

};
