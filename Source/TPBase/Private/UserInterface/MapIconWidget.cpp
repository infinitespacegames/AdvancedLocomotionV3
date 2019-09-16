// Copyright is Bullshit!  Do as you will with these files.

#include "MapIconWidget.h"
#include "MapWidget.h"

#include <Brushes/SlateDynamicImageBrush.h>
#include <Components/Image.h>
#include <Engine/Texture2D.h>
#include <Components/Throbber.h>
#include <GameFramework/Character.h>
#include <Kismet/GameplayStatics.h>


// Called every frame
void UMapIconWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (IsValid(MyActor)) {

		// Position
		FVector2D MapLocation;
		bool OnMap = MyMap->GetMapLocation(MyActor->GetActorLocation(), MapLocation);
		CurrentIcon->SetRenderTranslation(MapLocation);

		// Orientation
		FRotator Rotation = MyActor->GetActorRotation();
		CurrentIcon->SetRenderAngle(Rotation.Yaw);

		// Determine visibility
		if (!OnMap && !bAlwaysShow) {
			CurrentIcon->SetVisibility(ESlateVisibility::Hidden);
		}
		else {
			CurrentIcon->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else {
		RemoveFromParent();
	}
}


// Sets up icon image and tint
void UMapIconWidget::Setup(const FColor& IconTint, UTexture2D* IconImage, UMapWidget* InMap, AActor* InActor, bool StaticActor) {
	
	// Parent objects
	MyMap = InMap;
	MyActor = InActor;
	bAlwaysShow = StaticActor;

	// Use specified parameters
	if (IconImage) {
		if (!IconImage->IsRooted()) {
			IconImage->AddToRoot();
		}
		FLinearColor Color = IconTint;
		FSlateDynamicImageBrush SlateBrush(IconImage, FVector2D(16.0f, 16.0f), NAME_None, Color);
		CustomIcon->SetBrush(SlateBrush);
		CustomIcon->SetVisibility(ESlateVisibility::Visible);
		DefaultIcon->SetVisibility(ESlateVisibility::Hidden);
		CurrentIcon = CustomIcon;
	}

	// No image, use default throbber
	else {
		CustomIcon->SetVisibility(ESlateVisibility::Hidden);
		DefaultIcon->SetVisibility(ESlateVisibility::Visible);
		CurrentIcon = DefaultIcon;
	}
}

// Initialize icon widget
bool UMapIconWidget::Initialize() {
	if (!Super::Initialize()) { return false; }

	// Verify icon widgets
	if (!ensure(DefaultIcon)) { return false; }
	if (!ensure(CustomIcon)) { return false; }
	
	return true;
}
