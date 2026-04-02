#include "TestProj/Bot'sSystem/Public/BotsSystem.h"


#include "DrawDebugHelpers.h"
#include "Camera/CameraActor.h"
#include "EngineUtils.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/TriggerBox.h"
#include "TestProj/Bot'sSystem/Public/GlobalCamera.h"

DEFINE_LOG_CATEGORY_STATIC(BotSystem, Log, All)

void BotsSystem::Update(UWorld* World, float DeltaTime, TArray<FBotParam>& BotsParams, FPuckParam& PuckParams)
{
	UpdateBotsDebug(World, BotsParams);
	UpdatePuckDebug(World, PuckParams);
	
}

void BotsSystem::Init(UWorld* World,
					APlayerController* PC,
					EIHMatchRulesMode Rules,
					TArray<FBotParam>& BotsParams,
					bool bUseActorBots,
					TMap<AStaticMeshActor*, ETeam>& BotsMap,
					FPuckParam& PuckParams,
					AGlobalCamera*& GlobalCamera,
					ATriggerBox*& MatchAreaBox,
					TArray<ATriggerBox*>& GoalsTriggerBoxes,
					EIHMatchGameMode MatchGameMode,
					ETeam GoalToTeam,
					TArray<FGoalParams>& GoalsParams,
					TArray<FZone>& Zones)
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
	FindBoxActor(World, MatchAreaBox, GoalsTriggerBoxes);
	UE_LOG(BotSystem, Display, TEXT("MatchMode: %s"), *UEnum::GetValueAsString(MatchGameMode));
	
	UBoxComponent* Box = Cast<UBoxComponent>(MatchAreaBox->GetCollisionComponent());
	
	
	FVector BoxForward = Box->GetForwardVector();
	
	if (GlobalCamera)
	{
		SetGlobalCameraView(PC, GlobalCamera);
		if (MatchAreaBox && GlobalCamera)
		{
			SetZones(Zones, Box);
			SetBotsParams(BotsParams, BoxForward);
			SetBotsDefaultPositions(MatchAreaBox, MatchGameMode, BotsParams);
			SetPuckParams(MatchAreaBox, PuckParams);
			SetRules(Rules, MatchAreaBox, BotsParams, MatchGameMode, GoalToTeam);
			SetGoalsParams(Box, GoalsParams);

			if (bUseActorBots)
			{
				BotsSpawn(World, BotsParams, BotsMap);
			}
			
		}
	}
}

void BotsSystem::FindBoxActor(UWorld* World, ATriggerBox*& MatchAreaBox, TArray<ATriggerBox*>& GoalsTriggerBoxes)
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
		else if (FoundActor->ActorHasTag("Goals"))
		{
			GoalsTriggerBoxes.AddUnique(Cast<ATriggerBox>(FoundActor));
			UE_LOG(BotSystem, Display, TEXT("FindBoxActor::Goals has been found"));
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
	GlobalCamera = World->SpawnActor<AGlobalCamera>(AGlobalCamera::StaticClass(), FVector(0,0,3900), FRotator(-90,0,0));
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

void BotsSystem::BotsSpawn(UWorld* World, TArray<FBotParam> BotsParams, TMap<AStaticMeshActor*, ETeam>& BotsMap)
{
	UE_LOG(BotSystem, Display, TEXT("<------------------------------------>"));
	UE_LOG(BotSystem, Display, TEXT("---BotsSpawn---"));
	UStaticMesh* Mesh = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	UMaterialInterface* BlueTeamMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("Material'/Game/Materials/M_BlueTeam.M_BlueTeam'"));
	UMaterialInterface* RedTeamMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("Material'/Game/Materials/M_RedTeam.M_RedTeam'"));
	for (auto Bot : BotsParams)
	{
		AStaticMeshActor* SpawnedBot = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Bot.Position, Bot.Direction.Rotator());
		if (Mesh && BlueTeamMaterial && RedTeamMaterial)
		{
			SpawnedBot->GetStaticMeshComponent()->SetStaticMesh(Mesh);
			SpawnedBot->GetStaticMeshComponent()->SetMaterial(0, Bot.Team == ETeam::Team1 ? BlueTeamMaterial : RedTeamMaterial);
		}
		SpawnedBot->SetMobility(EComponentMobility::Movable);
		BotsMap.Add(SpawnedBot, Bot.Team);
	}
}

