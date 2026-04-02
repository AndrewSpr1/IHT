#pragma once
#include "BotParam.h"
#include "GoalParams.h"
#include "PuckParam.h"
#include "Tactics.h"
#include "Zone.h"
#include "Components/BoxComponent.h"


namespace BotNavigationSystem
{
	void Update(float DeltaTime,
		APlayerController* PC,
		UBoxComponent* Box,
		TArray<UBoxComponent*> GoalsBoxes,
		UWorld* World,
		ETeam Team,
		TArray<FBotParam>& BotsParams,
		FPuckParam& PuckParams,
		TArray<FZone>& Zones,
		TArray<FGoalParams> GoalsParams,
		const float Radius,
		const float OffsetUnderPuck,
		const float AngleBetweenBots,
		const float RightBotAngle,
		const float LeftBotAngle);
	
	void Init(APlayerController* PC, UBoxComponent* Box, UWorld* World, TArray<FBotParam>& BotsParams, FPuckParam& PuckParams, TArray<FZone>& Zones);
	
	// Mouse Detection
	FVector FindMouseClickPosition(APlayerController* PC);
	void CheckClickDistance(FVector ClickPosition, TArray<FBotParam>& BotsParams, FPuckParam& PuckParam);
	void StopControl(TArray<FBotParam>& BotsParams, FPuckParam& PuckParams);
	
	//UpdateObjects
	void UpdatePuckPosition(float DeltaTime, UBoxComponent* Box, APlayerController* PC, FPuckParam& PuckParams);
	void UpdateBotsPosition(UWorld* World, float DeltaTime, UBoxComponent* Box, TArray<FZone> Zones, TArray<FBotParam>& BotsParams, FPuckParam& PuckParams, TArray<FGoalParams> GoalsParams, ETeam Team);
	
	//---------------------------------------Didn't Use---------------------------------------
	
	//Zones
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
	
	FVector BotDefaultPosition(ECharacterPosition Role, ETeam Team, const FVector PuckPos, const FVector BoxForward, const float OffsetBots, const float OffsetUnderPuck);
	FVector BotOffsetFromSide(ETeam Team, const FVector BotPos, const FVector PuckPos, const FVector BoxCenter, const FVector BoxForward, const FVector BoxRight,
		const FVector BoxExtent, const float OffsetBots, const float OffsetUnderPuck);
	
	
	//--------------------------------------------Navigation with Radius-----------------------------------------
	
	FVector ClampToZone(FVector Pos, UBoxComponent* Box);
	
	FVector UnclampToZone(FVector Pos, UBoxComponent* Box);
	
	void BotPositionManagementNeutral(UWorld* World, FBotParam& BotParams, UBoxComponent* Box, TArray<UBoxComponent*> GoalsBoxes, const float DU, const float DV, FPuckParam PuckParams,
		const float Radius, const float AngleBetweenBots, const float RightBotAngle, const float LeftBotAngle);
	
	FVector GetPointsOnCircle(
		ECharacterPosition Role
		, const FVector Center
		, const FVector DefaultSectorDirection
		, const FVector RotatedSectorDirection
		, const float Angle
		, const float Radius
		, const float LeftAngle  = 0
		, const float RightAngle = 0
		, const float LeftRadius = 0
		, const float RightRadius = 0);
	
	float GetMainVectorAngle(const float AngleBetweenBots, const float DU, const float DV, const ETeamTactic Tactic = ETeamTactic::None);
	
	
	
	float GetAngleForDefence(const float AngleBetweenBots, float& DU, const float DV, const float MaxAngleOffset = 120);
	
	float GetOwnAngleForDefence(const float AngleBetweenBots, float& DU, const float DV, const float MaxAngleOffset = 120);
	float GetOwnAngleForAttack(const float AngleBetweenBots, float& DU, const float DV, const float MaxAngleOffset = 120);
	
	float GetMainRadiusForDefence(const float CurrentRadius, const float DU, const float DV, const float MinRadius = 200);
	float GetOwnRadiusForDefence(const float DU, const float DV, const float MaxRadius = 200, const bool bIsRight = true);
	
	FVector GetBotPositionNearPuck(const FVector ViewDirection, const FVector PuckLocation, const FVector CurrentBotLocation, const float Offset);
	
	// Horizontal parameter (Right: 1, Left: -1)
	float GetDU(const FVector BoxCenter, const FVector BoxExtent, const FVector ViewDirection, const FVector PuckPos);
	
	// Vertical parameter (Up: 1, Down:-1)
	float GetDV(const FVector BoxCenter, const FVector BoxExtent, const FVector ViewDirection, const FVector PuckPos);
	
	float ClampFromMinusOneToOne(const float Size, const float Alpha);
}
