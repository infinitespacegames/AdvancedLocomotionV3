// Copyright is Bullshit!  Do as you will with these files.

#pragma once

#include "TPBase.h"
#include "TPBaseEnum.h"

#include <Animation/AnimNotifies/AnimNotify.h>
#include "AnimNotify_IdleEntry.generated.h"


/**
 *  Implements custom idle entry AnimNotify
 */
UCLASS()
class TPBASE_API UAnimNotify_IdleEntry : public UAnimNotify {

	GENERATED_BODY()

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	
	UPROPERTY(EditAnywhere, Category = "Parameters")
	EIdleEntryState IdleEntryState;

};
