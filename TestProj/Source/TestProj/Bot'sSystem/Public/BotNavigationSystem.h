#pragma once
#include "BotParam.h"
#include "GoalParams.h"
#include "PuckParam.h"
#include "Zone.h"
#include "Components/BoxComponent.h"


namespace BotNavigationSystem
{
	void Update(float DeltaTime,
		APlayerController* PC,
		UBoxComponent* Box,
		UWorld* World,
		ETeam Team,
		TArray<FBotParam>& BotsParams,
		FPuckParam& PuckParams,
		TArray<FZone>& Zones,
		TArray<FGoalParams> GoalsParams);
	
	void Init(APlayerController* PC, UBoxComponent* Box, UWorld* World, TArray<FBotParam>& BotsParams, FPuckParam& PuckParams, TArray<FZone>& Zones);
	
	// Mouse Detection
	FVector FindMouseClickPosition(APlayerController* PC);
	void CheckClickDistance(FVector ClickPosition, TArray<FBotParam>& BotsParams, FPuckParam& PuckParam);
	void StopControl(TArray<FBotParam>& BotsParams, FPuckParam& PuckParams);
	
	//UpdateObjects
	void UpdatePuckPosition(float DeltaTime, UBoxComponent* Box, APlayerController* PC, FPuckParam& PuckParams);
	void UpdateBotsPosition(UWorld* World, float DeltaTime, UBoxComponent* Box, TArray<FZone> Zones, TArray<FBotParam>& BotsParams, FPuckParam& PuckParams, TArray<FGoalParams> GoalsParams, ETeam Team);
	
	//Zones
	void SetZones(TArray<FZone>& Zones, UBoxComponent* Box);
	FZone GetZoneWithRole(TArray<FZone> Zones, ECharacterPosition Role, ETeam Team);
	
	//Following The Puck
	FBotParam* FollowPuck(float DeltaTime, TArray<FBotParam>& BotsParams, FPuckParam& PuckParams, ETeam Team, TArray<FZone> Zones, UBoxComponent* Box);
	FBotParam* FindNearestBotToPuck(TArray<FBotParam>& BotsParams, FPuckParam PuckParams, ETeam Team);
	
	//Positions Params
	FVector LookingOnPuck(FBotParam& BotParams, FPuckParam PuckParams, TArray<FZone> Zones, UBoxComponent* Box);
	FVector CheckNearBots(TArray<FBotParam>& BotsParams, FBotParam& BotParams, FVector TargetPosition, ETeam Team, TArray<FZone> Zones);
	FVector WingOffset(FVector TargetPosition, FBotParam& BotParams, TArray<FZone> Zones);
	FVector GetPointToGoals(FBotParam TargetBot, FVector TargetPosition, FVector PositionNearGoals, FPuckParam PuckParams, TArray<FZone> Zones);
	
	//Get Forward Offset to Goals 
	FVector OffsetToGoals(FGoalParams CurrentGoalParams, UBoxComponent* Box);
	
	//Get Goals To Team
	FGoalParams GetTeamGoalParams(ETeam Team, TArray<FGoalParams> GoalsParams);
	
	//Check Is CurrentBot NearestToGoals
	bool IsNearestToGoal(TArray<FBotParam> BotsParams, FBotParam TargetBot, FGoalParams GoalsParams);
}
