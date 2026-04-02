

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "TestProj/TestProjGameModeBase.h"
#include "TestProj/Bot'sSystem/Public/GameModeParams.h"
#include "BaseGameMode.generated.h"


/**
 * 
 */
UCLASS()
class TESTPROJ_API ABaseGameMode : public ATestProjGameModeBase
{
	GENERATED_BODY()

	ABaseGameMode();

	UPROPERTY()
	APlayerController* PC;
	UPROPERTY()
	UBoxComponent* Box;
	
	UPROPERTY()
	TArray<UBoxComponent*> GoalsBoxes;

public:

	UPROPERTY(Category=BotSystem, EditAnywhere, BlueprintReadWrite)
	FGameModeParams GameModeParams;
	
	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;
};
