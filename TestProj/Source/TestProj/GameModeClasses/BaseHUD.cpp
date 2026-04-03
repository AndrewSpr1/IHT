#include "BaseHUD.h"

void ABaseHUD::BeginPlay()
{
	Super::BeginPlay();
	
	if (GEngine && GEngine->GameViewport)
	{
		ABaseGameMode* GameMode = Cast<ABaseGameMode>(GetWorld()->GetAuthGameMode());
		HUDWidget = SNew(SHUDWidget)
			.GameMode(GameMode);
		GEngine->GameViewport->AddViewportWidgetContent(HUDWidget.ToSharedRef(), 0);
		UE_LOG(LogTemp, Warning, TEXT("HUD WasCreated"));
	}
}
