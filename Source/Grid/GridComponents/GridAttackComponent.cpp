#include "GridAttackComponent.h"
#include "Grid/GridActions/GridAction_Attack.h"
#include "Grid/GridActor.h"
#include "GridMovementComponent.h"
#include "../GridUtilityLibrary.h"
#include <limits>


UGridAttackComponent::UGridAttackComponent()
{

}

void UGridAttackComponent::CollectActions(TArray<UGridActionBase*>& OutActions)
{
	UGridActionBase* Action = NewObject<UGridAction_Attack>(this, TEXT("Attack Action"));

	OutActions.Add(Action);
}

void UGridAttackComponent::SetShowAttackableArea(bool bShow)
{
	if (bShow)
	{
		SetShowAttackableArea(false);
		GetAllAttackableTiles(ShownAttackableTiles);

		for (UHexGridTile* Tile : ShownAttackableTiles)
		{
			Tile->SetTileState(this, ETileState::ShowAsAttackable, ETileStateLayers::HoverAttack);
		}
	}
	else
	{
		for (UHexGridTile* Tile : ShownAttackableTiles)
		{
			Tile->ResetTileState(this);
		}
		ShownAttackableTiles.Reset();
	}
}

void UGridAttackComponent::AttackTile(UHexGridTile* TargetTile)
{
	ensureMsgf(CanAttackTile(TargetTile), TEXT("Tried to attack a tile that cant be attacked!"));

	if (MovementComponent)
	{
		MovementComponent->MoveToTile(GetClosestTileToAttackFrom(TargetTile));
		MovementComponent->OnComplete.AddDynamic(this, &UGridAttackComponent::OnPreAttackMovementComplete);
	}
	else
	{
		OnPreAttackMovementComplete();
	}
}

bool UGridAttackComponent::CanAttackTile(UHexGridTile* TargetTile)
{
	TArray<UHexGridTile*> AttackableTiles;
	
	GetAllAttackableTiles(AttackableTiles);

	return AttackableTiles.Contains(TargetTile);
}

void UGridAttackComponent::OnPreAttackMovementComplete()
{
	UE_LOG(LogTemp, Log, TEXT("moved into attack position"))

	OnComplete.Broadcast();
}

void UGridAttackComponent::GetAllAttackableTiles(TArray<UHexGridTile*>& Result)
{
	Result.Reset();

	TArray<UHexGridTile*> ReachableTiles;

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

		while (OpenTiles.Num() > 0)
		{
			UHexGridTile* current = OpenTiles[0];
			int32 distance = UGridUtilityLibrary::GetHexDistance_FromTiles(current, ReachableTile);

			//Add the tile to attackable tiles, if its within the attack range
			if (distance <= MaximumRange)
			{
				if (distance >= MinimumRange)
					Result.AddUnique(current);
				//Tile has been processed, move it from open to closed set
				OpenTiles.Remove(current);
				ClosedTiles.Add(current);
			}

			//If we're not at the maximum distance yet, add all unknown neighbours to the open tiles
			if (distance < MaximumRange)
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
}



bool UGridAttackComponent::GetPathToClosestTileToAttackFrom(UHexGridTile* Target, TArray<UHexGridTile*>& Result)
{
	Result.Reset();

	if (!MovementComponent) 
	{
		return CanAttackTile(Target);
	}
	else
	{
		if (CanAttackTile(Target))
		{
			TArray<UHexGridTile*> MovmementTargetTiles;
			GetTileToAttackFromSortedByDistance(Target, MovmementTargetTiles);

			for (UHexGridTile* tile : MovmementTargetTiles)
			{
				if (MovementComponent->GetPathTo(tile, Result))
				{
					return true;
				}
			}
		}
	}	

	Result.Reset();
	return false;
}

UHexGridTile* UGridAttackComponent::GetClosestTileToAttackFrom(UHexGridTile* Target)
{
	ensureMsgf(MovementComponent, TEXT("Trying to get closest tile to attack from on an actor that cant move on the grid. Doesnt work like that"));
	if (!MovementComponent) return nullptr;

	TArray<UHexGridTile*> PotentialTiles;
	GetAllTilesToAttackFrom(Target, PotentialTiles);


	UHexGridTile* ClosestTile = nullptr;
	float distance = std::numeric_limits<float>::max();
	float temp;

	for (UHexGridTile* tile : PotentialTiles)
	{
		temp = UGridUtilityLibrary::GetHexDistance_FromTiles(tile, MovementComponent->CurrentTile);

		if (temp < distance)
		{
			ClosestTile = tile;
			distance = temp;
		}
	}

	return ClosestTile;
}

void UGridAttackComponent::GetTileToAttackFromSortedByDistance(UHexGridTile* Target, TArray<UHexGridTile*>& OutArray)
{
	ensureMsgf(MovementComponent, TEXT("Trying to get closest tile to attack from on an actor that cant move on the grid. Doesnt work like that"));
	if (!MovementComponent) return;

	GetAllTilesToAttackFrom(Target, OutArray);
	Algo::Sort(OutArray, [=]( UHexGridTile* A, UHexGridTile* B)
	{
		float toA = UGridUtilityLibrary::GetHexDistance_FromTiles(MovementComponent->CurrentTile, A);
		float toB = UGridUtilityLibrary::GetHexDistance_FromTiles(MovementComponent->CurrentTile, B);
		return toA < toB;
	});
}

void UGridAttackComponent::GetAllTilesToAttackFrom(UHexGridTile* Target, TArray<UHexGridTile*>& Result)
{
	Result.Reset();

	if (!Target) return;


	TArray<UHexGridTile*> OpenTiles, ClosedTiles;
	Target->GetNeighbours(OpenTiles);

	while (OpenTiles.Num() > 0)
	{
		UHexGridTile* current = OpenTiles[0];
		int32 distance = UGridUtilityLibrary::GetHexDistance_FromTiles(current, Target);

		//Add the tile to attackable tiles, if its within the attack range
		if (distance <= MaximumRange)
		{
			if (distance >= MinimumRange)
				Result.AddUnique(current);
			//Tile has been processed, move it from open to closed set
			OpenTiles.Remove(current);
			ClosedTiles.Add(current);
		}

		//If we're not at the maximum distance yet, add all unknown neighbours to the open tiles
		if (distance < MaximumRange)
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

void UGridAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	MovementComponent = GetOwner()->FindComponentByClass<UGridMovementComponent>();
}
