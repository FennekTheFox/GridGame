#include "GridMovementComponent.h"

#include "Grid/GridPathFinder.h"
#include "Kismet/GameplayStatics.h"
#include "Grid/GridActions/GridAction_Move.h"


UGridMovementComponent::UGridMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UGridMovementComponent::AttachToGrid(AGridActor* NewGrid)
{
	if (NewGrid)
	{	
		//If a grid has been passed, attach to it. If NewGrid is a nullptr, look for a grid
		Grid = NewGrid;
	}
	else
	{
		Grid = Cast<AGridActor>(UGameplayStatics::GetActorOfClass(this, AGridActor::StaticClass()));
	}

	//Initialise the path finders reference to the grid
	PathFinder->GridActor = Grid;

	//Snap the actor to the closest tile
	CurrentTile = Grid->GetTileClosestToCoordinates(GetOwner()->GetActorLocation(), false);
	CurrentTile->OccupyTile(GetOwner());
	GetOwner()->SetActorLocation(CurrentTile->Location);
	CurrentTileLoc = GetOwner()->GetActorLocation();
}

bool UGridMovementComponent::MoveToTile(UHexGridTile* TargetTile)
{
	ensure(PathFinder);
	ensure(TargetTile);

	TArray<UHexGridTile*>Path;

	FGridPathFinderRequest Request;
	Request.Sender = GetOwner();
	Request.Start = CurrentTile;
	Request.Goal = TargetTile;
	Request.MaxCost = MovementRadius;

	if (PathFinder->FindPath(Request, Path))
	{
		bInMotion = true;
		TimePassed = 0.f;

		//Clean the current tile from the path
		Path.Remove(CurrentTile);	

		if (Path.Num() != 0)
		{
			PathToTravel = Path;
			NextTile = (Path.Num() == 0 ? nullptr : PathToTravel.Pop());
			NextTileLoc = (NextTile ? NextTile->Location : FVector::ZeroVector);

			DistanceToTravel = (CurrentTileLoc - NextTileLoc).Size();
			CurrentTile->LeaveTile(GetOwner());
			
			return true;
		}
	}
	return false;
}

void UGridMovementComponent::AbortMovement()
{
	bInMotion = false;
	PathToTravel.Reset();
}

void UGridMovementComponent::PauseMovement()
{
	bPaused = true;
}

void UGridMovementComponent::ResumeMovement()
{
	bPaused = false;
}

bool UGridMovementComponent::CanMoveToTile(UHexGridTile* TargetTile, TArray<UHexGridTile*>& PotentialPath)
{
	ensure(PathFinder);

	PotentialPath.Reset();

	FGridPathFinderRequest Request;
	Request.Sender = GetOwner();
	Request.Start = CurrentTile;
	Request.Goal = TargetTile;
	Request.MaxCost = MovementRadius;

	if (Grid->FindPath(Request, PotentialPath))
		return true;
	return false;
}

TArray<UHexGridTile*> UGridMovementComponent::GetAllReachableTiles(TArray<UHexGridTile*>& ReachableTiles)
{
	ensure(PathFinder);

	ReachableTiles.Reset();

	FGridPathFinderRequest Request;
	Request.Sender = GetOwner();
	Request.Start = CurrentTile;
	Request.Goal = nullptr;
	Request.MaxCost = MovementRadius;

	PathFinder->GetReachableTiles(Request, ReachableTiles);

	return ReachableTiles;
}

void UGridMovementComponent::SetShowMovableArea(bool bShow)
{
	TArray<UHexGridTile*> ReachableTiles;
	GetAllReachableTiles(ReachableTiles);

	for (UHexGridTile* Tile : ReachableTiles)
	{
		if (bShow)
		{
			Tile->SetTileState(this, ETileState::ShowAsMovable, ETileStateLayers::HoverMovement);
		}
		else
		{
			Tile->ResetTileState(this);
		}
	}
}

void UGridMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (NextTile)
	{
		TimePassed += DeltaTime;
		float BlendFactor = TimePassed / (DistanceToTravel / MovementSpeed);

		if (BlendFactor < 1)
		{
			FVector TransitionLocation = (1 - BlendFactor) * CurrentTileLoc + BlendFactor * NextTileLoc;
			GetOwner()->SetActorLocation(TransitionLocation);
		}
		else
		{
			//Movement to next tile has been concluded
			TimePassed = 0;
			if (PathToTravel.Num()==0)
			{
				//We've reached the goal
				CurrentTile = NextTile;
				CurrentTileLoc = CurrentTile->Location;
				GetOwner()->SetActorLocation(CurrentTileLoc);
				NextTile = nullptr;
				bInMotion = false;
				CurrentTile->OccupyTile(GetOwner());
				OnComplete.Broadcast();
			}
			else
			{
				CurrentTile = NextTile;
				CurrentTileLoc = CurrentTile->Location;
				NextTile = PathToTravel.Pop();
				NextTileLoc = NextTile->Location;

				DistanceToTravel = (CurrentTileLoc - NextTileLoc).Size();
			}

		}
		FVector TransitionLocation;
	}
	else
	{
		bInMotion = false;
	}
}

bool UGridMovementComponent::IsComponentTickEnabled() const
{
	return bInMotion && !bPaused;
}

void UGridMovementComponent::OnRegister()
{
	Super::OnRegister();

	PathFinder = NewObject<UGridPathFinderAgent>(this, TEXT("GridMovementComponent_PathFinder"));
}

void UGridMovementComponent::OnUnregister()
{	
	Super::OnUnregister();

	PathFinder = nullptr;
}

void UGridMovementComponent::CollectActions(TArray<UGridActionBase*>& OutActions)
{
	OutActions.Add(NewObject<UGridAction_Move>(this, TEXT("GridAction_Move")));
}
