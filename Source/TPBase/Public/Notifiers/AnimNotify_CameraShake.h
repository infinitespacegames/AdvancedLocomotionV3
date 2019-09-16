// Copyright is Bullshit!  Do as you will with these files.

#pragma once

#include "TPBase.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_CameraShake.generated.h"

class UCameraShake;


/**
 *  Implements custom camera shake AnimNotify
 */
UCLASS()
class TPBASE_API UAnimNotify_CameraShake : public UAnimNotify {

	GENERATED_BODY()

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;


	UPROPERTY(EditAnywhere, Category = "Parameters")
	float ShakeScale = 1.0f;
	
	UPROPERTY(EditAnywhere, Category = "Parameters")
	TSubclassOf<UCameraShake> ShakeClass;

};