void BotsSystem::LogBotsParameters(FBotParam BotParam)
{
	UE_LOG(BotSystem, Display, TEXT("<------------------------------------>"));
	UE_LOG(BotSystem, Display, TEXT("---BotParams---"));
	UE_LOG(BotSystem, Display, TEXT("Position: %s"), *BotParam.Position.ToString());
	UE_LOG(BotSystem, Display, TEXT("Direction: %s"), *BotParam.Direction.ToString());
	UE_LOG(BotSystem, Display, TEXT("IsControlled: %s"), BotParam.IsControlled ? TEXT("true") : TEXT("false"));
	UE_LOG(BotSystem, Display, TEXT("Team: %s"), *UEnum::GetValueAsString(BotParam.Team));
	UE_LOG(BotSystem, Display, TEXT("Role: %s"), *UEnum::GetValueAsString(BotParam.Role));
}

void BotsSystem::LogZonesParameters(FZone ZoneParams)
{
	UE_LOG(BotSystem, Display, TEXT("<------------------------------------>"));
	UE_LOG(BotSystem, Display, TEXT("---ZoneParams---"));
	UE_LOG(BotSystem, Display, TEXT("Position: Min:%s, Max:%s"), *ZoneParams.Min.ToString(), *ZoneParams.Max.ToString());
	UE_LOG(BotSystem, Display, TEXT("Direction: %s"), *ZoneParams.Direction.ToString());
	UE_LOG(BotSystem, Display, TEXT("Team: %s"), *UEnum::GetValueAsString(ZoneParams.ZoneTeam));
	UE_LOG(BotSystem, Display, TEXT("Role: %s"), *UEnum::GetValueAsString(ZoneParams.ZoneRole));
}

void BotsSystem::SetBotsParams(TArray<FBotParam>& BotsParamsArray, FVector BoxForward)
{
	FBotParam BotParams;
	
	for(int i = 0; i < 6; ++i)
	{
		if (i % 2 == 0)
		{
			BotParams.Color = FColor::Red;
			BotParams.Team = ETeam::Team2;
			BotParams.ViewDirection = -BoxForward;
		}
		else
		{
			BotParams.Color = FColor::Blue;
			BotParams.Team = ETeam::Team1;
			BotParams.ViewDirection = BoxForward;
		}
		BotParams.Role = static_cast<ECharacterPosition>(i/2 + 1);
		BotsParamsArray.Add(BotParams);
	}
	
	for(int i = 0; i < BotsParamsArray.Num(); ++i)
	{
		LogBotsParameters(BotsParamsArray[i]);
	}
}

void BotsSystem::SetRules(EIHMatchRulesMode Rules, ATriggerBox* MatchAreaBox, TArray<FBotParam>& BotsParamsArray, EIHMatchGameMode MatchGameMode, ETeam GoalToTeam)
{
	UE_LOG(BotSystem, Display, TEXT("<------------------------------------>"));
	UE_LOG(BotSystem, Display, TEXT("---SetRules---"));
	UE_LOG(BotSystem, Display, TEXT("MatchRule: %s"), *UEnum::GetValueAsString(Rules));
	
	switch (Rules)
	{
	case EIHMatchRulesMode::None:
		break;
	case EIHMatchRulesMode::FaceOff:
		{
			SetBotsFaceOffPositions(MatchAreaBox, MatchGameMode, BotsParamsArray);
			break;
		}
	case  EIHMatchRulesMode::Goal:
		{
			switch (GoalToTeam)
			{
				case ETeam::Team1:
					UE_LOG(BotSystem, Display, TEXT("MatchRule: Second team scored a goal"));
					SetBotsGoalPositions(MatchAreaBox, MatchGameMode, BotsParamsArray, GoalToTeam);
					break;
				case ETeam::Team2:
					UE_LOG(BotSystem, Display, TEXT("MatchRule: First team scored a goal"));
					SetBotsGoalPositions(MatchAreaBox, MatchGameMode, BotsParamsArray, GoalToTeam);
					break;
				case ETeam::TeamNone:
					UE_LOG(BotSystem, Error, TEXT("MatchRule: The team was not transmitted. FaceOff mode is set"));
					SetBotsFaceOffPositions(MatchAreaBox, MatchGameMode, BotsParamsArray);
					break;
			}
			
			break;
		}
	}
	for(int i = 0; i < BotsParamsArray.Num(); ++i)
	{
		LogBotsParameters(BotsParamsArray[i]);
	}
	UE_LOG(BotSystem, Display, TEXT("<------------------------------------>"));
}

