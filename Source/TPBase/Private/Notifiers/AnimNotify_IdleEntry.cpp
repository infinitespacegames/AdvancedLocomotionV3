// Copyright is Bullshit!  Do as you will with these files.

#include "AnimNotify_IdleEntry.h"
#include "TPBaseAnimInstance.h"


void UAnimNotify_IdleEntry::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) {
	UTPBaseAnimInstance* AInst = Cast<UTPBaseAnimInstance>(MeshComp->GetAnimInstance());
	if (!AInst) { return; }

	//AInst->IdleEntry_Notify(IdleEntryState);
}
 