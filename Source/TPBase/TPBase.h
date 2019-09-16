// Copyright is Bullshit!  Do as you will with these files.

#pragma once

#include "CoreMinimal.h"


#define PrintScreen(x, t, c) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, t, c, x, false)

/** Enumeration to string */
template<typename TEnum>
static FORCEINLINE FString Enum2Str(const FString& Name, TEnum Value) {
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);
	if (!enumPtr) { 
		return FString("Invalid"); 
	}
	FString NameString = enumPtr->GetNameStringByValue((int64)Value);
	NameString.RemoveFromStart("e");
	return NameString;
}
