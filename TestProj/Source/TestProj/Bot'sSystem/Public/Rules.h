#pragma once

UENUM(BlueprintType)
enum class EIHMatchRulesMode : uint8
{
	None,
	StartMatch,
	PreparationGame,
	Game,
	Goal,
	PuckKillZone,
	FaceOff,
	Offside,
	IcingThePuck,
	OverTime,
	EndMatch
};