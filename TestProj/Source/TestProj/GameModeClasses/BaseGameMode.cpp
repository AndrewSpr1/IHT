#include "BaseGameMode.h"

#include "BaseGameState.h"
#include "BaseHUD.h"
#include "BasePawn.h"
#include "BasePlayerController.h"
#include "BasePlayerState.h"
#include "BaseSpectatorPawn.h"
#include "TestProj/Bot'sSystem/Public/BotNavigationSystem.h"
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

	BotsSystem::Update(GetWorld(),GameModeParams.BotsParamArray, GameModeParams.PuckParam);
	//BotNavigationSystem::Update(DeltaTime, PC, Box, GetWorld(), ETeam::Team1, GameModeParams.BotsParamArray, GameModeParams.PuckParam, GameModeParams.Zones, GameModeParams.GoalParams);
}

void ABaseGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	PC = GetWorld()->GetFirstPlayerController();
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
		GameModeParams.GoalToTeam,
		GameModeParams.GoalParams
		);
	Box = Cast<UBoxComponent>(GameModeParams.MatchAreaBox->GetCollisionComponent());
	BotNavigationSystem::Init(PC, Box, GetWorld(), GameModeParams.BotsParamArray, GameModeParams.PuckParam, GameModeParams.Zones);
}

