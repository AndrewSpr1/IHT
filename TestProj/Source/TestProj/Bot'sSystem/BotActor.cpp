


#include "BotActor.h"

#include "DrawDebugHelpers.h"
#include "Misc/MapErrors.h"


ABotActor::ABotActor()
{
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	RootComponent = Camera;
}


void ABotActor::BeginPlay()
{
	Super::BeginPlay();

	SetPlayerDefaultPositions(PlayerPosStruct);

}


void ABotActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void ABotActor::SetPlayerDefaultPositions(FPlayerPosStruct& PlayerPositions)
{
	PlayerPositions.LeftPlayerCurrentPos.X = GetActorLocation().X + 200;// + PlayerPositions.LeftPlayerOffset.X;
	PlayerPositions.LeftPlayerCurrentPos.Y = GetActorLocation().Y - 200;// + PlayerPositions.LeftPlayerOffset.Y;
	
	PlayerPositions.CenterPlayerCurrentPos.X = GetActorLocation().X + PlayerPositions.CenterPlayerOffset.X;
	PlayerPositions.CenterPlayerCurrentPos.Y = GetActorLocation().Y + PlayerPositions.CenterPlayerOffset.Y;
	
	PlayerPositions.RightPlayerCurrentPos.X = GetActorLocation().X + PlayerPositions.RightPlayerOffset.X;
	PlayerPositions.RightPlayerCurrentPos.Y = GetActorLocation().Y + PlayerPositions.RightPlayerOffset.Y;
}

