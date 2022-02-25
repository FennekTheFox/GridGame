#pragma once

#include "IGridInteractable.generated.h"

UINTERFACE(BlueprintType)
class UGridInteractable : public UInterface
{
	GENERATED_BODY()
};

class IGridInteractable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Grid Interaction")
		TArray<UGridActionBase*> GetActions();
};