void BotsSystem::SetBotsFaceOffPositions(ATriggerBox* MatchAreaBox, EIHMatchGameMode MatchGameMode, TArray<FBotParam>& BotsParamsArray)
{
	UE_LOG(BotSystem, Display, TEXT("<------------------------------------>"));
	UE_LOG(BotSystem, Display, TEXT("---SetBotsFaceOffParams---"));
	TMap<ECharacterPosition, FVector> FirstTeamParams = GetCenterPoints(MatchAreaBox, ETeam::Team1, MatchGameMode);
	TMap<ECharacterPosition, FVector> SecondTeamParams = GetCenterPoints(MatchAreaBox, ETeam::Team2, MatchGameMode);
	FVector AreaCenter = MatchAreaBox->GetActorLocation();
	
	for (int i = 0; i < BotsParamsArray.Num(); ++i)
	{
		if(BotsParamsArray[i].Team == ETeam::Team1)
		{
			BotsParamsArray[i].Position = FirstTeamParams[BotsParamsArray[i].Role];
			FVector Direction = (AreaCenter - BotsParamsArray[i].Position).GetSafeNormal();
			BotsParamsArray[i].Direction = Direction.ToOrientationQuat();
		}
		else
		{
			BotsParamsArray[i].Position = SecondTeamParams[BotsParamsArray[i].Role];
			FVector Direction = (AreaCenter - BotsParamsArray[i].Position).GetSafeNormal();
			BotsParamsArray[i].Direction = Direction.ToOrientationQuat();
		}
	}
	
	
}

void BotsSystem::SetBotsGoalPositions(ATriggerBox* MatchAreaBox, EIHMatchGameMode MatchGameMode, TArray<FBotParam>& BotsParamsArray, ETeam GoalToTeam)
{
	UE_LOG(BotSystem, Display, TEXT("<------------------------------------>"));
	UE_LOG(BotSystem, Display, TEXT("---SetBotsGoalParams---"));
	FVector AreaCenter = MatchAreaBox->GetActorLocation();
	switch (MatchGameMode)
	{
	case EIHMatchGameMode::MatchGameMode_3x3:
		{
			FVector QuarterPoint = GetRandQuarterCenter(MatchAreaBox, GoalToTeam);
			TArray<FVector> PointsNearQuarterPoint = GetPointsOnCircle(QuarterPoint, true);
			switch (GoalToTeam)
			{
				case ETeam::Team1:
					{
						TMap<ECharacterPosition, FVector> FirstTeamParams = GetCenterPoints(MatchAreaBox, ETeam::Team1, MatchGameMode);
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
								FVector Direction = (AreaCenter - BotsParamsArray[i].Position).GetSafeNormal();
								BotsParamsArray[i].Direction = Direction.ToOrientationQuat();
							}
						}
						break;
					}
				case ETeam::Team2:
						{
							TMap<ECharacterPosition, FVector> SecondTeamParams = GetCenterPoints(MatchAreaBox, ETeam::Team2, MatchGameMode);
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
									FVector Direction = (AreaCenter - BotsParamsArray[i].Position).GetSafeNormal();
									BotsParamsArray[i].Direction = Direction.ToOrientationQuat();
								}
							}
							break;
						}
			}
			break;
		}
	}
}

