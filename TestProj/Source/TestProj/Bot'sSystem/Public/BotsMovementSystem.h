#pragma once
#include "BotParam.h"
#include "Engine/StaticMeshActor.h"

namespace BotsMovementSystem
{
	void Update(float DeltaTime, TMap<AStaticMeshActor*, ETeam>& BotsMap, TArray<FBotParam> BotsParams);
	void Init();
	
	void UpdateActorBotsPosition(float DeltaTime, TMap<AStaticMeshActor*, ETeam>& BotsMap, TArray<FBotParam> BotsParams);
	
	void SetNearestActorToPuck(float DeltaTime, TMap<AStaticMeshActor*, ETeam>& BotsMap, TArray<FBotParam> BotsParams, TArray<AStaticMeshActor*>& OccupiedActors);
	void SetOtherActorsToPoint(float DeltaTime, TMap<AStaticMeshActor*, ETeam>& BotsMap, TArray<FBotParam> BotsParams, TArray<AStaticMeshActor*>& OccupiedActors);
	
	void SetNearestActorToPointPosition(float DeltaTime, TMap<AStaticMeshActor*, ETeam>& BotsMap, FBotParam Point, TArray<AStaticMeshActor*>& OccupiedActors);
}
