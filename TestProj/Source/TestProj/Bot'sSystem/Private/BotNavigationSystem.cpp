#include "TestProj/Bot'sSystem/Public/BotNavigationSystem.h"


#include "DrawDebugHelpers.h"
#include "Components/BoxComponent.h"
#include "TestProj/Bot'sSystem/Public/BotsSystem.h"

void BotNavigationSystem::Update(float DeltaTime, APlayerController* PC, UBoxComponent* Box, UWorld* World, ETeam Team,
                                 TArray<FBotParam>& BotsParams, FPuckParam& PuckParams, TArray<FZone>& Zones, TArray<FGoalParams> GoalsParams)
{
	if (PuckParams.IsControlled)
	{
		UpdatePuckPosition(DeltaTime, Box, PC, PuckParams);
	}
	UpdateBotsPosition(World, DeltaTime, Box, Zones, BotsParams, PuckParams, GoalsParams, ETeam::Team1);
	//UpdateBotsPosition(World, DeltaTime, Box, Zones, BotsParams, PuckParams, GoalsParams, ETeam::Team2);
}

void BotNavigationSystem::Init(APlayerController* PC, UBoxComponent* Box, UWorld* World, TArray<FBotParam>& BotsParams,
	FPuckParam& PuckParams, TArray<FZone>& Zones)
{
	SetZones(Zones, Box);
}

FVector BotNavigationSystem::FindMouseClickPosition(APlayerController* PC)
{
	float MouseX, MouseY;
	PC->GetMousePosition(MouseX, MouseY);
	
	FVector CamLoc;
	FRotator CamRot;
	PC->GetPlayerViewPoint(CamLoc, CamRot);
	

	const FVector Forward = CamRot.Vector();
	const FVector Right = FRotationMatrix(CamRot).GetScaledAxis(EAxis::Y);
	const FVector Up = FRotationMatrix(CamRot).GetScaledAxis(EAxis::Z); 

	int32 SizeX, SizeY;
	PC->GetViewportSize(SizeX, SizeY);

	const float NDC_X = (MouseX / SizeX) * 2.f - 1.f;
	const float NDC_Y = -(MouseY / SizeY) * 2.f + 1.f;

	FVector RayDir =
		Forward +
		Right * NDC_X +
		(Up * (static_cast<float>(SizeY)/static_cast<float>(SizeX))) * NDC_Y;

	RayDir.Normalize();

	const FVector PlanePoint = FVector(0, 0, 0);  
	const FVector PlaneNormal = FVector::UpVector;

	const float Denom = FVector::DotProduct(PlaneNormal, RayDir);

	if (FMath::Abs(Denom) > KINDA_SMALL_NUMBER)
	{
		const float T = FVector::DotProduct(
			PlanePoint - CamLoc,
			PlaneNormal
		) / Denom;

		if (T >= 0)
		{
			const FVector ClickWorldPos = CamLoc + RayDir * T;
			
			return ClickWorldPos;
		}
	}
	return FVector::ZeroVector;
}

void BotNavigationSystem::CheckClickDistance(FVector ClickPosition, TArray<FBotParam>& BotsParams,
	FPuckParam& PuckParam)
{
	if(FVector::Distance(PuckParam.Position, ClickPosition) < 30)
	{
		PuckParam.IsControlled = true;
	}
	else
	{
		PuckParam.IsControlled = false;
	}
	
	for (int i = 0; i < BotsParams.Num(); ++i)
	{
		if(FVector::Distance(BotsParams[i].Position, ClickPosition) < 50)
		{
			BotsParams[i].IsControlled = true;
			BotsSystem::LogBotsParameters(BotsParams[i]);
		}
		else
		{
			BotsParams[i].IsControlled = false;
		}
	}
}

void BotNavigationSystem::UpdatePuckPosition(float DeltaTime, UBoxComponent* Box, APlayerController* PC, FPuckParam& PuckParams)
{
	FVector MousePosition = FindMouseClickPosition(PC);
	
	FTransform BoxTransform = Box->GetComponentTransform();
	FVector BoxExtent = Box->GetScaledBoxExtent();
	FVector LocalPos = BoxTransform.InverseTransformPosition(MousePosition);
	
	LocalPos.X = FMath::Clamp(LocalPos.X, -BoxExtent.X, BoxExtent.X);
	LocalPos.Y = FMath::Clamp(LocalPos.Y, -BoxExtent.Y, BoxExtent.Y);
	LocalPos.Z = FMath::Clamp(LocalPos.Z, -BoxExtent.Z, BoxExtent.Z);
	
	FVector ClampedWorldPos = BoxTransform.TransformPosition(LocalPos);
	
	PuckParams.Position = FMath::VInterpTo(PuckParams.Position, ClampedWorldPos, DeltaTime, 6);
}

