// Copyright is Bullshit!  Do as you will with these files.

#pragma once

#include "TPBase.h"
#include <GameFramework/GameStateBase.h>
#include "TPBaseGameState.generated.h"

class UPointOfInterest;
class UMapWidget;


/**
 *  Implements base GameState for storing global information across server / clients
 */
UCLASS()
class TPBASE_API ATPBaseGameState : public AGameStateBase {

	GENERATED_BODY()

public:

	/** Any registered POI will show up on any registered maps */
	UFUNCTION(BlueprintCallable)
	void RegisterPointOfInterest(UPointOfInterest* POI);

	/** Any registered map will display all registered POI's */
	UFUNCTION(BlueprintCallable)
	void RegisterMap(UMapWidget* Map);

private:

	UPROPERTY()
	TArray<UPointOfInterest*> PointsOfInterest;

	UPROPERTY()
	TArray<UMapWidget*> Maps;

};

