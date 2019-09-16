// Copyright is Bullshit!  Do as you will with these files.

#pragma once

#include "TPBase.h"
#include <GameFramework/PlayerController.h>
#include "TPBasePlayerController.generated.h"



/**
 *  Implements the base player controller
 */
UCLASS()
class TPBASE_API ATPBasePlayerController : public APlayerController {

	GENERATED_BODY()

public:

	virtual void BeginPlay() override;
	
};

