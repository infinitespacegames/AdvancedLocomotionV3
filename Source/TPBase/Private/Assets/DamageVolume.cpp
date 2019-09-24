// Copyright is Bullshit!  Do as you will with these files.

#include "DamageVolume.h"
#include "PointOfInterest.h"


// Base constructor
ADamageVolume::ADamageVolume() {

	// Point of Interest Component
	PointOfInterest = CreateDefaultSubobject<UPointOfInterest>(TEXT("PointOfInterest Component"));
}

