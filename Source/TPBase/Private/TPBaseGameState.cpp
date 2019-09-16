// Copyright is Bullshit!  Do as you will with these files.

#include "TPBaseGameState.h"
#include "PointOfInterest.h"
#include "MapWidget.h"


void ATPBaseGameState::RegisterPointOfInterest(UPointOfInterest* POI) {
	if (!POI) { return; }

	PointsOfInterest.Add(POI);
	for (UMapWidget* Map : Maps) {
		Map->AddPointOfInterest(POI);
	}
}


void ATPBaseGameState::RegisterMap(UMapWidget* Map) {
	if (!Map) { return; }

	Maps.Add(Map);
	for (UPointOfInterest* POI : PointsOfInterest) {
		Map->AddPointOfInterest(POI);
	}
}
