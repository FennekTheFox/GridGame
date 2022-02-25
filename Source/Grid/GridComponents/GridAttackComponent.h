#pragma once

#include "IGridActionComponent.h"
#include "GridAttackComponent.generated.h"

UCLASS(ClassGroup = (Grid), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class UGridAttackComponent : public UActorComponent, public IGridActionComponent
{
	GENERATED_BODY()

public:
	UGridAttackComponent();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Grid Attack Properties")
		int32 MinimumRange = 1;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Grid Attack Properties")
		int32 MaximumRange = 1;

	//BEGIN IGridActionComponent
	void CollectActions(TArray<UGridActionBase*>& OutActions) override;
	//END IGridActionComponent
};