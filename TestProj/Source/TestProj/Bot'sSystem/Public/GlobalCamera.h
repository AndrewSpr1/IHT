#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "GlobalCamera.generated.h"

UCLASS()
class TESTPROJ_API AGlobalCamera : public ACameraActor
{
	GENERATED_BODY()

public:
	AGlobalCamera();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
