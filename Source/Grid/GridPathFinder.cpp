#include "GridPathFinder.h"

#include "GridActor.h"
#include "GridUtilityLibrary.h"
#include "GridComponents/GridMovementComponent.h"


bool FAStarHelper::Step()
{
	if (OpenSet.Num() == 0)
		return false;

	UHexGridTile* Current = nullptr;


	OpenSet.HeapPop(Current, Comparer);

	if (Current->Equals(Goal))
	{
		Success = true;
		return true;
	}

	CloseSet.Add(Current);

	TArray<UHexGridTile*> Neighbours;
	Current->GetNeighbours(Neighbours);

	for (int i = 0; i < Neighbours.Num(); i++)
	{
		UHexGridTile* UndiscoveredNeighbour = Neighbours[i];

		if (CloseSet.Contains(UndiscoveredNeighbour)) continue;
		//If we cant pass through the tile, dont consider it a valid tile for a path
		if (!Agent->CanPassThroughTile(UndiscoveredNeighbour)) continue;

		int NewCost = GCost[Current] + DistanceHeuristic(Current, UndiscoveredNeighbour);

		if (!GCost.Contains(UndiscoveredNeighbour) || NewCost < GCost[UndiscoveredNeighbour])
		{
			GCost.FindOrAdd(UndiscoveredNeighbour, NewCost);
			FCost.FindOrAdd(UndiscoveredNeighbour, NewCost + DistanceHeuristic(UndiscoveredNeighbour, Goal));

			if (!OpenSet.Contains(UndiscoveredNeighbour))
			{
				OpenSet.HeapPush(UndiscoveredNeighbour, Comparer);
			}

			CameFrom.FindOrAdd(UndiscoveredNeighbour, Current);
		}
	}

	return true;
}




void FAStarHelper::CollectPath(TArray<UHexGridTile*>& Result) const
{
	UHexGridTile* Current = Goal;

	Result.AddUnique(Current);

	while (Current != Start)
	{
		Current = CameFrom.FindChecked(Current);
		Result.Add(Current);
	}
}

int32 FAStarHelper::DistanceHeuristic(UHexGridTile* Probe, UHexGridTile* Target)
{
	//int32 MovementDistance = UGridUtilityLibrary::GetHexDistance(Probe->Coords, Target->Coords);

	return (Probe->Location - Target->Location).Size();
}








UGridPathFinderAgent::UGridPathFinderAgent()
{
	GridActor = nullptr;
}

UGridPathFinderAgent::~UGridPathFinderAgent()
{

}

UHexGridTile* UGridPathFinderAgent::GetStart() const
{
	return Request.Start;
}

UHexGridTile* UGridPathFinderAgent::GetDestination() const
{
	return Request.Goal;
}

AActor* UGridPathFinderAgent::GetSender() const
{
	return Request.Sender;
}

AGridActor* UGridPathFinderAgent::GetGrid() const
{
	return GridActor;
}

const FGameplayTagContainer& UGridPathFinderAgent::GetExtraTags() const
{
	return Request.ExtraTags;
}

bool UGridPathFinderAgent::IsReachable_Implementation(const FGridPathFinderRequest InRequest)
{
	TArray<UHexGridTile*> trash;
	return FindPath(InRequest, trash);
}

int32 UGridPathFinderAgent::GetCost_Implementation(UHexGridTile* From, UHexGridTile* To)
{
	return UGridUtilityLibrary::GetHexDistance_FromTiles(From, To);
};


int32 UGridPathFinderAgent::Heuristic_Implementation(UHexGridTile* From, UHexGridTile* To)
{
	return GetCost(From, To);
}

bool UGridPathFinderAgent::CanPassThroughTile_Implementation(UHexGridTile* Tile)
{
	return true;
}

bool UGridPathFinderAgent::CanStandOnTile_Implementation(UHexGridTile* Tile)
{
	return true;
}

bool UGridPathFinderAgent::FindPath(const FGridPathFinderRequest InRequest, TArray<UHexGridTile*>& Path)
{
	Request = InRequest;

	if (!Request.IsValid() || !GridActor || !CanStandOnTile_Implementation(Request.Goal))
	{
		return false;
	}

	Path.Reset();
	bool Success = false;

	FAStarHelper AStarHelper(Request.Start, Request.Goal, GridActor, this);

	int32 Step = 0;
	while (!Success)
	{
		if (!AStarHelper.Step())
			break;

		Success = AStarHelper.Success;

		if (++Step > Request.MaxSearchStep)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to find path within MaxFindStep"));
			break;
		}
	}

	if (Success)
	{
		AStarHelper.CollectPath(Path);

		if (Request.bRemoveDest)
		{
			Path.Pop();
		}

		if (Request.MaxCost >= 0)
		{
			int32 Cost = 0;
			int32 i;

			for (i = 1; i < Path.Num(); i++)
			{
				Cost += GetCost(Path[i - 1], Path[i]);
				if (Cost > Request.MaxCost)
					break;
			}

			if (i < Path.Num())
			{
				Path.RemoveAt(i, Path.Num() - i);
				Success = false;
			}
		}
	}

	return Success;
}

void UGridPathFinderAgent::GetReachableTiles(const FGridPathFinderRequest InRequest, TArray<UHexGridTile*>& Result)
{
	Request = InRequest;

	Result.Reset();
	Result.Add(Request.Start);
	
	for (int i = 0; i < Result.Num(); i++)
	{
		TArray<UHexGridTile*> Neighbours;
		Result[i]->GetNeighbours(Neighbours);

		for (UHexGridTile* Neighbour : Neighbours)
		{
			FGridPathFinderRequest tempRequest = InRequest;
			tempRequest.Goal = Neighbour;

			if (IsReachable_Implementation(tempRequest))
			{
				Result.AddUnique(Neighbour);
			}
		}
	}
}

FGridPathFinderRequest::FGridPathFinderRequest()
{
	bRemoveDest = false;
	MaxCost = -1;
	MaxSearchStep = 1000;
	Start = Goal = nullptr;
}

bool FGridPathFinderRequest::IsValid()
{
	return (Start && Goal);
}

bool UGridMovementAgent::CanPassThroughTile_Implementation(UHexGridTile* Tile)
{
	return GMC->CanPassTile(Tile);
}

bool UGridMovementAgent::CanStandOnTile_Implementation(UHexGridTile* Tile)
{
	return (Tile->GetOccupyingUnit() == nullptr || Tile->GetOccupyingUnit() == Request.Sender);
}
