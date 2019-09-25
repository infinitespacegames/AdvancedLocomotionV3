// Copyright is Bullshit!  Do as you will with these files.

#pragma once

#include "TPBase.h"
#include "TPBaseEnum.h"
#include "UObject/Interface.h"
#include "TPBaseInterfaceABPIK.generated.h"



/**
 * This class does not need to be modified.
 */
UINTERFACE(MinimalAPI)
class UTPBaseInterfaceABPIK : public UInterface {

	GENERATED_BODY()
};


/**
 *  Defines the interface between the character and animation blueprint.
 *  These functions must be overridden by animation instance class and should 
 *  be called each frame by the implementing class to provide the needed 
 *  information for the animation blueprint.  All over-ridable in blueprint.
 */
class TPBASE_API ITPBaseInterfaceABPIK {

	GENERATED_BODY()

public: 

	/** Called to set display debug traces */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|ABPIK_Interface")
	void OnSetShowTraces(bool bShow);

	/** Called to update animation ragdoll state */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|ABPIK_Interface")
	void OnSetIsRagdoll(bool bRagdoll);

	/** Called to update animation stance */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|ABPIK_Interface")
	void OnSetStance(EStance NewStance);

	/** Called to update animation locomotion mode */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TPBase|ABPIK_Interface")
	void OnSetLocomotionMode(ELocomotionMode NewMode);

};
