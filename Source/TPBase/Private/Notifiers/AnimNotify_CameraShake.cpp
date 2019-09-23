// Copyright is Bullshit!  Do as you will with these files.

#include "AnimNotify_CameraShake.h"
#include "TPBaseCharacter.h"

#include <Camera/CameraShake.h>


void UAnimNotify_CameraShake::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) {
	ATPBaseCharacter* MyOwner = Cast<ATPBaseCharacter>(MeshComp->GetOwner());
	if (!MyOwner) { return; }

	MyOwner->CameraShake_Notify(ShakeClass, ShakeScale);
}
 