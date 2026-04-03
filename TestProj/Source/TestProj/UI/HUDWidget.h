#pragma once

#include "CoreMinimal.h"
#include "TestProj/Bot'sSystem/Public/Rules.h"
#include "TestProj/GameModeClasses/BaseGameMode.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SEditableTextBox.h"

class TESTPROJ_API SHUDWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SHUDWidget) {}
		SLATE_ARGUMENT(ABaseGameMode*, GameMode)
	SLATE_END_ARGS()
	
	TArray<TSharedPtr<EIHMatchRulesMode>> Options;
	TSharedPtr<EIHMatchRulesMode> SelectedOption;
	TSharedPtr<SComboBox<TSharedPtr<EIHMatchRulesMode>>> ComboBox;
	ABaseGameMode* GameMode = nullptr;
	
	const UEnum* EnumPtr = StaticEnum<ETeamTactic>();
	
	TSharedPtr<SEditableTextBox> RadiusTextBox;
	FText RadiusCurrentText;
	
	TSharedPtr<SEditableTextBox> AngleTextBox;
	FText AngleCurrentText;
	
	void Construct(const FArguments& InArgs);
	
	void SetOptions();
	
	TSharedRef<SWidget> GenerateWidget(TSharedPtr<EIHMatchRulesMode> Item) const;
	void OnSelectionChange(TSharedPtr<EIHMatchRulesMode> Item, ESelectInfo::Type SelectInfo);
	FText GetSelectedText() const;
	
	void OnRadiusTextCommitted(const FText& NewText, ETextCommit::Type CommitMethod);
	void OnAngleTextCommitted(const FText& NewText, ETextCommit::Type CommitMethod);
};
