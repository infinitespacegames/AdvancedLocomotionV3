// Copyright is Bullshit!  Do as you will with these files.

#include "AnimNotify_Pivot.h"
#include "TPBaseAnimInstance.h"


void UAnimNotify_Pivot::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) {
	UTPBaseAnimInstance* AInst = Cast<UTPBaseAnimInstance>(MeshComp->GetAnimInstance());
	if (!AInst) { return; }

	AInst->Pivot_Notify(PivotParams);
}
 