void BotNavigationSystem::StopControl(TArray<FBotParam>& BotsParams, FPuckParam& PuckParams)
{
	PuckParams.IsControlled = false;
	
	for (int i = 0; i < BotsParams.Num(); ++i)
	{
		BotsParams[i].IsControlled = false;
	}
}

void BotNavigationSystem::SetZones(TArray<FZone>& Zones, UBoxComponent* Box)
{
	FVector BoxExtent = Box->GetScaledBoxExtent();
	
	FZone Team1LeftZone;
	FZone Team1RightZone;
	FZone Team1CenterZone;
	
	FZone Team2LeftZone;
	FZone Team2RightZone;
	FZone Team2CenterZone;
	
	Team1LeftZone.Min = Box->GetComponentTransform().TransformPosition(FVector(-BoxExtent.X, -BoxExtent.Y, 0));
	Team1LeftZone.Max = Box->GetComponentTransform().TransformPosition(FVector(-BoxExtent.X * 0.3, 0, 0));
	Team1LeftZone.ZoneRole = ECharacterPosition::LeftWing;
	Team1LeftZone.Direction = Box->GetForwardVector().ToOrientationQuat();
	Team1LeftZone.ZoneTeam = ETeam::Team1;
	
	Team1RightZone.Min = Box->GetComponentTransform().TransformPosition(FVector(-BoxExtent.X, 0, 0));
	Team1RightZone.Max = Box->GetComponentTransform().TransformPosition(FVector(-BoxExtent.X * 0.3, BoxExtent.Y, 0));
	Team1RightZone.ZoneRole = ECharacterPosition::RightWing;
	Team1RightZone.Direction = Box->GetForwardVector().ToOrientationQuat();
	Team1RightZone.ZoneTeam = ETeam::Team1;
	
	Team1CenterZone.Min = Box->GetComponentTransform().TransformPosition(FVector(-BoxExtent.X * 0.3, -BoxExtent.Y, 0));
	Team1CenterZone.Max = Box->GetComponentTransform().TransformPosition(FVector(0, BoxExtent.Y, 0));
	Team1CenterZone.ZoneRole = ECharacterPosition::Center;
	Team1CenterZone.Direction = Box->GetForwardVector().ToOrientationQuat();
	Team1CenterZone.ZoneTeam = ETeam::Team1;
	
	FVector ZoneTeam2ForwardVector = Box->GetForwardVector() * -1;
	
	Team2LeftZone.Min = Box->GetComponentTransform().TransformPosition(FVector(BoxExtent.X * 0.3, 0, 0));
	Team2LeftZone.Max = Box->GetComponentTransform().TransformPosition(FVector(BoxExtent.X, BoxExtent.Y, 0));
	Team2LeftZone.ZoneRole = ECharacterPosition::LeftWing;
	Team2LeftZone.Direction = ZoneTeam2ForwardVector.ToOrientationQuat();
	Team2LeftZone.ZoneTeam = ETeam::Team2;
	
	Team2RightZone.Min = Box->GetComponentTransform().TransformPosition(FVector(BoxExtent.X * 0.3, -BoxExtent.Y, 0));
	Team2RightZone.Max = Box->GetComponentTransform().TransformPosition(FVector(BoxExtent.X, 0, 0));
	Team2RightZone.ZoneRole = ECharacterPosition::RightWing;
	Team2RightZone.Direction = ZoneTeam2ForwardVector.ToOrientationQuat();
	Team2RightZone.ZoneTeam = ETeam::Team2;
	
	Team2CenterZone.Min = Box->GetComponentTransform().TransformPosition(FVector(0, -BoxExtent.Y, 0));
	Team2CenterZone.Max = Box->GetComponentTransform().TransformPosition(FVector(BoxExtent.X * 0.3, BoxExtent.Y, 0));
	Team2CenterZone.ZoneRole = ECharacterPosition::Center;
	Team2CenterZone.Direction = ZoneTeam2ForwardVector.ToOrientationQuat();
	Team2CenterZone.ZoneTeam = ETeam::Team2;
	
	Zones.Add(Team1LeftZone);
	Zones.Add(Team1RightZone);
	Zones.Add(Team1CenterZone);
	
	Zones.Add(Team2LeftZone);
	Zones.Add(Team2RightZone);
	Zones.Add(Team2CenterZone);
}

FZone BotNavigationSystem::GetZoneWithRole(TArray<FZone> Zones, ECharacterPosition Role, ETeam Team)
{
	for (int i = 0; i < Zones.Num(); ++i)
	{
		if (Zones[i].ZoneRole == Role && Zones[i].ZoneTeam == Team)
		{
			return Zones[i];
		}
	}
	return FZone();
}

