#include "TestProj/Bot'sSystem/Public/BotsMovementSystem.h"

void BotsMovementSystem::Update(float DeltaTime, TMap<AStaticMeshActor*, ETeam>& BotsMap,
	TArray<FBotParam> BotsParams)
{
	UpdateActorBotsPosition(DeltaTime, BotsMap, BotsParams);
}

void BotsMovementSystem::UpdateActorBotsPosition(float DeltaTime, TMap<AStaticMeshActor*, ETeam>& BotsMap, TArray<FBotParam> BotsParams)
{
	TArray<AStaticMeshActor*> OccupiedActors;
	SetNearestActorToPuck(DeltaTime, BotsMap, BotsParams, OccupiedActors);
	SetOtherActorsToPoint(DeltaTime, BotsMap, BotsParams, OccupiedActors);
}

void BotsMovementSystem::SetNearestActorToPuck(float DeltaTime, TMap<AStaticMeshActor*, ETeam>& BotsMap, TArray<FBotParam> BotsParams, TArray<AStaticMeshActor*>& OccupiedActors)
{
	for (auto Bot : BotsParams)
	{
		if (Bot.Role == ECharacterPosition::Center)
		{
			SetNearestActorToPointPosition(DeltaTime, BotsMap, Bot, OccupiedActors);
		}
	}
}

void BotsMovementSystem::SetOtherActorsToPoint(float DeltaTime, TMap<AStaticMeshActor*, ETeam>& BotsMap,
	TArray<FBotParam> BotsParams, TArray<AStaticMeshActor*>& OccupiedActors)
{
	for (auto Bot : BotsParams)
	{
		if (Bot.Role != ECharacterPosition::Center)
		{
			SetNearestActorToPointPosition(DeltaTime, BotsMap, Bot, OccupiedActors);
		}
	}
}

void BotsMovementSystem::SetNearestActorToPointPosition(float DeltaTime, TMap<AStaticMeshActor*, ETeam>& BotsMap, FBotParam Point, TArray<AStaticMeshActor*>& OccupiedActors)
{
	float Distance                 = MAX_flt;
	AStaticMeshActor* NearestActor = nullptr;
			
	for (TMap<AStaticMeshActor*, ETeam>::TIterator It(BotsMap); It; ++It)
	{
		if (!OccupiedActors.Contains(It->Key) && Point.Team == It->Value)
		{
			float DistanceBotToPoint = FVector::VectorPlaneProject(It->Key->GetActorLocation() - Point.Position, FVector::UpVector).Size();
			if (DistanceBotToPoint < Distance)
			{
				Distance     = DistanceBotToPoint;
				NearestActor = It->Key;
			}
		}
	}
	OccupiedActors.Add(NearestActor);
			
	FVector Direction = (Point.Position - NearestActor->GetActorLocation()).GetSafeNormal();
	NearestActor->AddActorWorldOffset(Direction * 400 * DeltaTime);
	if (FVector::VectorPlaneProject(NearestActor->GetActorLocation() - Point.Position, FVector::UpVector).Size() < 1)
	{
		NearestActor->SetActorLocation(Point.Position);
	}
}
