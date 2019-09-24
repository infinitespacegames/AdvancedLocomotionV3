// Copyright is Bullshit!  Do as you will with these files.

#include "AnimNotify_Footstep.h"

#include <Animation/AnimInstance.h>
#include <Components/AudioComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include <Kismet/GameplayStatics.h>
#include <UObject/ConstructorHelpers.h>



UAnimNotify_Footstep::UAnimNotify_Footstep() {

	// TODO: Make Subclass asset variable in ABP
	static ConstructorHelpers::FObjectFinder<USoundBase> SoundCueObject(TEXT("/Game/Audio/Footsteps/Footstep_Cue"));
	FootstepCue = SoundCueObject.Object;
}


void UAnimNotify_Footstep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) {

	auto AInst = MeshComp->GetAnimInstance();
	if (!AInst) { return; }

	// Not visible don't play
	if (!MeshComp->IsVisible()) { return; }

	auto Montage = AInst->GetCurrentActiveMontage();
	float PlayRate = AInst->Montage_GetPlayRate(Montage);

	// Play rate must match PlayReversed setting
	if (PlayReversed) {
		if (PlayRate >= 0.0f) { return; }
	}
	else {
		if (PlayRate < 0.0f) { return; }
	}

	// Spawn audio component
	auto AudioComp = UGameplayStatics::SpawnSoundAttached(
		FootstepCue,
		MeshComp,
		AttachPointName,
		FVector(0.0f),
		EAttachLocation::KeepRelativeOffset,
		false,
		VolumeMultiplier,
		PitchMultiplier
	);
	if (!AudioComp) { return; }

	// Play footstep cue
	AudioComp->SetIntParameter("FootstepType", (int32)FootstepType);
}
 