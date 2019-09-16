// Copyright is Bullshit!  Do as you will with these files.

#pragma once

#include "TPBase.h"
#include "MapWidget.h"
#include "MiniMapWidget.generated.h"

class UWidgetSwitcher;


/**
 * Implements widget for mini map functionality
 */
UCLASS()
class TPBASE_API UMiniMapWidget : public UMapWidget {

	GENERATED_BODY()

public:

	/** Called every frame */
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:

	/** Initialize user widget */
	virtual bool Initialize() override;

	/** Blueprint round / square switcher */
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* MapSwitcher = nullptr;

	/** Blueprint round map image */
	UPROPERTY(meta = (BindWidget))
	UImage* RoundMap = nullptr;

	/** Dynamic material for round map texture */
	UPROPERTY()
	UMaterialInstanceDynamic* RoundMapMaterial = nullptr;
};

