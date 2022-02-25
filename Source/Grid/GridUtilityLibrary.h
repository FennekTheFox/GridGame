// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GridUtilityLibrary.generated.h"

/**
 * 
 */
UCLASS()
class GRID_API UGridUtilityLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Grid Utility")
		static int GetHexDistance(FIntVector A, FIntVector B);
	
};
