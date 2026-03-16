


#include "BasePawn.h"



ABasePawn::ABasePawn()
{

	PrimaryActorTick.bCanEverTick = true;
}

void ABasePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

