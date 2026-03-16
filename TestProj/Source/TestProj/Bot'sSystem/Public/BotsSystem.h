#pragma once
#include "BotParam.h"
#include "PuckParam.h"
#include "Rules.h"
#include "TestProj/GameModeClasses/BaseGameMode.h"


namespace BotsSystem
{
	//Call in GameMode
	void Update(UWorld* World, TArray<FBotParam> BotsParams, FPuckParam PuckParams);
	void Init(
		UWorld* World, 
		APlayerController* PC, 
		EIHMatchRulesMode Rules, 
		TArray<FBotParam>& BotsParams, 
		FPuckParam& PuckParams, 
		AGlobalCamera*& GlobalCamera, 
		ATriggerBox*& MatchAreaBox, 
		EIHMatchGameMode MatchGameMode, 
		bool bIsGoalTop = false
		);

	//Spawn and find functions
	void FindBoxActor(UWorld* World, ATriggerBox*& MatchAreaBox);
	void CameraSpawn(UWorld* World, AGlobalCamera*& GlobalCamera);
	void SetGlobalCameraView(APlayerController* PC, AGlobalCamera* GlobalCamera);

	//Log bot parameters
	void LogBotsParameters(FBotParam BotParam);

	//Set default parameters
	void SetBotsParams(ATriggerBox* MatchAreaBox, EIHMatchGameMode MatchGameMode, TArray<FBotParam>& BotsParamsArray, float CenterRadius = 200);
	void SetPuckParams(ATriggerBox* MatchAreaBox, FPuckParam& Puck);

	//Set parameters after change rules
	void SetRules(EIHMatchRulesMode Rules, ATriggerBox* MatchAreaBox, TArray<FBotParam>& BotsParamsArray, EIHMatchGameMode MatchGameMode, bool bIsGoalTop = false, float CenterRadius = 200);
	void SetBotsFaceOffParams(ATriggerBox* MatchAreaBox, EIHMatchGameMode MatchGameMode, TArray<FBotParam>& BotsParamsArray, float CenterRadius);
	void SetBotsGoalParams(ATriggerBox* MatchAreaBox, EIHMatchGameMode MatchGameMode, TArray<FBotParam>& BotsParamsArray, bool bIsGoalTop, float CenterRadius);
	
	//Debud
	void UpdateBotsDebug(UWorld* World, TArray<FBotParam> BotsParams);
	void UpdatePuckDebug(UWorld* World, FPuckParam Puck);
	void DrawDebug(UWorld* World, FVector Position, FColor Color, float Radius = 50.f, int32 Segments = 32, float Thickness = 2.f, float Duration = 0.1f);
	void DrawDirectionDebug(UWorld* World, FBotParam BotsParams, FColor Color, float Radius = 50.f, float Thickness = 2.f, float Duration = 0.1f);

	//MouseClick
	void FindMouseClickPosition(UWorld* World, APlayerController* PC, ABaseGameMode* GameMode);
	void CheckClickDistance(FVector ClickPosition, TArray<FBotParam>& BotsParams, FPuckParam& PuckParam);

	//Find positions on Goal
	FVector GetRandQuarterCenter(ATriggerBox* MatchAreaBox, bool bTop = true);
	FVector FindPointOnCircle(FVector Center, float Angle, float Radius = 100);
	TArray<FVector> GetPointsOnCircle(FVector Center, bool bIsRand = false, float DefaultAngle = 0, int PointsCount = 3, float Radius = 100);

	//Find positions on FaceOff
	TMap<ECharacterPosition, FVector> GetCenterPoints(ATriggerBox* MatchAreaBox, ETeam Team, EIHMatchGameMode MatchGameMode, float Radius, float CenterPointOffset = 100);

	
	

}
