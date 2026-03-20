#pragma once

#include "Team.h"
#include "GoalParams.generated.h"

USTRUCT(BlueprintType)
struct FGoalParams
{
    GENERATED_BODY()
    
    UPROPERTY(Category=GoalParameters, EditAnywhere, BlueprintReadWrite)
    FVector Position;
    
    UPROPERTY(Category=GoalParameters, EditAnywhere, BlueprintReadWrite)
    ETeam GoalTeam;
};