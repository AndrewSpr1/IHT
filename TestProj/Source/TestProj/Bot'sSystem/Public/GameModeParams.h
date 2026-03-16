#pragma once
#include "BotParam.h"
#include "GlobalCamera.h"
#include "MatchGameMode.h"
#include "PuckParam.h"
#include "Rules.h"
#include "Engine/TriggerBox.h"


#include "GameModeParams.generated.h"

USTRUCT(BlueprintType)
struct FGameModeParams
{
	GENERATED_BODY()
	
	UPROPERTY(Category=Parameters, EditAnywhere, BlueprintReadWrite)
	TArray<FBotParam> BotsParamArray;

	UPROPERTY(Category=Parameters, EditAnywhere, BlueprintReadWrite)
	FPuckParam PuckParam;

	UPROPERTY(Category=Parameters, EditAnywhere, BlueprintReadWrite)
	EIHMatchRulesMode MatchRules = EIHMatchRulesMode::FaceOff;
	
	UPROPERTY(Category=Parameters, EditAnywhere, BlueprintReadWrite)
	EIHMatchGameMode MatchGameMode = EIHMatchGameMode::MatchGameMode_3x3;
	
	UPROPERTY(Category=Parameters, EditAnywhere, BlueprintReadWrite)
	ETeam GoalToTeam = ETeam::Team1;

	UPROPERTY()
	AGlobalCamera* GlobalCamera = nullptr;

	UPROPERTY()
	ATriggerBox* MatchAreaBox = nullptr;
};