void BotsSystem::SetBotsDefaultPositions(ATriggerBox* MatchAreaBox, EIHMatchGameMode MatchGameMode, TArray<FBotParam>& BotsParamsArray)
{
	UE_LOG(BotSystem, Display, TEXT("<------------------------------------>"));
	UE_LOG(BotSystem, Display, TEXT("---SetBotsDefaultParams---"));
	FVector BlueLineCenterTeam1 = GetBlueLineCenterPoint(MatchAreaBox, ETeam::Team1);
	FVector BlueLineCenterTeam2 = GetBlueLineCenterPoint(MatchAreaBox, ETeam::Team2);
	
	TArray<FVector> PointsOnBlueLineTeam1 = GetRowPointsFromCenter(BlueLineCenterTeam1, MatchAreaBox, MatchGameMode, 50);
	TArray<FVector> PointsOnBlueLineTeam2 = GetRowPointsFromCenter(BlueLineCenterTeam2, MatchAreaBox, MatchGameMode, 50);
	
	UBoxComponent* Box = Cast<UBoxComponent>(MatchAreaBox->GetCollisionComponent());

	FVector BoxCenter = Box->GetComponentLocation();
	FVector BoxExtent = Box->GetScaledBoxExtent();
	FVector BoxRightVector = Box->GetRightVector();
	FVector LookPoint = BoxCenter + BoxRightVector * BoxExtent;
	
	FVector Direction;
	
	for (int i = 0; i < BotsParamsArray.Num(); ++i)
	{
		switch (BotsParamsArray[i].Team)
		{
			case ETeam::Team1:
				BotsParamsArray[i].Position = PointsOnBlueLineTeam1[i/2];
				Direction = (LookPoint - BotsParamsArray[i].Position).GetSafeNormal();
				BotsParamsArray[i].Direction = Direction.ToOrientationQuat();
				break;
			case ETeam::Team2:
				BotsParamsArray[i].Position = PointsOnBlueLineTeam2[i/2];
				Direction = (LookPoint - BotsParamsArray[i].Position).GetSafeNormal();
				BotsParamsArray[i].Direction = Direction.ToOrientationQuat();
				break;
		}
	}
}

void BotsSystem::SetPuckParams(ATriggerBox* MatchAreaBox, FPuckParam& Puck)
{
	FTransform CenterTransform = MatchAreaBox->GetActorTransform();
	FVector ForwardVector = MatchAreaBox->GetActorForwardVector();
	Puck.Position = CenterTransform.TransformPosition(FVector(0,1,0));
	Puck.Direction = ForwardVector.ToOrientationQuat();
}

void BotsSystem::SetGoalsParams(UBoxComponent* Box, TArray<FGoalParams>& GoalsParams)
{
	FGoalParams GoalParams;
	
	FVector ForwardBoxVector = Box->GetForwardVector();
	FVector BoxCenter = Box->GetComponentLocation();
	FVector BoxExtent = Box->GetScaledBoxExtent();
	FVector PositionTeam1 = BoxCenter - ForwardBoxVector * BoxExtent.X / 1.45f;
	
	GoalParams.Position = PositionTeam1;
	GoalParams.GoalTeam = ETeam::Team1;
	GoalsParams.Add(GoalParams);
	
	FVector PositionTeam2 = BoxCenter + ForwardBoxVector * BoxExtent.X / 1.45f;
	
	GoalParams.Position = PositionTeam2;
	GoalParams.GoalTeam = ETeam::Team2;
	GoalsParams.Add(GoalParams);
	
}

void BotsSystem::SetZones(TArray<FZone>& Zones, UBoxComponent* Box)
{
	FVector BoxExtent = Box->GetScaledBoxExtent();
	
	//Create Zones
	FZone Team1LeftZone;
	FZone Team1RightZone;
	FZone Team1CenterZone;
	
	FZone Team2LeftZone;
	FZone Team2RightZone;
	FZone Team2CenterZone;
	
	//First Team Set Zones
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
	
	//Second Team Set Zones
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
	
	//Add Zones To Array
	Zones.Add(Team1LeftZone);
	Zones.Add(Team1RightZone);
	Zones.Add(Team1CenterZone);
	
	Zones.Add(Team2LeftZone);
	Zones.Add(Team2RightZone);
	Zones.Add(Team2CenterZone);

	for (auto Zone : Zones)
	{
		LogZonesParameters(Zone);
	}
}

