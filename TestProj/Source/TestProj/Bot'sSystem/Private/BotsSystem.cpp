#include "TestProj/Bot'sSystem/Public/BotsSystem.h"

#include "DrawDebugHelpers.h"
#include "Camera/CameraActor.h"
#include "EngineUtils.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/TriggerBox.h"
#include "TestProj/Bot'sSystem/Public/GlobalCamera.h"

DEFINE_LOG_CATEGORY_STATIC(BotSystem, Log, All)

void BotsSystem::Update(UWorld* World, TArray<FBotParam> BotsParams, FPuckParam PuckParams)
{
	UpdateBotsDebug(World, BotsParams);
	UpdatePuckDebug(World, PuckParams);
}

void BotsSystem::Init(UWorld* World, APlayerController* PC, EIHMatchRulesMode Rules, TArray<FBotParam>& BotsParams, FPuckParam& PuckParams, AGlobalCamera*& GlobalCamera, ATriggerBox*& MatchAreaBox, EIHMatchGameMode MatchGameMode, bool bIsGoalTop)
{
	if(!World)
	{
		UE_LOG(BotSystem, Error, TEXT("Init::World is not find"));
	}
	if(!PC)
	{
		UE_LOG(BotSystem, Error, TEXT("Init::PC is not find"));
	}
	CameraSpawn(World, GlobalCamera);
	FindBoxActor(World, MatchAreaBox);
	UE_LOG(BotSystem, Display, TEXT("MatchMode: %s"), *UEnum::GetValueAsString(MatchGameMode));
	if (GlobalCamera)
	{
		SetGlobalCameraView(PC, GlobalCamera);
		if (MatchAreaBox && GlobalCamera)
		{
			SetBotsParams(MatchAreaBox, MatchGameMode, BotsParams, 170);
			SetPuckParams(MatchAreaBox, PuckParams);
			SetRules(Rules, MatchAreaBox, BotsParams, MatchGameMode, bIsGoalTop, 170);
		}
	}
}

void BotsSystem::FindBoxActor(UWorld* World, ATriggerBox*& MatchAreaBox)
{
	UE_LOG(BotSystem, Display, TEXT("<------------------------------------>"));
	UE_LOG(BotSystem, Display, TEXT("---FindBoxActor---"));
	for (TActorIterator<AActor> ActorItr(World); ActorItr; ++ActorItr)
	{
		AActor* FoundActor = *ActorItr;

		if (FoundActor->ActorHasTag("Box"))
		{
			MatchAreaBox = Cast<ATriggerBox>(FoundActor);
			UE_LOG(BotSystem, Display, TEXT("FindBoxActor::TriggerBox has been found"));
		}
	}
	if (MatchAreaBox == nullptr)
	{
		UE_LOG(BotSystem, Error, TEXT("FindBoxActor::TriggerBox was not found"));
	}
}

void BotsSystem::CameraSpawn(UWorld* World, AGlobalCamera*& GlobalCamera)
{
	UE_LOG(BotSystem, Display, TEXT("<------------------------------------>"));
	UE_LOG(BotSystem, Display, TEXT("---CameraSpawn---"));
	GlobalCamera = World->SpawnActor<AGlobalCamera>(AGlobalCamera::StaticClass(), FVector(0,0,2800), FRotator(-90,0,0));
	if(GlobalCamera)
	{
		GlobalCamera->GetCameraComponent()->SetConstraintAspectRatio(false);
		UE_LOG(BotSystem, Display, TEXT("CameraSpawn::GlobalCamera has been Spawned"));
	}
	else
	{
		UE_LOG(BotSystem, Error, TEXT("CameraSpawn::GlobalCamera was not found"));
	}
}

void BotsSystem::SetGlobalCameraView(APlayerController* PC, AGlobalCamera* GlobalCamera) 
{
	PC->SetViewTarget(GlobalCamera);
}

