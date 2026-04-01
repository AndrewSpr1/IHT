#include "TestProj/Bot'sSystem/Public/BotNavigationSystem.h"


#include "DrawDebugHelpers.h"
#include "Components/BoxComponent.h"
#include "Math/UnitConversion.h"
#include "TestProj/Bot'sSystem/Public/BotsSystem.h"

#define NAV_SYSTEM_LOG_ENABLED 0

#if NAV_SYSTEM_LOG_ENABLED
	#define NAV_SYSTEM_LOG(Category, Verbosity, Format, ...) \
		UE_LOG(Category, Verbosity, Format, ##__VA_ARGS__)
#else
	#define NAV_SYSTEM_LOG(Category, Verbosity, Format, ...)
#endif

DEFINE_LOG_CATEGORY_STATIC(NavigationSystem, Log, All)

 

void BotNavigationSystem::Update(float DeltaTime, APlayerController* PC, UBoxComponent* Box, UWorld* World, ETeam Team,
                                 TArray<FBotParam>& BotsParams, FPuckParam& PuckParams, TArray<FZone>& Zones, TArray<FGoalParams> GoalsParams,
                                 const float Radius, const float OffsetUnderPuck, const float AngleBetweenBots, const float RightBotAngle, const float LeftBotAngle)
{
	const FVector BoxCenter = Box->GetComponentLocation();
	const FVector BoxForward = Box->GetForwardVector();
	const FVector BoxRight = Box->GetRightVector();
	const FVector BoxExtent = Box->GetScaledBoxExtent();
	
	if (PuckParams.IsControlled)
	{
		UpdatePuckPosition(DeltaTime, Box, PC, PuckParams);
	}
	
	for (auto& Bot : BotsParams)
	{
		const float DU = GetDU(BoxCenter, BoxExtent, Bot.ViewDirection, PuckParams.Position);
		const float DV = GetDV(BoxCenter, BoxExtent, Bot.ViewDirection, PuckParams.Position);
		BotPositionManagementNeutral(World, Bot, Box, DU, DV, PuckParams, Radius, AngleBetweenBots, RightBotAngle, LeftBotAngle);
	}
	
	
}

void BotNavigationSystem::Init(APlayerController* PC, UBoxComponent* Box, UWorld* World, TArray<FBotParam>& BotsParams,
	FPuckParam& PuckParams, TArray<FZone>& Zones)
{
	
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

//---------------------------------------Didn't Use---------------------------------------

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
	
	float Alpha = DistanceToPuck / 3000;
	
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

FVector BotNavigationSystem::BotDefaultPosition(ECharacterPosition Role, ETeam Team, const FVector PuckPos, const FVector BoxForward,
	const float OffsetBots, const float OffsetUnderPuck)
{
	FVector ResultPosition;
	
	const FVector ViewDirection = Team == ETeam::Team1 ? BoxForward * -1 : BoxForward;
	const FVector OffsetFromPuck      = PuckPos + ViewDirection * OffsetUnderPuck;

	switch (Role)
	{
		case ECharacterPosition::Center:
			{
				ResultPosition = GetBotPositionNearPuck(ViewDirection, PuckPos, PuckPos, 100);
				break;
			}
		case ECharacterPosition::LeftWing:
			{
				ResultPosition = OffsetFromPuck - (PuckPos - OffsetFromPuck).ToOrientationQuat().GetRightVector() * OffsetBots;
				break;
			}
		case ECharacterPosition::RightWing:
			{
				ResultPosition = OffsetFromPuck + (PuckPos - OffsetFromPuck).ToOrientationQuat().GetRightVector() * OffsetBots;
				break;
			}
	}
	
	return ResultPosition;
}

FVector BotNavigationSystem::BotOffsetFromSide(ETeam Team, const FVector BotPos, const FVector PuckPos, const FVector BoxCenter,
	const FVector BoxForward, const FVector BoxRight, const FVector BoxExtent, const float OffsetBots,
	const float OffsetUnderPuck)
{
	FVector ResultPosition;
	
	const FVector Dir                  = PuckPos - BoxCenter;
	const float PuckDotProductToCenter = BoxRight | Dir;
	
	const FVector ViewDirection = Team == ETeam::Team1 ? BoxForward * -1 : BoxForward;
	FVector OffsetFromPuck      = PuckPos + ViewDirection * OffsetUnderPuck;
		
	FVector BotDir = (BotPos - PuckPos).GetSafeNormal();
	const float BotDotProductToPuck = BotDir | BoxRight;

	FVector2D Input            = FVector2D(BotDotProductToPuck < 0 ? -BoxExtent.Y : BoxExtent.Y, 0);
	FVector2D Output           = FVector2D(0, OffsetBots);
	const float ClampedOffset  = FMath::GetMappedRangeValueClamped(Input, Output, PuckDotProductToCenter);
		
	if (BotDotProductToPuck < 0 && PuckDotProductToCenter < 0)
	{
		ResultPosition = OffsetFromPuck - BoxRight * ClampedOffset;
		return ResultPosition;
	}
	if (BotDotProductToPuck > 0 && PuckDotProductToCenter > 0)
	{
		ResultPosition = OffsetFromPuck + BoxRight * ClampedOffset;
		return ResultPosition;
	}
	
	return BotPos;
}

//--------------------------------------------Navigation with Radius-----------------------------------------

FVector BotNavigationSystem::ClampToZone(FVector Pos, UBoxComponent* Box)
{
	FTransform BoxTransform = Box->GetComponentTransform();
	FVector Position = BoxTransform.InverseTransformPosition(Pos);
	FVector ClosestPoint = BoxTransform.InverseTransformPosition(Pos);
	
	FVector BoxExtent = Box->GetScaledBoxExtent();
	FVector Min = FVector(-BoxExtent.X, -BoxExtent.Y, 0);
	FVector Max = FVector(BoxExtent.X, BoxExtent.Y, 0);
	
	if (Position.X < Min.X)
	{
		ClosestPoint.X = Min.X;
	}
	else if (Position.X > Max.X)
	{
		ClosestPoint.X = Max.X;
	}
	
	if (Position.Y < Min.Y)
	{
		ClosestPoint.Y = Min.Y;
	}
	else if (Position.Y > Max.Y)
	{
		ClosestPoint.Y = Max.Y;
	}
	ClosestPoint.Z = 0;
	
	ClosestPoint = BoxTransform.TransformPosition(ClosestPoint);
	
	return ClosestPoint;
}

void BotNavigationSystem::BotPositionManagementNeutral(
	UWorld* World
	, FBotParam& BotParams
	, UBoxComponent* Box
	, const float DU
	, const float DV
	, FPuckParam PuckParams
	, const float Radius
	, const float AngleBetweenBots
	, const float RightBotAngle
	, const float LeftBotAngle)
{
	
	FVector SectorDirection = BotParams.ViewDirection * -1;
	float CurrentDU = DU;
	
	
	auto AngleForDefence = GetOwnAngleForDefence(AngleBetweenBots, CurrentDU, DV);								//remade
	auto AngleForAttack  = GetOwnAngleForAttack(AngleBetweenBots, CurrentDU, DV, 60);				//new
	
	auto OwnRightAngle = AngleForDefence;
	auto OwnLeftAngle  = AngleForDefence;
	
	
	auto MainRadiusForDefence = GetMainRadiusForDefence(Radius, DU, DV, 500);
	
	auto OwnRightRadius = MainRadiusForDefence + GetOwnRadiusForDefence(DU, DV, 800);						//new
	auto OwnLeftRadius  = MainRadiusForDefence + GetOwnRadiusForDefence(DU, DV, 800, false);		//new

	
	const auto MainAngleForAttack  = GetMainVectorAngle(AngleBetweenBots, DU, DV, ETeamTactic::Attack);			//remade
	const auto MainAngleForDefence = GetMainVectorAngle(AngleBetweenBots, CurrentDU, DV, ETeamTactic::Defence);	//remade
	
	FVector Direction = SectorDirection.RotateAngleAxis(MainAngleForAttack + MainAngleForDefence, FVector::UpVector);
	
	
	
	BotParams.Position = GetPointsOnCircle(
		BotParams.Role
		, PuckParams.Position
		, SectorDirection
		, Direction
		, AngleBetweenBots
		, Radius
		, OwnLeftAngle
		, OwnRightAngle
		, OwnLeftRadius
		, OwnRightRadius);
	
	BotParams.Position = ClampToZone(BotParams.Position, Box);
	
}

FVector BotNavigationSystem::GetPointsOnCircle(
	ECharacterPosition Role
	, const FVector Center
	, const FVector DefaultSectorDirection
	, const FVector RotatedSectorDirection
	, const float Angle
	, const float Radius
	, const float LeftAngle
	, const float RightAngle
	, const float LeftRadius
	, const float RightRadius)
{
	
	const FVector NormalizeSectorDirection = RotatedSectorDirection.GetSafeNormal();
	
	switch (Role)
	{
		case ECharacterPosition::Center:
			{
				return GetBotPositionNearPuck(DefaultSectorDirection, Center, Center, 100);
			}
		case ECharacterPosition::LeftWing:
			{
				const auto ResultsAngle = LeftAngle == 0 ?  Angle : LeftAngle;
				const auto ResultRadius = LeftRadius == 0 ? Radius : LeftRadius;
				const FVector Dir = NormalizeSectorDirection.RotateAngleAxis(abs(ResultsAngle), FVector::UpVector);
				return Center + Dir * ResultRadius;
			}
		case ECharacterPosition::RightWing:
			{
				const auto ResultsAngle = RightAngle == 0 ?  Angle : RightAngle;
				const auto ResultRadius = RightRadius == 0 ? Radius : RightRadius;
				const FVector Dir = NormalizeSectorDirection.RotateAngleAxis(-abs(ResultsAngle), FVector::UpVector);
				return Center + Dir * ResultRadius;
			}
		
		default:
		break;
	}

	return FVector::ZeroVector;
}

float BotNavigationSystem::GetMainVectorAngle(const float AngleBetweenBots, const float DU, const float DV, const ETeamTactic Tactic)
{
	switch (Tactic)
	{
	case ETeamTactic::None:
		return DU * AngleBetweenBots;
	case ETeamTactic::Attack:
		if (DV > 0) return DU * AngleBetweenBots;
		break;
	case ETeamTactic::Defence:
		if (DV < 0) return DU * AngleBetweenBots;
		break;
	}
	return 0;
}

float BotNavigationSystem::GetOwnAngleForDefence(const float AngleBetweenBots, float& DU, const float DV,
	const float MaxAngleOffset)
{
	if (DV < 0)
	{
		const FVector2D Input1  = FVector2D(-0.4f, -0.8f);
		const FVector2D Output1 = FVector2D(0, MaxAngleOffset);
		float ClampDV           = FMath::GetMappedRangeValueClamped(Input1, Output1, DV);
		
		auto Angle = AngleBetweenBots + ClampDV;
		
		const FVector2D Input  = FVector2D(60.0f, MaxAngleOffset);
		const FVector2D Output = FVector2D(DU, DU * -1);
		float ClampAngle       = FMath::GetMappedRangeValueClamped(Input, Output, Angle);
		
		DU = ClampAngle;
		return Angle;
	}
	return 0;
}


float BotNavigationSystem::GetAngleForDefence(const float AngleBetweenBots, float& DU, const float DV, const float MaxAngleOffset)
{
	if (DV < 0)
	{
		const auto DVx2 = FMath::Clamp(DV*2, -1.0f, 1.0f);
		auto Angle = AngleBetweenBots + MaxAngleOffset * abs(DVx2);
		
		
		const FVector2D Input = FVector2D(70.0f, 90.0f);
		const FVector2D Output = FVector2D(DU, DU * -1);
		float Clamp = FMath::GetMappedRangeValueClamped(Input, Output, Angle);
		
		DU = Clamp;
		return Angle;
	}
	return 0;
}

float BotNavigationSystem::GetOwnAngleForAttack(const float AngleBetweenBots, float& DU, const float DV, const float MaxAngleOffset)
{
	if (DV > 0)
	{
		const auto DVx2 = FMath::Clamp(DV*2, -1.0f, 1.0f);
		auto Angle = AngleBetweenBots + MaxAngleOffset * abs(DVx2);
		
		
		const FVector2D Input = FVector2D(70.0f, AngleBetweenBots + MaxAngleOffset);
		const FVector2D Output = FVector2D(DU, DU * -1);
		float Clamp = FMath::GetMappedRangeValueClamped(Input, Output, Angle);
		
		DU = Clamp;
		return Angle;
	}
	return 0;
}

float BotNavigationSystem::GetMainRadiusForDefence(const float CurrentRadius, const float DU, const float DV,
	const float MinRadius)
{
	if (DV < 0)
	{
		auto X = MinRadius + (CurrentRadius - MinRadius) * ((FMath::Cos(2 * PI * DV) + 1) / 2);
		return X;
		
	}
	return CurrentRadius;
}

float BotNavigationSystem::GetOwnRadiusForDefence(const float DU, const float DV, const float MaxRadius, const bool bIsRight)
{
	if (DV < 0)
	{
		if (bIsRight)
		{
			if (DU < 0) return MaxRadius * abs(DV) * abs(DU);
		}
		else
		{
			if (DU > 0) return MaxRadius * abs(DV) * abs(DU);
		}
	}
	return 0;
}


FVector BotNavigationSystem::GetBotPositionNearPuck(const FVector ViewDirection, const FVector PuckLocation, const FVector CurrentBotLocation, const float Offset)
{
	if (FVector::VectorPlaneProject(PuckLocation - CurrentBotLocation, FVector::UpVector).Size() < Offset)
	{
		return PuckLocation + ViewDirection * Offset;
	}
	return CurrentBotLocation;
}

float BotNavigationSystem::GetDU(const FVector BoxCenter, const FVector BoxExtent, const FVector ViewDirection, const FVector PuckPos)
{
	const auto Dir = PuckPos - BoxCenter;
	const auto Alpha = ViewDirection.ToOrientationQuat().GetRightVector() | Dir;
	
	const auto Size = abs(BoxExtent.Y);
	const auto DU   = ClampFromMinusOneToOne(Size, Alpha);
	
	NAV_SYSTEM_LOG(NavigationSystem, Display, TEXT("------------"));
	NAV_SYSTEM_LOG(NavigationSystem, Display, TEXT("ViewDirection: %s"), *ViewDirection.ToString());
	NAV_SYSTEM_LOG(NavigationSystem, Display, TEXT("DU: %f"), DU);
	
	return DU;
}

float BotNavigationSystem::GetDV(const FVector BoxCenter, const FVector BoxExtent, const FVector ViewDirection, const FVector PuckPos)
{
	const auto Dir = PuckPos - BoxCenter;
	const auto Alpha = ViewDirection | Dir;
	
	const auto Size = abs(BoxExtent.X);
	const auto DV   = ClampFromMinusOneToOne(Size, Alpha);
	
	NAV_SYSTEM_LOG(NavigationSystem, Display, TEXT("------------"));
	NAV_SYSTEM_LOG(NavigationSystem, Display, TEXT("ViewDirection: %s"), *ViewDirection.ToString());
	NAV_SYSTEM_LOG(NavigationSystem, Display, TEXT("DV: %f"), DV);
	
	return DV;
}

float BotNavigationSystem::ClampFromMinusOneToOne(const float Size, const float Alpha)
{
	const FVector2D Input  = FVector2D(-Size, Size);
	const FVector2D Output = FVector2D(-1, 1);
	return FMath::GetMappedRangeValueClamped(Input, Output, Alpha);
}






