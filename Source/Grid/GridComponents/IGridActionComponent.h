#pragma once

#include "Grid/GridActions/GridActionBase.h"
#include "IGridActionComponent.generated.h"

UINTERFACE(BlueprintType)
class UGridActionComponent : public UInterface
{
	GENERATED_BODY()
};

class IGridActionComponent
{
	GENERATED_BODY()
public:
	virtual void CollectActions(TArray<UGridActionBase*>& OutActions) = 0;
};
