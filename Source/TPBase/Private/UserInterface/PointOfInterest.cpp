// Copyright is Bullshit!  Do as you will with these files.

#include "PointOfInterest.h"
#include "TPBaseGameState.h"

#include <Engine/Texture2D.h>
#include <Engine/Engine.h>


// Sets default values for this component's properties
UPointOfInterest::UPointOfInterest() {
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UPointOfInterest::BeginPlay() {
	Super::BeginPlay();

	if (GetWorld()) {
		auto GameState = Cast<ATPBaseGameState>(GetWorld()->GetGameState());
		if (GameState) {
			GameState->RegisterPointOfInterest(this);
		}
	}
}