FBotParam* BotNavigationSystem::FollowPuck(float DeltaTime, TArray<FBotParam>& BotsParams, FPuckParam& PuckParams, ETeam Team, TArray<FZone> Zones, UBoxComponent* Box)
{
	FBotParam* NearestBot = FindNearestBotToPuck(BotsParams, PuckParams, Team);
	
	if (NearestBot)
	{
		FVector Direction = (PuckParams.Position - NearestBot->Position).GetSafeNormal();  
  
		NearestBot->Direction = Direction.ToOrientationQuat();  
		FVector BotForwardVector = NearestBot->Direction.GetForwardVector();
	
		float Distance = FVector::Distance(PuckParams.Position, NearestBot->Position);  
		FVector PointNearPuck = NearestBot->Position + BotForwardVector * (Distance - 100);
	
		//FVector ClampPosition = GetZoneWithRole(Zones, NearestBot->Role, Team).ClampToZone(PointNearPuck, Box);
	
		NearestBot->Position = FMath::VInterpTo(NearestBot->Position, PointNearPuck, DeltaTime, 2);
		
		return NearestBot;
	}
	return nullptr;
}

FVector BotNavigationSystem::LookingOnPuck(FBotParam& CurrentBot, FPuckParam PuckParams, TArray<FZone> Zones, UBoxComponent* Box)
{
	float DistanceToPuck = FVector::Distance(PuckParams.Position, CurrentBot.Position);
	FVector ClampPosition = CurrentBot.Position;
	if (DistanceToPuck < 100)
	{
		FVector Direction = (PuckParams.Position - CurrentBot.Position).GetSafeNormal();
		CurrentBot.Direction = Direction.ToOrientationQuat(); 
		FVector Point = CurrentBot.Position - Direction * (100 - DistanceToPuck);
		ClampPosition = GetZoneWithRole(Zones, CurrentBot.Role, CurrentBot.Team).ClampToZone(Point);
	}
	else if (DistanceToPuck > 100)
	{
		FVector Direction = (PuckParams.Position - CurrentBot.Position).GetSafeNormal();
		CurrentBot.Direction = Direction.ToOrientationQuat(); 
		FVector Point = CurrentBot.Position + Direction * (DistanceToPuck - 100);
		ClampPosition = GetZoneWithRole(Zones, CurrentBot.Role, CurrentBot.Team).ClampToZone(Point);
	}
	
	return ClampPosition;
}

void BotNavigationSystem::UpdateBotsPosition(UWorld* World, float DeltaTime, UBoxComponent* Box, TArray<FZone> Zones, TArray<FBotParam>& BotsParams, FPuckParam& PuckParams, TArray<FGoalParams> GoalsParams, ETeam Team)
{
	FVector BotPosition = FVector::ZeroVector;
	//FBotParam* PuckBot = FollowPuck(DeltaTime, BotsParams, PuckParams, Team, Zones, Box);
	FGoalParams CurrentGoalParams = GetTeamGoalParams(Team, GoalsParams);
	for (auto& Bot : BotsParams)
	{
		if (Bot.Team == Team)
		{
			if (true) //Bot != *PuckBot
			{
				BotPosition = LookingOnPuck(Bot, PuckParams, Zones, Box);
				if (!IsNearestToGoal(BotsParams, Bot, CurrentGoalParams))
				{
					BotPosition = WingOffset(BotPosition, Bot, Zones);
				}
				FVector PositionNearGoals = OffsetToGoals(CurrentGoalParams, Box);
				BotPosition = GetPointToGoals(Bot, BotPosition, PositionNearGoals, PuckParams, Zones);
				
				BotPosition = CheckNearBots(BotsParams, Bot, BotPosition, Team, Zones);
				
				Bot.Position = Bot.Position = FMath::VInterpTo(Bot.Position, BotPosition, DeltaTime, 10);
			}
		}
	}
}

FBotParam* BotNavigationSystem::FindNearestBotToPuck(TArray<FBotParam>& BotsParams, FPuckParam PuckParams, ETeam Team)
{
	FBotParam* NearestBot = nullptr;
	float LowestDistance = MAX_flt;
	
	
	for (int i = 0; i < BotsParams.Num(); ++i)
	{
		if (BotsParams[i].Team == Team)
		{
			float DistanceToPuck = FVector::Distance(BotsParams[i].Position, PuckParams.Position);
			if (DistanceToPuck < LowestDistance)
			{
				LowestDistance = DistanceToPuck;
				NearestBot = &BotsParams[i];
			}
		}
	}
	
	return NearestBot;
}