void BotsSystem::UpdateBotsDebug(UWorld* World, TArray<FBotParam> BotsParams)
{
	for (int i = 0; i < BotsParams.Num(); ++i)
	{
		if(BotsParams[i].IsControlled)
		{
			DrawDebug(World, BotsParams[i].Position, FColor::Yellow, 50);
			DrawDirectionDebug (World, BotsParams[i], FColor::Yellow, 50);
		}
		else
		{
			DrawDebug(World, BotsParams[i].Position, BotsParams[i].Color, 50);
			DrawDirectionDebug (World, BotsParams[i], BotsParams[i].Color, 50);
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

FVector BotsSystem::GetRandQuarterCenter(ATriggerBox* MatchAreaBox, ETeam GoalToTeam)
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
	FVector FrontOrBack;

	switch (GoalToTeam)
	{
	case ETeam::Team1:
		FrontOrBack = Back;
		break;
	case ETeam::Team2:
		FrontOrBack = Front;
		break;
	}

	//FVector FrontOrBack = bTop ? Front : Back;
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

TMap<ECharacterPosition, FVector> BotsSystem::GetCenterPoints(ATriggerBox* MatchAreaBox, ETeam Team, EIHMatchGameMode MatchGameMode, float CenterPointOffset)
{
	UBoxComponent* Box = Cast<UBoxComponent>(MatchAreaBox->GetCollisionComponent());
	
	FVector BoxCenter = Box->GetComponentLocation();
	FVector BoxExtent = Box->GetScaledBoxExtent();
	FVector BoxForward = Box->GetForwardVector();
	FVector BoxRight = Box->GetRightVector();
	
	FVector RadiusOffset = BoxCenter + BoxForward * BoxExtent.X * 0.18; //0.18 - на эту часть делится половина поля, для нахождения точки, лежащей на окружности центральной зоны
	float Radius = FVector::Distance(BoxCenter, RadiusOffset);
	
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
				Dir1 = (-BoxForward + BoxRight).GetSafeNormal(); //Низ-право
				Dir2 = (-BoxForward - BoxRight).GetSafeNormal(); //Низ-лево

				PointsArray.Add(ECharacterPosition::RightWing, BoxCenter + Dir1 * Radius);
				PointsArray.Add(ECharacterPosition::LeftWing, BoxCenter + Dir2 * Radius);
				PointsArray.Add(ECharacterPosition::Center, BoxCenter - BoxForward * CenterPointOffset);
				break;
			case ETeam::Team2:
				Dir1 = (BoxForward + BoxRight).GetSafeNormal(); //Верх-право
				Dir2 = (BoxForward - BoxRight).GetSafeNormal(); //Верх-лево

				PointsArray.Add(ECharacterPosition::LeftWing, BoxCenter + Dir1 * Radius);
				PointsArray.Add(ECharacterPosition::RightWing, BoxCenter + Dir2 * Radius);
				PointsArray.Add(ECharacterPosition::Center, BoxCenter + BoxForward * CenterPointOffset);
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

FVector BotsSystem::GetBlueLineCenterPoint(ATriggerBox* MatchAreaBox, ETeam Team)
{
	UBoxComponent* Box = Cast<UBoxComponent>(MatchAreaBox->GetCollisionComponent());

	FVector BoxCenter = Box->GetComponentLocation();
	FVector BoxExtent = Box->GetScaledBoxExtent();
	FVector BoxForward = Box->GetForwardVector();
	
	FVector BlueLineCenter;
	
	// Рассматриваем половину арены, при делении на 0.5, получаем 1/4, при делении на 0.25, получаем 1/8, но для нахождения линии, необходимо делить на 0.3, т.е. 1/3 половины поля
	switch (Team)
	{
		case ETeam::Team1:
			BlueLineCenter = (BoxCenter - BoxForward * BoxExtent) * 0.3;
			break;
		case ETeam::Team2:
			BlueLineCenter = (BoxCenter + BoxForward * BoxExtent) * 0.3;
			break;
	}
	
	return BlueLineCenter;
}

TArray<FVector> BotsSystem::GetRowPointsFromCenter(FVector Center, ATriggerBox* MatchAreaBox,
	EIHMatchGameMode MatchGameMode, float Radius)
{
	TArray<FVector> PointsArray;
	FVector RightVector = MatchAreaBox->GetActorRightVector();
	switch (MatchGameMode)
	{
		case EIHMatchGameMode::MatchGameMode_3x3:
			{
				PointsArray.Add(Center);
				PointsArray.Add(Center + RightVector * (Radius * 2));
				PointsArray.Add(Center - RightVector * (Radius * 2));
			}
	}
	return PointsArray;
}