#pragma once


#include "PuckParam.generated.h"

USTRUCT(BlueprintType)
struct FPuckParam
{
	GENERATED_BODY()
	
	UPROPERTY(Category=PuckParameters, EditAnywhere, BlueprintReadWrite);
	FVector Position = FVector::ZeroVector;

	UPROPERTY(Category=PuckParameters, EditAnywhere, BlueprintReadWrite);
	bool IsPuckInGame = false;

	UPROPERTY(Category=PuckParameters, EditAnywhere, BlueprintReadWrite);
	bool IsControlled = false;
};