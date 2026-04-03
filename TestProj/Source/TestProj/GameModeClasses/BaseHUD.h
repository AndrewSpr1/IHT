

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TestProj/UI/HUDWidget.h"
#include "BaseHUD.generated.h"

/**
 * 
 */
UCLASS()
class TESTPROJ_API ABaseHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	
private:
	TSharedPtr<SHUDWidget> HUDWidget;
};
