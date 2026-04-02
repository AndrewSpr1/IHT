#pragma once
#include "BotParam.h"
#include "Components/BoxComponent.h"
#include "Engine/StaticMeshActor.h"

namespace BotsMovementSystem
{
	void Update(float DeltaTime, TMap<AStaticMeshActor*, ETeam>& BotsMap, TArray<FBotParam> BotsParams, TArray<UBoxComponent*> GoalsBoxes);
	void Init();
	
	void UpdateActorBotsPosition(float DeltaTime, TMap<AStaticMeshActor*, ETeam>& BotsMap, TArray<FBotParam> BotsParams, TArray<UBoxComponent*> GoalsBoxes);
	
	void SetNearestActorToPuck(float DeltaTime, TMap<AStaticMeshActor*, ETeam>& BotsMap, TArray<FBotParam> BotsParams, TArray<AStaticMeshActor*>& OccupiedActors, TArray<UBoxComponent*> GoalsBoxes);
	void SetOtherActorsToPoint(float DeltaTime, TMap<AStaticMeshActor*, ETeam>& BotsMap, TArray<FBotParam> BotsParams, TArray<AStaticMeshActor*>& OccupiedActors, TArray<UBoxComponent*> GoalsBoxes);
	
	void SetNearestActorToPointPosition(float DeltaTime, TMap<AStaticMeshActor*, ETeam>& BotsMap, FBotParam Point, TArray<AStaticMeshActor*>& OccupiedActors, TArray<UBoxComponent*> GoalsBoxes);
}
