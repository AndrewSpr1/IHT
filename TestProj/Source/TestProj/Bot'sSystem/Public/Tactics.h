#pragma once

UENUM(BlueprintType)
enum class ETeamTactic : uint8
{
	None         UMETA(DisplayName = "None"),
	Attack       UMETA(DisplayName = "Attack"),
	Defence      UMETA(DisplayName = "Defence"),
};