

#pragma once

#include "CoreMinimal.h"
#include <GameFramework/PlayerController.h>
#include "BasePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TESTPROJ_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()

	UPROPERTY()
	AActor* InteractActor = nullptr;

protected:
	virtual void SetupInputComponent() override;

	virtual void BeginPlay() override;

	//void FindMouseClickPosition();

	void MouseClick();
	
	void SetCursor();
};
