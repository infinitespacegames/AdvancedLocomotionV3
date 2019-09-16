// Copyright is Bullshit!  Do as you will with these files.

#include "TPBaseGameMode.h"
#include "TPBaseCharacter.h"

#include <GameFramework/GameStateBase.h>
#include <UObject/ConstructorHelpers.h>


// TPBase constructor
ATPBaseGameMode::ATPBaseGameMode() {

	// Set default pawn class
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Character/BP_BaseCharacter"));
	if (ensure(PlayerPawnBPClass.Class)) {
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// Set default game state
	static ConstructorHelpers::FClassFinder<AGameStateBase> GameStateBPClass(TEXT("/Game/BP_BaseGameState"));
	if (ensure(GameStateBPClass.Class)) {
		GameStateClass = GameStateBPClass.Class;
	}

	// Set default player controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/BP_BasePlayerController"));
	if (ensure(PlayerControllerBPClass.Class)) {
		PlayerControllerClass = PlayerControllerBPClass.Class;
		ReplaySpectatorPlayerControllerClass = PlayerControllerBPClass.Class;
	}

}
