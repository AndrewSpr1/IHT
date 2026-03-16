#include "BasePlayerController.h"


#include "TestProj/Bot'sSystem/Public/BotNavigationSystem.h"
#include "TestProj/Bot'sSystem/Public/BotsSystem.h"

void ABasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	InputComponent->BindAction("MouseClick", IE_Pressed, this, &ABasePlayerController::MousePressed);
	InputComponent->BindAction("MouseClick", IE_Released, this, &ABasePlayerController::MouseRelease);
}

void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();
	BaseGameMode = Cast<ABaseGameMode>(GetWorld()->GetAuthGameMode());
	SetCursor();
}

void ABasePlayerController::MousePressed()
{
	FVector ClickPosition = BotNavigationSystem::FindMouseClickPosition(GetWorld(), this);
	BotNavigationSystem::CheckClickDistance(ClickPosition, BaseGameMode->GameModeParams.BotsParamArray, BaseGameMode->GameModeParams.PuckParam);
}

void ABasePlayerController::MouseRelease()
{
	BotNavigationSystem::StopControl(BaseGameMode->GameModeParams.BotsParamArray, BaseGameMode->GameModeParams.PuckParam);
}

void ABasePlayerController::SetCursor()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is not find"));
		return;
	}

	PC->SetShowMouseCursor(true);
}
