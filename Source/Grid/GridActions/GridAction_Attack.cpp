#include "GridAction_Attack.h"
#include "Grid/GridComponents/GridAttackComponent.h"
#include "Grid/GridComponents/GridMovementComponent.h"
#include "../GridUtilityLibrary.h"

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
	MovementComponent = AttackComponent->GetOwner()->FindComponentByClass<UGridMovementComponent>();

	return AttackComponent->IsValidLowLevel();
}

bool UGridAction_Attack::ExecuteAction(UHexGridTile* TargetTile)
{
	return false;
}

void UGridAction_Attack::AbortAction()
{

}

bool UGridAction_Attack::CanExecuteAction(UHexGridTile* CheckTile)
{
	return false;
}

void UGridAction_Attack::UpdateVisualization(UHexGridTile* PotentialTarget)
{

}

void UGridAction_Attack::ActionFinishedCallback()
{

}

TArray<UHexGridTile*> UGridAction_Attack::GetAllAttackableTiles()
{
	TArray<UHexGridTile*> ReachableTiles, AttackableTiles;

	if (MovementComponent)
	{
		MovementComponent->GetAllReachableTiles(ReachableTiles);
	}
	ReachableTiles.AddUnique(MovementComponent->CurrentTile);


	//Iterate through all reachable tiles and get all attackable tiles
	for (UHexGridTile* ReachableTile : ReachableTiles)
	{
		TArray<UHexGridTile*> OpenTiles, ClosedTiles;
		ReachableTile->GetNeighbours(OpenTiles);

		//for (int x = -AttackComponent->MaximumRange; x <= AttackComponent->MaximumRange; x++)
		//{
		//	for (int y = -AttackComponent->MaximumRange; y <= AttackComponent->MaximumRange; y++)
		//	{
		//		FIntVector
		//	}
		//}

		while (OpenTiles.Num() > 0)
		{
			UHexGridTile* current  = OpenTiles[0];
			int32 distance = UGridUtilityLibrary::GetHexDistance(current->Coords, ReachableTile->Coords);

			//Add the tile to attackable tiles, if its within the attack range
			if (distance <= AttackComponent->MaximumRange)
			{
				if(distance >= AttackComponent->MinimumRange)
					AttackableTiles.AddUnique(current);
				//Tile has been processed, move it from open to closed set
				OpenTiles.Remove(current);
				ClosedTiles.Add(current);
			}

			//If we're not at the maximum distance yet, add all unknown neighbours to the open tiles
			if (distance < AttackComponent->MaximumRange)
			{
				TArray<UHexGridTile*> DirectNeighbours;
				current->GetNeighbours(DirectNeighbours);

				for (UHexGridTile* directNeighbour : DirectNeighbours)
				{
					//Only add it if the neighbour is not known
					if (!ClosedTiles.Contains(directNeighbour) && !OpenTiles.Contains(directNeighbour))
					{
						OpenTiles.Add(directNeighbour);
					}
				}
			}
		}
	}

	return AttackableTiles;
}

void UGridAction_Attack::ShowHoveredVisualization(UHexGridTile* PotentialTarget)
{
	CachedVisualizedTiles = GetAllAttackableTiles();

	for (UHexGridTile* tile : CachedVisualizedTiles)
	{
		tile->SetTileState(this, ShowAsAttackable, ETileStateLayers::HoverAttack);
	}
}

void UGridAction_Attack::HideHoveredVisualization()
{
	for (UHexGridTile* tile : CachedVisualizedTiles)
	{
		tile->ResetTileState(this);
	}
}



#undef LOCTEXT_NAMESPACE