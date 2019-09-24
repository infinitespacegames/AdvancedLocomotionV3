// Copyright is Bullshit!  Do as you will with these files.

#include "AnimNotify_TurnInPlace.h"
#include "TPBaseCharacter.h"
#include "TPBaseAnimInstance.h"

#include <Curves/CurveFloat.h>


// Called at beginning of notify
void UAnimNotify_TurnInPlace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) {
	auto AInst = Cast<UTPBaseAnimInstance>(MeshComp->GetAnimInstance());
	if (!AInst) { return; }

	AInst->TurnInPlace_Notify(Montage, true, true, bIsRightTurn);
}
 

// Called each frame
void UAnimNotify_TurnInPlace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) {

	ATPBaseCharacter* MyOwner = Cast<ATPBaseCharacter>(MeshComp->GetOwner());
	if (!MyOwner) { return; }

	UAnimInstance* AInst = MeshComp->GetAnimInstance();
	if (!AInst) { return; }
	
	// Get montage information
	float Position = AInst->Montage_GetPosition(Montage);
	float PlayRate = AInst->Montage_GetPlayRate(Montage);
	
	// Determine needed yaw
	float CurrentYaw = CurveAsset->GetFloatValue(Position);
	float NextYaw = CurveAsset->GetFloatValue((PlayRate * FrameDeltaTime) + Position);
	float Yaw = NextYaw - CurrentYaw;

	// Apply yaw
	MyOwner->DelayedRotation_Notify(FRotator(0.0f, Yaw, 0.0f), 0.2f);

	// Stop montage if moving
	if (!MyOwner->GetVelocity().Equals(FVector(0.0f), 0.001)) {
		AInst->Montage_Stop(0.2f, Montage);
	}
}


// Called at end of notify
void UAnimNotify_TurnInPlace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) {
	auto AInst = Cast<UTPBaseAnimInstance>(MeshComp->GetAnimInstance());
	if (!AInst) { return; }

	AInst->TurnInPlace_Notify(nullptr, false, false, false);
}
