#pragma once
#include "BotParam.h"
#include "PuckParam.h"
#include "Engine/TriggerBox.h"


namespace BotNavigationSystem
{
	void Update(float DeltaTime, APlayerController* PC, ATriggerBox* MatchAreaBox, UWorld* World, ETeam Team, TArray<FBotParam>& BotsParams, FPuckParam& PuckParams);
	
	void Init(APlayerController* PC, ATriggerBox* MatchAreaBox, UWorld* World, TArray<FBotParam>& BotsParams, FPuckParam& PuckParams);
	
	FVector FindMouseClickPosition(UWorld* World, APlayerController* PC);
	void CheckClickDistance(FVector ClickPosition, TArray<FBotParam>& BotsParams, FPuckParam& PuckParam);
	
	void UpdatePuckPosition(float DeltaTime, ATriggerBox* MatchAreaBox, UWorld* World, APlayerController* PC, FPuckParam& PuckParams);
	void UpdateBotsPosition(float DeltaTime, ETeam Team, ATriggerBox* MatchAreaBox, UWorld* World, TArray<FBotParam>& BotsParams, FPuckParam& PuckParams);
	void StopControl(TArray<FBotParam>& BotsParams, FPuckParam& PuckParams);
	
	FBotParam* FindNearestBotToPuck(TArray<FBotParam>& BotsParams, FPuckParam& PuckParams, ETeam Team);
	
	
	void testfunc(float DeltaTime, TArray<FBotParam>& BotsParams, FPuckParam& PuckParams, FBotParam* NearestToPuck, ETeam Team);
}
