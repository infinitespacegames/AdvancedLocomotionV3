// Copyright is Bullshit!  Do as you will with these files.

#pragma once

#include "TPBase.h"
#include <Components/ActorComponent.h>
#include "PointOfInterest.generated.h"

class UTexture2D;


/** 
  Implements a POI for use within maps
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPBASE_API UPointOfInterest : public UActorComponent {

	GENERATED_BODY()

public:
	
	/** Base constructor */
	UPointOfInterest();

protected:

	/** Called when the game starts */
	virtual void BeginPlay() override;

public:	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PointOfInterest")
	bool bAlwaysShow = true; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PointOfInterest")
	FColor IconTint = FColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PointOfInterest")
	UTexture2D* IconTexture = nullptr;
};