FVector BotNavigationSystem::CheckNearBots(TArray<FBotParam>& BotsParams, FBotParam& CurrentBot, FVector TargetPosition, ETeam Team, TArray<FZone> Zones)
{
	for (auto& Bot : BotsParams)
	{
		if (Bot.Team == Team && Bot != CurrentBot)
		{
			float DistanceToPlayer = FVector::Distance(Bot.Position, TargetPosition);
			if (DistanceToPlayer < 300)
			{
				FVector DirectionToPlayer = (Bot.Position - TargetPosition).GetSafeNormal();
				FVector Point = TargetPosition - DirectionToPlayer * (300 - DistanceToPlayer);
				FVector ClampPosition = GetZoneWithRole(Zones, CurrentBot.Role, Team).ClampToZone(Point);
				return ClampPosition;
			}
		}
	}
	return TargetPosition;
}

FVector BotNavigationSystem::OffsetToGoals(FGoalParams CurrentGoalParams, UBoxComponent* Box)
{
	FVector BoxCenter = Box->GetComponentLocation();
	FVector DirectionToGoals = (BoxCenter - CurrentGoalParams.Position).GetSafeNormal();
	FVector PointNearGoals = CurrentGoalParams.Position + DirectionToGoals * 300;

	return PointNearGoals;
}

FGoalParams BotNavigationSystem::GetTeamGoalParams(ETeam Team, TArray<FGoalParams> GoalsParams)
{
	for (auto Goal : GoalsParams)
	{
		if (Goal.GoalTeam == Team)
		{
			return Goal;
		}
	}
	return FGoalParams();
}

FVector BotNavigationSystem::WingOffset(FVector TargetPosition, FBotParam& BotParams, TArray<FZone> Zones)
{
	FZone Zone = GetZoneWithRole(Zones, BotParams.Role, BotParams.Team);
	FVector ZoneRightVector = Zone.Direction.GetRightVector();
	FVector ZoneForwardVector = Zone.Direction.GetForwardVector();
	switch (BotParams.Role)
	{
	case ECharacterPosition::LeftWing:
		{
			FVector Direction = -ZoneRightVector - ZoneForwardVector;
			FVector Point = TargetPosition + Direction * 100;
			FVector ClampPoint = Zone.ClampToZone(Point);
			return ClampPoint;
		}
	case ECharacterPosition::RightWing:
		{
			FVector Direction = ZoneRightVector - ZoneForwardVector;
			FVector Point = TargetPosition + Direction * 100;
			FVector ClampPoint = Zone.ClampToZone(Point);
			return ClampPoint;
		}
	}
	return TargetPosition;
}

FVector BotNavigationSystem::GetPointToGoals(FBotParam TargetBot, FVector TargetPosition, FVector PositionNearGoals, FPuckParam PuckParams, TArray<FZone> Zones)
{
	if (TargetBot.Role == ECharacterPosition::Center)
	{
		return TargetPosition;
	}
	FZone TargetZone = GetZoneWithRole(Zones, TargetBot.Role, TargetBot.Team);
	float DistancePuckToZone = FVector::Distance(PuckParams.Position, TargetZone.GetCenter());
	for (auto Zone : Zones)
	{
		if (Zone.ZoneRole != TargetZone.ZoneRole && Zone.ZoneRole != ECharacterPosition::Center && Zone.ZoneTeam == TargetZone.ZoneTeam)
		{
			float Distance = FVector::Distance(PuckParams.Position, Zone.GetCenter());
			if (DistancePuckToZone < Distance)
			{
				return TargetPosition;
			}
		}
	}
	
	FVector CenterZone = TargetZone.GetCenter();

	float DistanceToPuck = FVector::Distance(CenterZone, PuckParams.Position);
	
	FVector ZoneForwardVector = TargetZone.Direction.GetForwardVector();
	
	float Alpha = DistanceToPuck / 2000;
	
	FVector Result = FMath::Lerp(PositionNearGoals, TargetPosition, Alpha);
	
	FVector DirectionResultToGoalsPoint = Result - PositionNearGoals;
	
	float Dot = FVector::DotProduct(ZoneForwardVector, DirectionResultToGoalsPoint);
	
	if (Dot < 0.0f)
	{
		return PositionNearGoals;
	}
	
	Result = TargetZone.ClampToZone(Result);
	
	return Result;
}

bool BotNavigationSystem::IsNearestToGoal(TArray<FBotParam> BotsParams, FBotParam TargetBot, FGoalParams GoalsParams)
{
	float TargetBotDistance = FVector::Distance(TargetBot.Position, GoalsParams.Position);

	for (auto Bot : BotsParams)
	{
		float OtherBotDistance = FVector::Distance(Bot.Position, GoalsParams.Position);
		if (OtherBotDistance < TargetBotDistance)
		{
			return false;
		}
	}
	return true;
}






