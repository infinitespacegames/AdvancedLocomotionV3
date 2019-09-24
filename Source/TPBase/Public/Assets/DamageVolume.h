// Copyright is Bullshit!  Do as you will with these files.

#pragma once

#include "CoreMinimal.h"

#include <GameFramework/PainCausingVolume.h>
#include "DamageVolume.generated.h"

class UPointOfInterest;


/**
 *  Implements custom pain volume with point of interest
 */
UCLASS()
class TPBASE_API ADamageVolume : public APainCausingVolume {

	GENERATED_BODY()

public:

	/** Base constructor */
	ADamageVolume();

protected:

	/** Point of interest icon for maps */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UPointOfInterest* PointOfInterest;

};

