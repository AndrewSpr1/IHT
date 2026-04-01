#pragma once
#include "Role.h"
#include "Team.h"

#include "BotParam.generated.h"

USTRUCT(BlueprintType)
struct FBotParam
{
	GENERATED_BODY()

	UPROPERTY(Category=BotParameters, EditAnywhere, BlueprintReadWrite);
	FVector Position = FVector::ZeroVector;
	
	UPROPERTY(Category=BotParameters, EditAnywhere, BlueprintReadWrite);
	FQuat Direction = FQuat(FVector::ZeroVector, FMath::DegreesToRadians(0));
	
	UPROPERTY(Category=BotParameters, EditAnywhere, BlueprintReadWrite);
	bool IsControlled = false;
	
	UPROPERTY(Category=BotParameters, EditAnywhere, BlueprintReadWrite);
	FColor Color = FColor::Black;
	
	UPROPERTY(Category=BotParameters, EditAnywhere, BlueprintReadWrite);
	ETeam Team = ETeam::TeamNone;
	
	UPROPERTY(Category=BotParameters, EditAnywhere, BlueprintReadWrite);
	ECharacterPosition Role = ECharacterPosition::Any;
	
	UPROPERTY()
	FVector ViewDirection = FVector::ZeroVector;
	
	
	bool operator== (const FBotParam& BotParam) const
	{
		return Position == BotParam.Position && Direction == BotParam.Direction && Team == BotParam.Team && Role == BotParam.Role;
	}
	
	bool operator!= (const FBotParam& BotParam) const
	{
		return Position != BotParam.Position || Direction != BotParam.Direction || Team != BotParam.Team || Role != BotParam.Role;
	}
};
