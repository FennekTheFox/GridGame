#include "GridAttackComponent.h"
#include "Grid/GridActions/GridAction_Attack.h"



UGridAttackComponent::UGridAttackComponent()
{

}

void UGridAttackComponent::CollectActions(TArray<UGridActionBase*>& OutActions)
{
	UGridActionBase* Action = NewObject<UGridAction_Attack>(this, TEXT("Attack Action"));

	OutActions.Add(Action);
}

