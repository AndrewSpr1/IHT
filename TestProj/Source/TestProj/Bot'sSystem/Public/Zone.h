#pragma once
#include "Role.h"
#include "Team.h"
#include "Components/BoxComponent.h"

#include "Zone.generated.h"


USTRUCT(BlueprintType)
struct FZone
{
	GENERATED_BODY()
	
	FVector Min;
	FVector Max;
	
	FQuat Direction = FVector(0, 0, 0).ToOrientationQuat();
	
	ECharacterPosition ZoneRole;
	ETeam ZoneTeam;
	
	bool IsInside(const FVector Pos, UBoxComponent* Box);
	
	FVector ClampToZone(FVector Pos);
	
	FVector GetExtent();
	
	FVector GetCenter();
};