// Copyright is Bullshit!  Do as you will with these files.

#pragma once

#include "TPBase.h"
#include <Blueprint/UserWidget.h>
#include "MapWidget.generated.h"

class UImage;
class UOverlay;
class UPointOfInterest;


/**
 * Implements widget for general map functionality
 */
UCLASS()
class TPBASE_API UMapWidget : public UUserWidget {

	GENERATED_BODY()

public:
	
	/** Base constructor */
	UMapWidget(const FObjectInitializer& ObjectInitializer);

	/** Called every frame */
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:

	/** Initialize user widget */
	virtual bool Initialize() override;

public:

	/** Image map displays */
	UPROPERTY(EditAnywhere, Category = "Map Display")
	UTexture* MapImage = nullptr;

	/** Size of captured map image */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map Display")
	float Dimensions = 0;

	/** Screen half-size of map widget */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map Display")
	float HalfSize = 0;

	/** Map zoom level */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Display")
	float Zoom = 0.5;

	/** Convert world position to map coordinates */
	UFUNCTION(BlueprintCallable)
	bool GetMapLocation(const FVector& WorldLocation, FVector2D& MapLocation);

	/** Add a POI to the map */
	UFUNCTION(BlueprintCallable)
	void AddPointOfInterest(UPointOfInterest* POI);

protected:

	/** Use round or square map, doesn't effect base map */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Display")
	bool IsRound = false;

	/** Blueprint map overlay widget */
	UPROPERTY(meta = (BindWidget))
	UOverlay* MapOverlay = nullptr;

	/** Blueprint square map widget */
	UPROPERTY(meta = (BindWidget))
	UImage* SquareMap = nullptr;

	/** Dynamic material for square map texture */
	UPROPERTY()
	UMaterialInstanceDynamic* SquareMapMaterial = nullptr;

	/** Tint for player icon */
	UPROPERTY(EditAnywhere, Category = "Player Display")
	FColor PlayerIconTint = FColor::White;

	/** Image for player icon */
	UPROPERTY(EditAnywhere, Category = "Player Display")
	UTexture2D* PlayerIcon = nullptr;

	/** Blueprint player icon class */
	UPROPERTY(EditAnywhere, Category = "Map Display")
	TSubclassOf<UUserWidget> MapIconClass;

	/** Map parameter collection instance */
	UPROPERTY()
	UMaterialParameterCollectionInstance* MapData = nullptr;

	/** Adds player icon to map */
	UFUNCTION()
	void AddPlayerIcon();

	/** Adds point of interest icon to map */
	UFUNCTION()
	void AddMapIcon(const FColor& IconTint, UTexture2D* IconImage, AActor* Actor, bool bAlwaysShow);
};

