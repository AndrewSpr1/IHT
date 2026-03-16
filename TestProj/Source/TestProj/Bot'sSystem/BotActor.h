#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Actor.h"
#include "BotActor.generated.h"

USTRUCT(BlueprintType)
struct FPlayerPosStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector LeftPlayerOffset = FVector::ZeroVector;
	
	UPROPERTY()
	FVector LeftPlayerCurrentPos = FVector::ZeroVector;

	UPROPERTY()
	bool LeftPlayerActive = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector CenterPlayerOffset = FVector::ZeroVector;

	UPROPERTY()
	FVector CenterPlayerCurrentPos = FVector::ZeroVector;

	UPROPERTY()
	bool CenterPlayerActive = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector RightPlayerOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector RightPlayerCurrentPos = FVector::ZeroVector;

	UPROPERTY()
	bool RightPlayerActive = false;
};

UCLASS()
class TESTPROJ_API ABotActor : public AActor
{
	GENERATED_BODY()

public:

	ABotActor();

	UPROPERTY(EditAnywhere, Category= "Camera")
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerPosition")
	FPlayerPosStruct PlayerPosStruct;

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void SetPlayerDefaultPositions(FPlayerPosStruct& PlayerPositions);
};
