

#pragma once

#include "CoreMinimal.h"
#include <GameFramework/PlayerController.h>
#include "BaseGameMode.h"
#include "BasePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TESTPROJ_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()
	
	

protected:
	
	UPROPERTY()
	ABaseGameMode* BaseGameMode;
	
	virtual void SetupInputComponent() override;

	virtual void BeginPlay() override;
	
	void MousePressed();
	void MouseRelease();
	
	void SetCursor();
};
