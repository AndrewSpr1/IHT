#include "HUDWidget.h"

#include "Brushes/SlateColorBrush.h"
#include "Widgets/Layout/SConstraintCanvas.h"

void SHUDWidget::Construct(const FArguments& InArgs)
{
	GameMode = InArgs._GameMode;
	SetOptions();
	ChildSlot
	[
		SNew(SConstraintCanvas)

		// Top Widget
		+ SConstraintCanvas::Slot()
		.Offset(FMargin(300.f, 500.f, 500.f, 500.f))
		.Anchors(FAnchors(0.f, 0.f))
		.AutoSize(true)
		[
			SNew(SBorder)
			.BorderImage(new FSlateColorBrush(FLinearColor::White))
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SHorizontalBox)

				+SHorizontalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.Padding(FMargin(4.f, 2.f, 4.f, 2.f))
				.AutoWidth()
				[
					SNew(SVerticalBox)

					+SVerticalBox::Slot()
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Fill)
					.Padding(5.f)
					.AutoHeight()
					[
						SNew(STextBlock)
						.Text(FText::FromString(TEXT("GameMode")))  
						.ColorAndOpacity(FSlateColor(FLinearColor::Black))
					]

					+SVerticalBox::Slot()
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Center)
					[
						SNew(SBorder)
						.BorderImage(new FSlateColorBrush(FLinearColor::Black))
					]
					

					+SVerticalBox::Slot()
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Fill)
					.Padding(5.f)
					.AutoHeight()
					[
						SNew(STextBlock)
						.Text(FText::FromString(TEXT("3 vs 3")))  
						.ColorAndOpacity(FSlateColor(FLinearColor::Black))
					]
				]

				+SHorizontalBox::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Fill)
				[
					SNew(SBorder)
					.BorderImage(new FSlateColorBrush(FLinearColor::Black))
				]

				+SHorizontalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.Padding(FMargin(4.f, 2.f, 4.f, 2.f))
				.AutoWidth()
				[
					SNew(SVerticalBox)

					+SVerticalBox::Slot()
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Fill)
					.Padding(5.f)
					.AutoHeight()
					[
						SNew(STextBlock)
						.Text(FText::FromString(TEXT("Rules")))  
						.ColorAndOpacity(FSlateColor(FLinearColor::Black))
					]

					+SVerticalBox::Slot()
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Center)
					[
						SNew(SBorder)
						.BorderImage(new FSlateColorBrush(FLinearColor::Black))
					]
					

					+SVerticalBox::Slot()
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Fill)
					.Padding(5.f)
					.AutoHeight()
					[
						SAssignNew(ComboBox, SComboBox<TSharedPtr<EIHMatchRulesMode>>)
						.OptionsSource(&Options)
						.OnGenerateWidget_Raw(this, &SHUDWidget::GenerateWidget)
						.OnSelectionChanged_Raw(this, &SHUDWidget::OnSelectionChange)
						[
							SNew(STextBlock)
							.Text(this, &SHUDWidget::GetSelectedText)
						]
					]
				]

				+SHorizontalBox::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Fill)
				[
					SNew(SBorder)
					.BorderImage(new FSlateColorBrush(FLinearColor::Black))
				]

				+SHorizontalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.Padding(FMargin(4.f, 2.f, 4.f, 2.f))
				.AutoWidth()
				[
					SNew(SVerticalBox)

					+SVerticalBox::Slot()
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Fill)
					.Padding(5.f)
					.AutoHeight()
					[
						SNew(STextBlock)
						.Text(FText::FromString(TEXT("Team1")))  
						.ColorAndOpacity(FSlateColor(FLinearColor::Black))
					]

					+SVerticalBox::Slot()
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Center)
					[
						SNew(SBorder)
						.BorderImage(new FSlateColorBrush(FLinearColor::Black))
					]
					
					+SVerticalBox::Slot()
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Fill)
					.Padding(5.f)
					.AutoHeight()
					[
						SNew(STextBlock)
						.Text(FText(EnumPtr->GetDisplayValueAsText(GameMode->GameModeParams.Team1Tactic)))  
						.ColorAndOpacity(FSlateColor(FLinearColor::Black))
					]
				]

				+SHorizontalBox::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Fill)
				[
					SNew(SBorder)
					.BorderImage(new FSlateColorBrush(FLinearColor::Black))
				]

				+SHorizontalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.Padding(FMargin(4.f, 2.f, 4.f, 2.f))
				.AutoWidth()
				[
					SNew(SVerticalBox)

					+SVerticalBox::Slot()
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Fill)
					.Padding(5.f)
					.AutoHeight()
					[
						SNew(STextBlock)
						.Text(FText::FromString(TEXT("Team2")))  
						.ColorAndOpacity(FSlateColor(FLinearColor::Black))
					]

					+SVerticalBox::Slot()
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Center)
					[
						SNew(SBorder)
						.BorderImage(new FSlateColorBrush(FLinearColor::Black))
					]
					
					+SVerticalBox::Slot()
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Fill)
					.Padding(5.f)
					.AutoHeight()
					[
						SNew(STextBlock)
						.Text(FText(EnumPtr->GetDisplayValueAsText(GameMode->GameModeParams.Team2Tactic)))  
						.ColorAndOpacity(FSlateColor(FLinearColor::Black))
					]
				]
			]
		]

		// Bottom Widget
		+SConstraintCanvas::Slot()
		.Offset(FMargin(300.f, 600.f, 500.f, 500.f))
		.Anchors(FAnchors(0.f, 0.f))
		.AutoSize(true)
		[
			SNew(SBorder)
			.BorderImage(new FSlateColorBrush(FLinearColor::White))
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SHorizontalBox)

				+SHorizontalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.Padding(FMargin(4.f, 2.f, 4.f, 2.f))
				.AutoWidth()
				[
					SNew(SVerticalBox)

					+SVerticalBox::Slot()
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Fill)
					.Padding(5.f)
					.AutoHeight()
					[
						SNew(STextBlock)
						.Text(FText::FromString(TEXT("Radius")))  
						.ColorAndOpacity(FSlateColor(FLinearColor::Black))
					]

					+SVerticalBox::Slot()
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Center)
					[
						SNew(SBorder)
						.BorderImage(new FSlateColorBrush(FLinearColor::Black))
					]
					

					+SVerticalBox::Slot()
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Fill)
					.Padding(5.f)
					.AutoHeight()
					[
						SAssignNew(RadiusTextBox, SEditableTextBox)
						.Text(FText::AsNumber(GameMode->GameModeParams.Radius))
						.OnTextCommitted_Raw(this, &SHUDWidget::OnRadiusTextCommitted)
					]
				]

				+SHorizontalBox::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Fill)
				[
					SNew(SBorder)
					.BorderImage(new FSlateColorBrush(FLinearColor::Black))
				]

				+SHorizontalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.Padding(FMargin(4.f, 2.f, 4.f, 2.f))
				.AutoWidth()
				[
					SNew(SVerticalBox)

					+SVerticalBox::Slot()
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Fill)
					.Padding(5.f)
					.AutoHeight()
					[
						SNew(STextBlock)
						.Text(FText::FromString(TEXT("Angle")))  
						.ColorAndOpacity(FSlateColor(FLinearColor::Black))
					]

					+SVerticalBox::Slot()
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Center)
					[
						SNew(SBorder)
						.BorderImage(new FSlateColorBrush(FLinearColor::Black))
					]
					

					+SVerticalBox::Slot()
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Fill)
					.Padding(5.f)
					.AutoHeight()
					[
						SAssignNew(RadiusTextBox, SEditableTextBox)
						.Text(FText::AsNumber(GameMode->GameModeParams.AngleBetweenBots))
						.OnTextCommitted_Raw(this, &SHUDWidget::OnAngleTextCommitted)
					]
				]
			]
		]
	];
	
}