void BotsSystem::LogBotsParameters(FBotParam BotParam)
{
	UE_LOG(BotSystem, Display, TEXT("<------------------------------------>"));
	UE_LOG(BotSystem, Display, TEXT("---BotParams---"));
	UE_LOG(BotSystem, Display, TEXT("Position: X:%f, Y:%f, Z:%f"), BotParam.Position.X, BotParam.Position.Y, BotParam.Position.Z);
	UE_LOG(BotSystem, Display, TEXT("Direction: X:%f, Y:%f, Z:%f"), BotParam.Direction.X, BotParam.Direction.Y, BotParam.Direction.Z);
	UE_LOG(BotSystem, Display, TEXT("IsControlled: %s"), BotParam.IsControlled ? TEXT("true") : TEXT("false"));
	UE_LOG(BotSystem, Display, TEXT("Team: %s"), *UEnum::GetValueAsString(BotParam.Team));
	UE_LOG(BotSystem, Display, TEXT("Role: %s"), *UEnum::GetValueAsString(BotParam.Role));
}

void BotsSystem::SetBotsParams(ATriggerBox* MatchAreaBox, EIHMatchGameMode MatchGameMode, TArray<FBotParam>& BotsParamsArray, float CenterRadius)
{
	FBotParam BotParams;
	
	for(int i = 0; i < 6; ++i)
	{
		if (i % 2 == 0)
		{
			BotParams.Color = FColor::Red;
			BotParams.Team = ETeam::Team2;
		}
		else
		{
			BotParams.Color = FColor::Blue;
			BotParams.Team = ETeam::Team1;
		}
		BotParams.Role = static_cast<ECharacterPosition>(i/2 + 1);
		BotsParamsArray.Add(BotParams);
	}
	SetBotsFaceOffParams(MatchAreaBox, MatchGameMode, BotsParamsArray, CenterRadius);
	
	for(int i = 0; i < BotsParamsArray.Num(); ++i)
	{
		LogBotsParameters(BotsParamsArray[i]);
	}
}

void BotsSystem::SetRules(EIHMatchRulesMode Rules, ATriggerBox* MatchAreaBox, TArray<FBotParam>& BotsParamsArray, EIHMatchGameMode MatchGameMode, bool bIsGoalTop, float CenterRadius)
{
	UE_LOG(BotSystem, Display, TEXT("<------------------------------------>"));
	UE_LOG(BotSystem, Display, TEXT("---SetRules---"));
	UE_LOG(BotSystem, Display, TEXT("MatchRule: %s"), *UEnum::GetValueAsString(Rules));
	
	switch (Rules)
	{
	case EIHMatchRulesMode::FaceOff:
		{
			SetBotsFaceOffParams(MatchAreaBox, MatchGameMode, BotsParamsArray, CenterRadius);
			break;
		}
	case  EIHMatchRulesMode::Goal:
		{
			UE_LOG(BotSystem, Display, TEXT("MatchRule: %s"), bIsGoalTop ? TEXT("First team scored a goal") : TEXT("Second team scored a goal"));
			SetBotsGoalParams(MatchAreaBox, MatchGameMode, BotsParamsArray, bIsGoalTop, CenterRadius);
			break;
		}
	}
	for(int i = 0; i < BotsParamsArray.Num(); ++i)
	{
		LogBotsParameters(BotsParamsArray[i]);
	}
	UE_LOG(BotSystem, Display, TEXT("<------------------------------------>"));
}

