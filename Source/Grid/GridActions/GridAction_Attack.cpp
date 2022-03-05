#include "GridAction_Attack.h"
#include "Grid/GridComponents/GridAttackComponent.h"
#include "Grid/GridComponents/GridMovementComponent.h"

#define LOCTEXT_NAMESPACE "ActionBase"

UGridAction_Attack::UGridAction_Attack()
{
	ActionName = LOCTEXT("GridAction_Attack", "Attack");

	UE_LOG(LogTemp, Log, TEXT("Foo"))
}

UGridAction_Attack::~UGridAction_Attack()
{

	UE_LOG(LogTemp, Log, TEXT("Bar"))
}

bool UGridAction_Attack::InitiateAction(UHexGridTile* InitTile)
{
	AttackComponent = Cast<UGridAttackComponent>(GetOuter());
	ensure(AttackComponent);

	return AttackComponent->IsValidLowLevel();
}

bool UGridAction_Attack::ExecuteAction(UHexGridTile* TargetTile)
{
	ensureMsgf(AttackComponent, TEXT("Trying to execute action Attack, but ActionComponent was null. Was action aborted but not cleaned up?"));


	if (AttackComponent)
	{
		OnActionStarted.Broadcast(this);
		AttackComponent->AttackTile(TargetTile);
		AttackComponent->OnComplete.AddDynamic(this, &UGridAction_Attack::ActionFinishedCallback);
		return true;
	}
	return false;
}

void UGridAction_Attack::AbortAction()
{
	AttackComponent = nullptr;
}

bool UGridAction_Attack::CanExecuteAction(UHexGridTile* CheckTile)
{
	ensureMsgf(AttackComponent, TEXT("Trying to check if action Attack can be executed, but AttackComponent was null. Was action aborted but not cleaned up?"));

	//Check if theres a valid target on the target tile
	AActor* OccupyingUnit = CheckTile->GetOccupyingUnit();
	AActor* Owner = AttackComponent->GetOwner();

	if (!OccupyingUnit || OccupyingUnit == Owner) return false;

	if (AttackComponent)
	{
		return AttackComponent->CanAttackTile(CheckTile);
	}

	return false;
}

void UGridAction_Attack::UpdateVisualization(UHexGridTile* PotentialTarget)
{
	if (AttackComponent)
	{
		//Clean up vizualization
		for (UHexGridTile* tile : VisualizedPath)
		{
			tile->ResetTileState(this);
		}
		if (VisualizedTarget)
		{
			VisualizedTarget->ResetTileState(this);
			VisualizedTarget = nullptr;
		}

		//Visualise Attack only if its not the location of the owner
		if (PotentialTarget->GetOccupyingUnit() == AttackComponent->GetOwner()) return;

		if (AttackComponent->GetPathToClosestTileToAttackFrom(PotentialTarget, VisualizedPath))
		{
			for (UHexGridTile* tile : VisualizedPath)
			{
				tile->SetTileState(this, ShowAsPath, ETileStateLayers::ShowPath);
			}
		}

		//Visualise the target tile based on its executability
		if (CanExecuteAction(PotentialTarget))
		{
			PotentialTarget->SetTileState(this, ShowAsAttackTarget_Valid, ETileStateLayers::ShowPath);
		}
		else
		{
			PotentialTarget->SetTileState(this, ShowAsAttackTarget_Invalid, ETileStateLayers::ShowPath);
		}
		VisualizedTarget = PotentialTarget;
	}
}

void UGridAction_Attack::ActionFinishedCallback()
{
	for (UHexGridTile* Tile : VisualizedPath)
	{
		Tile->ResetTileState(this);
	}

	if(VisualizedTarget)
		VisualizedTarget->ResetTileState(this);

	OnActionFinished.Broadcast(this);
}



void UGridAction_Attack::ShowHoveredVisualization(UHexGridTile* PotentialTarget)
{
	ensure(AttackComponent);

	if (AttackComponent)
	{
		AttackComponent->SetShowAttackableArea(true);
	}
}

void UGridAction_Attack::HideHoveredVisualization()
{
	ensure(AttackComponent);

	if (AttackComponent)
	{
		AttackComponent->SetShowAttackableArea(false);
	}
}



#undef LOCTEXT_NAMESPACE