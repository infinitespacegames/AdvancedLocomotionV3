// Copyright is Bullshit!  Do as you will with these files.

#include "AnimNotify_TurnInPlace.h"
#include "TPBaseCharacter.h"
#include "TPBaseAnimInstance.h"

#include <Curves/CurveFloat.h>


void UAnimNotify_TurnInPlace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) {
	auto AInst = Cast<UTPBaseAnimInstance>(MeshComp->GetAnimInstance());
	if (!AInst) { return; }

	//AInst->TurnInPlaceNotify(Montage, true, true, bIsRightTurn);
}
 

void UAnimNotify_TurnInPlace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) {

	ATPBaseCharacter* MyOwner = Cast<ATPBaseCharacter>(MeshComp->GetOwner());
	if (!MyOwner) { return; }

	UAnimInstance* AInst = MeshComp->GetAnimInstance();
	if (!AInst) { return; }
	
	float Position = AInst->Montage_GetPosition(Montage);
	float PlayRate = AInst->Montage_GetPlayRate(Montage);
	
	float MPos = CurveAsset->GetFloatValue(Position);
	float Next = CurveAsset->GetFloatValue((PlayRate * FrameDeltaTime) + Position);

	float Yaw = Next - MPos;
	MyOwner->DelayedRotationNotify(FRotator(0.0f, Yaw, 0.0f), 0.0f);

	if (!MyOwner->GetVelocity().Equals(FVector(0.0f), 0.001)) {
		AInst->Montage_Stop(0.2f, Montage);
	}
}


void UAnimNotify_TurnInPlace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) {
	auto AInst = Cast<UTPBaseAnimInstance>(MeshComp->GetAnimInstance());
	if (!AInst) { return; }

	//AInst->TurnInPlaceNotify(nullptr, false, false, false);
}
