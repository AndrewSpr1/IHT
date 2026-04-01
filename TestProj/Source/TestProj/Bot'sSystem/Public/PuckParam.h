#pragma once


#include "PuckParam.generated.h"

USTRUCT(BlueprintType)
struct FPuckParam
{
	GENERATED_BODY()
	
	UPROPERTY(Category=PuckParameters, EditAnywhere, BlueprintReadWrite);
	FVector Position = FVector::ZeroVector;
	
	UPROPERTY()
	FQuat Direction = FVector(0,0,0).ToOrientationQuat();
	
	FVector Velocity = FVector(0,0,0);

	UPROPERTY(Category=PuckParameters, EditAnywhere, BlueprintReadWrite);
	bool IsPuckInGame = false;

	UPROPERTY(Category=PuckParameters, EditAnywhere, BlueprintReadWrite);
	bool IsControlled = false;
};