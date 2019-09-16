// Copyright is Bullshit!  Do as you will with these files.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <ConstructorHelpers.h>
#include "PlayerInfoWidget.generated.h"

class UVerticalBox;


/**
 *  Implements widget for displaying character state
 */
UCLASS()
class TPBASE_API UPlayerInfoWidget : public UUserWidget {

	GENERATED_BODY()
	
public:

	/** Base constructor */
	UPlayerInfoWidget(const FObjectInitializer& ObjectInitializer);

	/** Sets the text widget to given data, clears previous */
	void SetPlayerInfo(const TArray<FString>& InfoStrings);

	/** Appends given data to text widget */
	void AddPlayerInfo(const TArray<FString>& InfoStrings);

	/** Set the font color of any new text */
	void SetColor(FColor Color);

	/** Set the font size of any new text */
	void SetFontSize(int32 FontSize);

protected:

	/** Initialize user widget */
	virtual bool Initialize() override;

private:

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* PlayerInfo;
	
	int32 FontSize = 14;
	
	FColor Color = FColor::White;

};
