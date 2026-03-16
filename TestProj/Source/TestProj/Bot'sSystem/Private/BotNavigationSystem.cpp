#include "TestProj/Bot'sSystem/Public/BotNavigationSystem.h"

#include "Components/BoxComponent.h"
#include "TestProj/Bot'sSystem/Public/BotsSystem.h"

void BotNavigationSystem::Update(float DeltaTime, APlayerController* PC, ATriggerBox* MatchAreaBox, UWorld* World, ETeam Team,
                                 TArray<FBotParam>& BotsParams, FPuckParam& PuckParams)
{
	if (PuckParams.IsControlled)
	{
		UpdatePuckPosition(DeltaTime, MatchAreaBox, World, PC, PuckParams);
	}
	UpdateBotsPosition(DeltaTime, Team, MatchAreaBox, World, BotsParams, PuckParams);
	
}

void BotNavigationSystem::Init(APlayerController* PC, ATriggerBox* MatchAreaBox, UWorld* World, TArray<FBotParam>& BotsParams,
	FPuckParam& PuckParams)
{
	
}

FVector BotNavigationSystem::FindMouseClickPosition(UWorld* World, APlayerController* PC)
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
			
			float Duration = 2.f;
			float Size = 10.f;

			/*DrawDebugPoint(
				World,
				ClickWorldPos,
				Size,
				FColor::Yellow,
				false,
				Duration,
				0);*/
			
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

void BotNavigationSystem::UpdatePuckPosition(float DeltaTime, ATriggerBox* MatchAreaBox, UWorld* World,
                                             APlayerController* PC, FPuckParam& PuckParams)
{
	FVector MousePosition = FindMouseClickPosition(World, PC);
	UBoxComponent* Box = Cast<UBoxComponent>(MatchAreaBox->GetCollisionComponent());
	
	FTransform BoxTransform = MatchAreaBox->GetActorTransform();
	FVector BoxExtent = Box->GetScaledBoxExtent();
	FVector LocalPos = BoxTransform.InverseTransformPosition(MousePosition);
	
	LocalPos.X = FMath::Clamp(LocalPos.X, -BoxExtent.X, BoxExtent.X);
	LocalPos.Y = FMath::Clamp(LocalPos.Y, -BoxExtent.Y, BoxExtent.Y);
	LocalPos.Z = FMath::Clamp(LocalPos.Z, -BoxExtent.Z, BoxExtent.Z);
	
	FVector ClampedWorldPos = BoxTransform.TransformPosition(LocalPos);
	
	PuckParams.Position = FMath::VInterpTo(PuckParams.Position, ClampedWorldPos, DeltaTime, 6);
}

void BotNavigationSystem::UpdateBotsPosition(float DeltaTime, ETeam Team, ATriggerBox* MatchAreaBox, UWorld* World,
	TArray<FBotParam>& BotsParams, FPuckParam& PuckParams)
{
	FBotParam* NearestBot = FindNearestBotToPuck(BotsParams, PuckParams, Team);
	
	for (int i = 0; i < BotsParams.Num(); ++i)
	{
		BotsParams[i].IsControlled = false;
	}
	NearestBot->IsControlled = true;
	
	FVector Direction = (PuckParams.Position - NearestBot->Position).GetSafeNormal();
	
	NearestBot->Direction = Direction.ToOrientationQuat();
	FVector BotForwardVector = NearestBot->Direction.GetForwardVector();
	
	float Distance = FVector::Distance(PuckParams.Position, NearestBot->Position);
	FVector PointNearPuck = NearestBot->Position + BotForwardVector * (Distance - 100);
			
	NearestBot->Position = FMath::VInterpTo(NearestBot->Position, PointNearPuck, DeltaTime, 2);
	
	testfunc(DeltaTime, BotsParams, PuckParams, NearestBot, Team);
}

void BotNavigationSystem::StopControl(TArray<FBotParam>& BotsParams, FPuckParam& PuckParams)
{
	PuckParams.IsControlled = false;
	
	for (int i = 0; i < BotsParams.Num(); ++i)
	{
		BotsParams[i].IsControlled = false;
	}
}

FBotParam* BotNavigationSystem::FindNearestBotToPuck(TArray<FBotParam>& BotsParams, FPuckParam& PuckParams, ETeam Team)
{
	FBotParam* NearestBot = nullptr;
	float LowestDistance = 1000;
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

void BotNavigationSystem::testfunc(float DeltaTime, TArray<FBotParam>& BotsParams, FPuckParam& PuckParams, FBotParam* NearestToPuck, ETeam Team)
{
	FVector BotWithPuckPosition = NearestToPuck->Position;
	float MinDistance = 300;
	float MaxDistance = 500;
	
	for (int i = 0; i < BotsParams.Num(); ++i)
	{
		if (BotsParams[i].Team == Team)
		{
			if (BotsParams[i].Role != NearestToPuck->Role)
			{
				FBotParam CurrentBot = BotsParams[i];
				for (int j = 0; j < BotsParams.Num(); ++j)
				{
					if (BotsParams[j].Team == Team)
					{
						if (BotsParams[j].Role != CurrentBot.Role && BotsParams[j].Role != NearestToPuck->Role)
						{
							float Distance = FVector::Distance(BotsParams[j].Position, CurrentBot.Position);
							
							if (Distance < MinDistance)
							{
								FVector Direction = (CurrentBot.Position - BotsParams[j].Position).GetSafeNormal();
								FVector Point = BotsParams[j].Position - Direction * (MinDistance - Distance);
								BotsParams[j].Position = FMath::VInterpTo(BotsParams[j].Position, Point, DeltaTime, 5);
							}
							else if (Distance > MaxDistance)
							{
								FVector Direction = (CurrentBot.Position - BotsParams[j].Position).GetSafeNormal();
								FVector Point = BotsParams[j].Position + Direction * (Distance - MaxDistance);
								BotsParams[j].Position = FMath::VInterpTo(BotsParams[j].Position, Point, DeltaTime, 5);
							}
						}
					}
				}
				float Distance = FVector::Distance(BotsParams[i].Position, NearestToPuck->Position);
				if (Distance < MinDistance)
				{
					FVector Direction = (BotWithPuckPosition - BotsParams[i].Position).GetSafeNormal();
					BotsParams[i].Direction = Direction.ToOrientationQuat();
					FVector BotForwardVector = BotsParams[i].Direction.GetForwardVector();
					FVector Point = BotsParams[i].Position - BotForwardVector * (MinDistance - Distance);
					BotsParams[i].Position = FMath::VInterpTo(BotsParams[i].Position, Point, DeltaTime, 5);
				}
				else if (Distance > MaxDistance)
				{
					FVector Direction = (BotWithPuckPosition - BotsParams[i].Position).GetSafeNormal();
					BotsParams[i].Direction = Direction.ToOrientationQuat();
					FVector BotForwardVector = BotsParams[i].Direction.GetForwardVector();
					FVector Point = BotsParams[i].Position + BotForwardVector * (Distance - MaxDistance);
					BotsParams[i].Position = FMath::VInterpTo(BotsParams[i].Position, Point, DeltaTime, 5);
				}
			}
		}
	}
}
