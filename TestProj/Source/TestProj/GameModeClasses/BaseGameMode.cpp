#include "BaseGameMode.h"

#include "BaseGameState.h"
#include "BaseHUD.h"
#include "BasePawn.h"
#include "BasePlayerController.h"
#include "BasePlayerState.h"
#include "BaseSpectatorPawn.h"
#include "TestProj/Bot'sSystem/Public/BotsSystem.h"

ABaseGameMode::ABaseGameMode()
{
	PrimaryActorTick.bCanEverTick = true;

	//Default Classes
	DefaultPawnClass = ABasePawn::StaticClass();
	HUDClass = ABaseHUD::StaticClass();
	PlayerControllerClass = ABasePlayerController::StaticClass();
	GameStateClass = ABaseGameState::StaticClass();
	PlayerStateClass = ABasePlayerState::StaticClass();
	SpectatorClass = ABaseSpectatorPawn::StaticClass();
}

void ABaseGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	BotsSystem::Update(GetWorld(), GameModeParams.BotsParamArray, GameModeParams.PuckParam);
}

void ABaseGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	check(PC)

	BotsSystem::Init(
		GetWorld(),
		PC,
		GameModeParams.MatchRules,
		GameModeParams.BotsParamArray,
		GameModeParams.PuckParam,
		GameModeParams.GlobalCamera,
		GameModeParams.MatchAreaBox,
		GameModeParams.MatchGameMode,
		GameModeParams.bIsGoalTop
		);
}