void SHUDWidget::SetOptions()
{
	Options.Add(MakeShared<EIHMatchRulesMode>(EIHMatchRulesMode::FaceOff));
	Options.Add(MakeShared<EIHMatchRulesMode>(EIHMatchRulesMode::Goal));
	Options.Add(MakeShared<EIHMatchRulesMode>(EIHMatchRulesMode::Game));
}

TSharedRef<SWidget> SHUDWidget::GenerateWidget(TSharedPtr<EIHMatchRulesMode> Item) const
{
	if (!Item.IsValid()) return SNew(STextBlock).Text(FText::FromString(TEXT("None")));
	
	return SNew(STextBlock).Text(FText::FromString(UEnum::GetValueAsString(*Item)));
}

void SHUDWidget::OnSelectionChange(TSharedPtr<EIHMatchRulesMode> Item, ESelectInfo::Type SelectInfo)
{
	if (Item.IsValid())
	{
		SelectedOption = Item;
		GameMode->GameModeParams.MatchRules = *SelectedOption;
	}
}

FText SHUDWidget::GetSelectedText() const
{
	return SelectedOption.IsValid() ? FText::FromString(UEnum::GetValueAsString(*SelectedOption)) : FText::FromString(UEnum::GetValueAsString(GameMode->GameModeParams.MatchRules));
}

void SHUDWidget::OnRadiusTextCommitted(const FText& NewText, ETextCommit::Type CommitMethod)
{
	RadiusCurrentText = NewText;
	GameMode->GameModeParams.Radius = FCString::Atof(*NewText.ToString());
}

void SHUDWidget::OnAngleTextCommitted(const FText& NewText, ETextCommit::Type CommitMethod)
{
	AngleCurrentText = NewText;
	GameMode->GameModeParams.AngleBetweenBots = FCString::Atof(*NewText.ToString());
}