void BotsSystem::SetBotsFaceOffParams(ATriggerBox* MatchAreaBox, EIHMatchGameMode MatchGameMode, TArray<FBotParam>& BotsParamsArray, float CenterRadius)
{
	UE_LOG(BotSystem, Display, TEXT("<------------------------------------>"));
	UE_LOG(BotSystem, Display, TEXT("---SetBotsFaceOffParams---"));
	TMap<ECharacterPosition, FVector> FirstTeamParams = GetCenterPoints(MatchAreaBox, ETeam::Team1, MatchGameMode, CenterRadius);
	TMap<ECharacterPosition, FVector> SecondTeamParams = GetCenterPoints(MatchAreaBox, ETeam::Team2, MatchGameMode, CenterRadius);
	
	for (int i = 0; i < BotsParamsArray.Num(); ++i)
	{
		if(BotsParamsArray[i].Team == ETeam::Team1)
		{
			BotsParamsArray[i].Position = FirstTeamParams[BotsParamsArray[i].Role];
			FVector Direction = (FVector::ZeroVector - BotsParamsArray[i].Position).GetSafeNormal();
			BotsParamsArray[i].Direction = Direction.ToOrientationQuat();
		}
		else
		{
			BotsParamsArray[i].Position = SecondTeamParams[BotsParamsArray[i].Role];
			FVector Direction = (FVector::ZeroVector - BotsParamsArray[i].Position).GetSafeNormal();
			BotsParamsArray[i].Direction = Direction.ToOrientationQuat();
		}
	}
	
	
}

void BotsSystem::SetBotsGoalParams(ATriggerBox* MatchAreaBox, EIHMatchGameMode MatchGameMode, TArray<FBotParam>& BotsParamsArray, bool bIsGoalTop, float CenterRadius)
{
	UE_LOG(BotSystem, Display, TEXT("<------------------------------------>"));
	UE_LOG(BotSystem, Display, TEXT("---SetBotsGoalParams---"));
	switch (MatchGameMode)
	{
	case EIHMatchGameMode::MatchGameMode_3x3:
		{
			FVector QuarterPoint = GetRandQuarterCenter(MatchAreaBox, bIsGoalTop);
			TArray<FVector> PointsNearQuarterPoint = GetPointsOnCircle(QuarterPoint, true);
			if(bIsGoalTop)
			{
				TMap<ECharacterPosition, FVector> SecondTeamParams = GetCenterPoints(MatchAreaBox, ETeam::Team2, MatchGameMode, CenterRadius);
				for (int i = 0; i < BotsParamsArray.Num(); ++i)
				{
					if(BotsParamsArray[i].Team == ETeam::Team1)
					{
						BotsParamsArray[i].Position = PointsNearQuarterPoint[i/2];
						FVector Direction = (QuarterPoint - BotsParamsArray[i].Position).GetSafeNormal();
						BotsParamsArray[i].Direction = Direction.ToOrientationQuat();
					}
					else
					{
						BotsParamsArray[i].Position = SecondTeamParams[BotsParamsArray[i].Role];
						FVector Direction = (FVector::ZeroVector - BotsParamsArray[i].Position).GetSafeNormal();
						BotsParamsArray[i].Direction = Direction.ToOrientationQuat();
					}
				}
			}
			else
			{
				TMap<ECharacterPosition, FVector> FirstTeamParams = GetCenterPoints(MatchAreaBox, ETeam::Team1, MatchGameMode, CenterRadius);
				for (int i = 0; i < BotsParamsArray.Num(); ++i)
				{
					if(BotsParamsArray[i].Team == ETeam::Team2)
					{
						BotsParamsArray[i].Position = PointsNearQuarterPoint[i/2];
						FVector Direction = (QuarterPoint - BotsParamsArray[i].Position).GetSafeNormal();
						BotsParamsArray[i].Direction = Direction.ToOrientationQuat();
					}
					else
					{
						BotsParamsArray[i].Position = FirstTeamParams[BotsParamsArray[i].Role];
						FVector Direction = (FVector::ZeroVector - BotsParamsArray[i].Position).GetSafeNormal();
						BotsParamsArray[i].Direction = Direction.ToOrientationQuat();
					}
				}
			}
			break;
		}
	}
}

void BotsSystem::SetPuckParams(ATriggerBox* MatchAreaBox, FPuckParam& Puck)
{
	FTransform CenterTransform = MatchAreaBox->GetActorTransform();
	Puck.Position = CenterTransform.TransformPosition(FVector(0,0,0));
}

