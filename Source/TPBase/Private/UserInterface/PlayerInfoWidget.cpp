


#include "PlayerInfoWidget.h"

#include <Components/VerticalBox.h>
#include <Components/TextBlock.h>


// Base constructor
UPlayerInfoWidget::UPlayerInfoWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
}


// Initialize user widget 
bool UPlayerInfoWidget::Initialize() {
	if (!Super::Initialize()) { return false; }
	if (!PlayerInfo) { return false; }
	return true;
}


// Sets the text widget to given data, clears previous 
void UPlayerInfoWidget::SetPlayerInfo(const TArray<FString>& InfoStrings) {
	if (!PlayerInfo) { return; }

	PlayerInfo->ClearChildren();
	AddPlayerInfo(InfoStrings);
}


// Appends given data to text widget 
void UPlayerInfoWidget::AddPlayerInfo(const TArray<FString>& InfoStrings) {
	if (!PlayerInfo) { return; }

	UTextBlock* TextBlock = nullptr;
	for (FString String : InfoStrings) {
		TextBlock = NewObject<UTextBlock>(UTextBlock::StaticClass());
		TextBlock->SetColorAndOpacity(Color.ReinterpretAsLinear());
		TextBlock->Font.Size = FontSize;
		TextBlock->SetText(FText::FromString(String));
		PlayerInfo->AddChildToVerticalBox(TextBlock);
	}
}


// Set the font color of any new text 
void UPlayerInfoWidget::SetColor(FColor NewColor) {
	Color = NewColor;
}


// Set the font size of any new text 
void UPlayerInfoWidget::SetFontSize(int32 NewFontSize) {
	FontSize = NewFontSize;
}

