// Copyright is Bullshit!  Do as you will with these files.

#include "MiniMapWidget.h"
#include "MapIconWidget.h"

#include <Components/Image.h>
#include <Components/WidgetSwitcher.h>
#include <Components/Overlay.h>
#include <ConstructorHelpers.h>
#include <GameFramework/Character.h>
#include <Kismet/GameplayStatics.h>
#include <Materials/MaterialInstanceDynamic.h>
#include <Materials/MaterialParameterCollection.h>
#include <Materials/MaterialParameterCollectionInstance.h>
#include <OverlaySlot.h>


static const auto MiniMapDataCollectionName = TEXT("MaterialParameterCollection'/Game/UI/ParameterCollections/MiniMap_Data.MiniMap_Data'");


// Called every frame
void UMiniMapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	UUserWidget::NativeTick(MyGeometry, InDeltaTime);

	// Get player character
	ACharacter* PC = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (PC) {
		// Set dynamic material texture zoom
		MapData->SetScalarParameterValue("Zoom", Zoom);

		// Set dynamic material texture position
		auto Location = PC->GetActorLocation();
		MapData->SetScalarParameterValue("Xord", Location.X);
		MapData->SetScalarParameterValue("Yord", Location.Y);
	}

	// Set appropriate map shape
	if (MapSwitcher) {
		MapSwitcher->SetActiveWidgetIndex((int32)IsRound);
	}
}


// Initialize widget
bool UMiniMapWidget::Initialize() {
	if (!UUserWidget::Initialize()) { return false; }

	// Get material parameter collection data
	UMaterialParameterCollection* MapParams = Cast<UMaterialParameterCollection>(StaticLoadObject(UMaterialParameterCollection::StaticClass(), NULL, MiniMapDataCollectionName));
	if (!ensure(MapParams)) { return false; }

	if (GetWorld()) {
		MapData = GetWorld()->GetParameterCollectionInstance(MapParams);
		if (!ensure(MapData)) { return false; }

		// Set material collection parameters
		MapData->SetScalarParameterValue("Dimensions", Dimensions);
		MapData->SetScalarParameterValue("MapHalfSize", HalfSize);
		MapData->SetScalarParameterValue("Zoom", Zoom);

		if (!ensure(SquareMap)) { return false; }
		SquareMapMaterial = SquareMap->GetDynamicMaterial();
		SquareMapMaterial->SetTextureParameterValue("MapImage", MapImage);

		// Verify map widgets and get dynamic materials
		if (!ensure(RoundMap)) { return false; }
		RoundMapMaterial = RoundMap->GetDynamicMaterial();
		RoundMapMaterial->SetTextureParameterValue("MapImage", MapImage);

		// Set appropriate map shape
		if (MapSwitcher) {
			MapSwitcher->SetActiveWidgetIndex((int32)IsRound);
		}

		// Player icon
		AddPlayerIcon();
		return true;
	}
	return false;
}