void BotsSystem::UpdateBotsDebug(UWorld* World, TArray<FBotParam> BotsParams)
{
	for (int i = 0; i < BotsParams.Num(); ++i)
	{
		if(BotsParams[i].IsControlled)
		{
			DrawDebug(World, BotsParams[i].Position, FColor::Yellow);
			DrawDirectionDebug (World, BotsParams[i], FColor::Yellow);
		}
		else
		{
			DrawDebug(World, BotsParams[i].Position, BotsParams[i].Color);
			DrawDirectionDebug (World, BotsParams[i], BotsParams[i].Color);
		}
	}
}

void BotsSystem::UpdatePuckDebug(UWorld* World, FPuckParam Puck)
{
	if(Puck.IsControlled)
	{
		DrawDebug(World, Puck.Position, FColor::Yellow, 30.f);
	}
	else
	{
		DrawDebug(World, Puck.Position, FColor::Black, 30.f);
	}
}

void BotsSystem::DrawDebug(UWorld* World, FVector Position, FColor Color, float Radius, int32 Segments, float Thickness, float Duration)
{
	DrawDebugCircle(
		World,
		Position,
		Radius,
		Segments,
		Color,
		false,
		Duration,
		0,
		Thickness,
		FVector::RightVector,
		FVector::ForwardVector,
		false
	);
}

void BotsSystem::DrawDirectionDebug(UWorld* World, FBotParam BotsParams, FColor Color, float Radius, float Thickness, float Duration)
{
	FVector ForwardVector = BotsParams.Direction.GetForwardVector();
	FVector RightVector = BotsParams.Direction.GetRightVector();
	FVector Center = BotsParams.Position;
	
	TArray<FVector> Verts;
	Verts.Add(Center + ForwardVector * Radius);
	Verts.Add(Center + RightVector * Radius);
	Verts.Add(Center - RightVector * Radius);
	
	TArray<int32> Indices;
	Indices.Add(0);
	Indices.Add(1);
	Indices.Add(2);
	
	FVector Point1 = Center + ForwardVector * Radius;
	FVector Point2 = Center + RightVector * Radius;
	FVector Point3 = Center - RightVector * Radius;
	
	DrawDebugMesh(World, Verts, Indices, Color, false, Duration);
}

void BotsSystem::FindMouseClickPosition(UWorld* World, APlayerController* PC, ABaseGameMode* GameMode)
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

			CheckClickDistance(ClickWorldPos, GameMode->GameModeParams.BotsParamArray, GameMode->GameModeParams.PuckParam);

			
			float Duration = 2.f;
			float Size = 10.f;

			DrawDebugPoint(
				World,
				ClickWorldPos,
				Size,
				FColor::Yellow,
				false,
				Duration,
				0);
		}
	}
}

void BotsSystem::CheckClickDistance(FVector ClickPosition, TArray<FBotParam>& BotsParams, FPuckParam& PuckParam)
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
			LogBotsParameters(BotsParams[i]);
		}
		else
		{
			BotsParams[i].IsControlled = false;
		}
	}
}

FVector BotsSystem::GetRandQuarterCenter(ATriggerBox* MatchAreaBox, bool bTop)
{
	UBoxComponent* Box = Cast<UBoxComponent>(MatchAreaBox->GetCollisionComponent());

	FVector Center = Box->GetComponentLocation();
	FVector Extent = Box->GetScaledBoxExtent();

	FVector Forward = Box->GetForwardVector();
	FVector Right = Box->GetRightVector();

	//Точки спереди и сзади бокса
	FVector Front = Center + Forward * Extent.X;
	FVector Back = Center - Forward * Extent.X;
	//Точки справа и слева бокса
	FVector RightSide = Center + Right * Extent.Y;
	FVector LeftSide = Center - Right * Extent.Y;

	bool bRight = FMath::RandBool();

	FVector FrontOrBack = bTop ? Front : Back;
	FVector RightOrLeft = bRight ? RightSide : LeftSide;

	FVector CenterQuarter = (FrontOrBack + RightOrLeft) * 0.5f;
	
	UE_LOG(BotSystem, Display, TEXT("------------------------------------"));
	UE_LOG(BotSystem, Display, TEXT("---GetRandQuarterCenter---"))
	UE_LOG(BotSystem, Display, TEXT("CenterPosition: X: %f, Y:%f, Z:%f"), CenterQuarter.X, CenterQuarter.Y, CenterQuarter.Z);

	return CenterQuarter;
}

