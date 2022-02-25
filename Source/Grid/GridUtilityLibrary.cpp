// Fill out your copyright notice in the Description page of Project Settings.


#include "GridUtilityLibrary.h"

int UGridUtilityLibrary::GetHexDistance(FIntVector A, FIntVector B)
{
	FIntVector D = B - A;
	if (FMath::Sign(D.X) == FMath::Sign(D.Y))
	{
		return FMath::Abs(D.X + D.Y);
	}
	else
	{
		return FMath::Max(FMath::Abs(D.X), FMath::Abs(D.Y));
	}
}

