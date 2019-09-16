// Copyright is Bullshit!  Do as you will with these files.


#include "TPBasePlayerController.h"



void ATPBasePlayerController::BeginPlay() {
	Super::BeginPlay();

	// Set this character to call Tick() every frame.
	PrimaryActorTick.bCanEverTick = false;

}