TArray<FVector> BotsSystem::GetPointsOnCircle(FVector Center, bool bIsRand, float DefaultAngle, int PointsCount, float Radius)
{
	if(PointsCount <= 0)
	{
		UE_LOG(BotSystem, Warning, TEXT("GetPointsOnCircle::Points Count <= 0"));
	}
	TArray<FVector> CirclePoints;
	float Angle = DefaultAngle;
	if(bIsRand)
	{
		Angle = FMath::FRandRange(0, 360);
	}
	
	UE_LOG(BotSystem, Display, TEXT("------------------------------------"));
	UE_LOG(BotSystem, Display, TEXT("---GetPointsOnCircle---"))
	
	float AngleOffset = 360 / PointsCount;
	for (int i = 0; i < PointsCount; ++i)
	{
		CirclePoints.Add(FindPointOnCircle(Center, Angle + (AngleOffset * i), Radius));
		UE_LOG(BotSystem, Display, TEXT("CenterPosition: X: %f, Y:%f, Z:%f"), CirclePoints[i].X, CirclePoints[i].Y, CirclePoints[i].Z);
	}
	return CirclePoints;
}

FVector BotsSystem::FindPointOnCircle(FVector Center, float Angle, float Radius)
{
	float Rad = FMath::DegreesToRadians(Angle);

	float X = Center.X + Radius * FMath::Cos(Rad);
	float Y = Center.Y + Radius * FMath::Sin(Rad);

	return FVector(X, Y, Center.Z);
}

TMap<ECharacterPosition, FVector> BotsSystem::GetCenterPoints(ATriggerBox* MatchAreaBox, ETeam Team, EIHMatchGameMode MatchGameMode, float Radius, float CenterPointOffset)
{
	const FVector Center = MatchAreaBox->GetActorLocation();
	const FVector Forward = MatchAreaBox->GetActorForwardVector();
	const FVector Right = MatchAreaBox->GetActorRightVector();
	
	TMap<ECharacterPosition, FVector> PointsArray;

	switch (MatchGameMode)
	{
	case EIHMatchGameMode::MatchGameMode_3x3:
		{
			FVector Dir1;
			FVector Dir2;

			switch (Team)
			{
			case ETeam::Team1:
				Dir1 = -Forward + Right; //Низ-право
				Dir2 = -Forward - Right; //Низ-лево

				PointsArray.Add(ECharacterPosition::RightWing, Center + Dir1 * Radius);
				PointsArray.Add(ECharacterPosition::LeftWing, Center + Dir2 * Radius);
				PointsArray.Add(ECharacterPosition::Center, Center - Forward * CenterPointOffset);
				break;
			case ETeam::Team2:
				Dir1 = Forward + Right; //Верх-право
				Dir2 = Forward - Right; //Верх-лево

				PointsArray.Add(ECharacterPosition::LeftWing, Center + Dir1 * Radius);
				PointsArray.Add(ECharacterPosition::RightWing, Center + Dir2 * Radius);
				PointsArray.Add(ECharacterPosition::Center, Center + Forward * CenterPointOffset);
				break;
			}
			break;
		}
	}
	
	UE_LOG(BotSystem, Display, TEXT("------------------------------------"))
	UE_LOG(BotSystem, Display, TEXT("---GetCenterPoints: %s---"), *UEnum::GetValueAsString(Team))  
	for (TMap<ECharacterPosition, FVector>::TIterator It(PointsArray); It; ++It)  
	{  
		UE_LOG(BotSystem, Display, TEXT("Role: %s"), *UEnum::GetValueAsString(It->Key));  
		UE_LOG(BotSystem, Display, TEXT("Position: X: %f, Y:%f, Z:%f"), It->Value.X, It->Value.Y, It->Value.Z);  
	}  
	
	return PointsArray;
}