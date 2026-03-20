#include "TestProj/Bot'sSystem/Public/Zone.h"


bool FZone::IsInside(const FVector Pos, UBoxComponent* Box)
{
	FTransform BoxTransform = Box->GetComponentTransform();
	FVector LocalPos = BoxTransform.InverseTransformPosition(Pos);
	
	return LocalPos.X >= Min.X && LocalPos.X <= Max.X &&
		   LocalPos.Y >= Min.Y && LocalPos.Y <= Max.Y;
}

FVector FZone::ClampToZone(FVector Pos)
{
	FVector ClosestPoint = Pos;
	if (Pos.X < Min.X)
	{
		ClosestPoint.X = Min.X;
	}
	else if (Pos.X > Max.X)
	{
		ClosestPoint.X = Max.X;
	}
	
	if (Pos.Y < Min.Y)
	{
		ClosestPoint.Y = Min.Y;
	}
	else if (Pos.Y > Max.Y)
	{
		ClosestPoint.Y = Max.Y;
	}
	ClosestPoint.Z = 0;
	
	return ClosestPoint;
}

FVector FZone::GetExtent()
{
	return 0.5 * (Max - Min);
}

FVector FZone::GetCenter()
{
	return FVector((Min + Max) * 0.5);
}
