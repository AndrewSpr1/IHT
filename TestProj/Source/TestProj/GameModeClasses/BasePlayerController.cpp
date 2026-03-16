#include "BasePlayerController.h"

#include "BaseGameMode.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "TestProj/Bot'sSystem/Public/BotsSystem.h"

void ABasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("MouseClick", IE_Pressed, this, &ABasePlayerController::MouseClick);
}

void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetCursor();
}

/*void ABasePlayerController::FindMouseClickPosition()
{
	float MouseX, MouseY;
	GetMousePosition(MouseX, MouseY);
	
	FVector CamLoc;
	FRotator CamRot;
	GetPlayerViewPoint(CamLoc, CamRot);
	

	FVector Forward = CamRot.Vector();
	FVector Right = FRotationMatrix(CamRot).GetScaledAxis(EAxis::Y);
	FVector Up = FRotationMatrix(CamRot).GetScaledAxis(EAxis::Z); 

	int32 SizeX, SizeY;
	GetViewportSize(SizeX, SizeY);

	float NDC_X = (MouseX / SizeX) * 2.f - 1.f;
	float NDC_Y = -(MouseY / SizeY) * 2.f + 1.f;

	FVector RayDir =
		Forward +
		Right * NDC_X +
		(Up * (static_cast<float>(SizeY)/static_cast<float>(SizeX))) * NDC_Y;

	RayDir.Normalize();

	FVector PlanePoint = FVector(0, 0, 0);  
	FVector PlaneNormal = FVector::UpVector;

	float check = FVector::DotProduct(PlaneNormal, RayDir);

	UE_LOG(LogTemp, Error, TEXT("DotProduct: %f"), check);

	float Denom = FVector::DotProduct(PlaneNormal, RayDir);

	if (FMath::Abs(Denom) > KINDA_SMALL_NUMBER)
	{
		float T = FVector::DotProduct(
			PlanePoint - CamLoc,
			PlaneNormal
		) / Denom;

		if (T >= 0)
		{
			FVector ClickWorldPos = CamLoc + RayDir * T;

			//BotsSystem::CheckClickDistance(GetWorld(), ClickWorldPos);

			
			float Duration = 2.f;
			float Size = 10.f;

			DrawDebugPoint(
				GetWorld(),
				ClickWorldPos,
				Size,
				FColor::Yellow,
				false,
				Duration,
				0);
		}
	}
}*/

void ABasePlayerController::MouseClick()
{
	ABaseGameMode* GameMode = Cast<ABaseGameMode>(GetWorld()->GetAuthGameMode());
	BotsSystem::FindMouseClickPosition(GetWorld(), this, GameMode);
}

void ABasePlayerController::SetCursor()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
	{
		UE_LOG(LogTemp, Display, TEXT("PlayerController is not find"));
		return;
	}

	PC->SetShowMouseCursor(true);
}