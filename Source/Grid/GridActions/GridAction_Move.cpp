#include "GridAction_Move.h"

#include "Grid/GridComponents/GridMovementComponent.h"



bool UGridAction_Move::InitiateAction(UHexGridTile* InitTile)
{
	ensure(InitTile);

	AActor* TargetActor = InitTile->GetOccupyingUnit();

	if (TargetActor)
	{
		MovementComponent = TargetActor->FindComponentByClass<UGridMovementComponent>();

		if (MovementComponent)
			return true;
	}
	return false;
}

bool UGridAction_Move::ExecuteAction(UHexGridTile* TargetTile)
{
	ensureMsgf(MovementComponent, TEXT("Trying to execute action Move but action movement was not initiated. Was action aborted but not discarded?"));

	if (MovementComponent)
	{
		OnActionStarted.Broadcast(this);
		MovementComponent->MoveToTile(TargetTile);
		MovementComponent->OnComplete.AddDynamic(this, &UGridAction_Move::ActionFinishedCallback);
		return true;
	}

	return false;
}

void UGridAction_Move::AbortAction()
{
	MovementComponent = nullptr;
}

bool UGridAction_Move::CanExecuteAction(UHexGridTile* TargetTile)
{
	ensureMsgf(MovementComponent, TEXT("Trying to check if action Move can be executed, but action movement was not initiated. Was action aborted but not discarded?"));

	if (MovementComponent)
	{
		TArray<UHexGridTile*> trash;
		return MovementComponent->CanMoveToTile(TargetTile, trash);
	}

	return false;
}

void UGridAction_Move::UpdateVisualization(UHexGridTile* PotentialTarget)
{
	TArray<UHexGridTile*> PotentialPath;

	if (MovementComponent)
	{
		//Reset the tile state of indicated path tiles
		for (UHexGridTile* Tile : CachedPath)
		{
			Tile->ResetTileState(this);
		}

		//If a path to the new potential target has been found, indicate the path
		if (MovementComponent->CanMoveToTile(PotentialTarget, PotentialPath))
		{
			for (UHexGridTile* Tile : PotentialPath)
			{
				Tile->SetTileState(this, ETileState::ShowAsPath, ETileStateLayers::ShowPath);
			}
		}
		else
		{
			PotentialTarget->SetTileState(this, ETileState::ShowAsInvalid, ETileStateLayers::ShowPath);
			CachedPath.Add(PotentialTarget);
		}
		CachedPath = PotentialPath;
	}
}

void UGridAction_Move::ActionFinishedCallback()
{
	//Reset the tile state of indicated path tiles
	for (UHexGridTile* Tile : CachedPath)
	{
		Tile->ResetTileState(this);
	}

	OnActionFinished.Broadcast(this);
	MovementComponent->OnComplete.RemoveAll(this);
}

void UGridAction_Move::ShowHoveredVisualization(UHexGridTile* PotentialTarget)
{
	ensure(MovementComponent);

	if (MovementComponent)
	{
		MovementComponent->SetShowMovableArea(true);
	}
}

void UGridAction_Move::HideHoveredVisualization()
{
	ensure(MovementComponent);

	if (MovementComponent)
	{
		MovementComponent->SetShowMovableArea(false);
	}
}
