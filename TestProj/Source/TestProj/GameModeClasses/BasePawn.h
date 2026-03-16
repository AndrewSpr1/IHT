#pragma once

#include "CoreMinimal.h"
#include <GameFramework/Pawn.h>
#include "BasePawn.generated.h"

UCLASS()
class TESTPROJ_API ABasePawn : public APawn
{
	GENERATED_BODY()

public:
	ABasePawn();
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
