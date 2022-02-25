// Fill out your copyright notice in the Description page of Project Settings.
#include "GridActor.h"
#include "Tickable.h"
#include "Components/DecalComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include <limits>
#include "GridUtilityLibrary.h"
#include "MyGridPainter.h"
#include "Kismet/KismetMathLibrary.h"


DEFINE_LOG_CATEGORY(LogHexGrid);


void AGridActor::OnConstruction(const FTransform& Transform)
{
	CreateGrid();

	if (!GridPainter)
		GridPainter = NewObject<UMyGridPainter>(this, GridPainterClass, TEXT("GridPainter"));
	GridPainter->SetGrid(this);

	PathFinder = NewObject<UGridPathFinderAgent>();
	PathFinder->GridActor = this;
}

bool AGridActor::FindPath(FGridPathFinderRequest Request, TArray<UHexGridTile*>& Result)
{		
	return PathFinder->FindPath(Request, Result);
}

UHexGridTile* AGridActor::GetTileClosestToCoordinates(FVector Location, bool bCanBeOccupied)
{
	UHexGridTile* Closest = nullptr;
	float dmin = std::numeric_limits<float>::max();

	for (auto&& KVPair : Tiles)
	{
		UHexGridTile* temp = KVPair.Value;
		float dcur = (Location - temp->Location).Size();

		if (dcur < dmin && (!temp->IsOccupied()||bCanBeOccupied))
		{
			dmin = dcur;
			Closest = temp;
		}
	}

	//if (dmin > (TileSize / 2)) return nullptr;

	return Closest;
}

UHexGridTile* AGridActor::GetTileByCoordinates(FIntVector Coordinates)
{
	return Tiles.FindChecked(Coordinates);
}

TArray<UHexGridTile*> AGridActor::GetAllReachableNeighbours(UHexGridTile* InTile, float MaxHeightDifference /*= 100.f*/, bool bAllowFalling /*= false*/)
{
	TArray<UHexGridTile*> ret;

	FIntVector Pivot = InTile->Coords;

	//Get all numeric hex neighbours of x and y coordinates
	//FIntVector DiagonalDirection =
	//	(Pivot.X % 2 == 0 ? FIntVector(1, -1, 0) : FIntVector(1, 1, 0));
	//FIntVector DiagonalDirection = FIntVector(1, -1, 0);

	TArray<FIntVector> PossibleNeighbours;
	PossibleNeighbours.Add(FIntVector(Pivot.X + 1, Pivot.Y, 0));
	PossibleNeighbours.Add(FIntVector(Pivot.X - 1, Pivot.Y, 0));
	PossibleNeighbours.Add(FIntVector(Pivot.X, Pivot.Y + 1, 0));
	PossibleNeighbours.Add(FIntVector(Pivot.X, Pivot.Y - 1, 0));
	PossibleNeighbours.Add(FIntVector(Pivot.X -1, Pivot.Y + 1, 0));
	PossibleNeighbours.Add(FIntVector(Pivot.X +1, Pivot.Y - 1, 0));
	//PossibleNeighbours.Add(FIntVector(FIntVector(Pivot.X, Pivot.Y, 0) + DiagonalDirection));
	//PossibleNeighbours.Add(FIntVector(FIntVector(Pivot.X, Pivot.Y, 0) - DiagonalDirection));


	//Iterate through the possible neighbours until no more neighbour with different Z axis has been found
	for (int i = 0; i < PossibleNeighbours.Num(); i++)
	{
		int z = 0;

		UHexGridTile** FoundTile = nullptr;
		FIntVector Key = PossibleNeighbours[i];
		FoundTile = Tiles.Find(Key);

		if(FoundTile)
			ret.Add(*FoundTile);

			//Height check disabled for now because it keeps doing null pointer access
		//UE_LOG(LogTemp, Log, TEXT("%s"), *Key.ToString())
		//while (FoundTile && InTile);
		//{
		//	UE_LOG(LogTemp, Log, TEXT("%p, %f"), FoundTile, FoundTile->Location.Z)
		//	//If we dont allow falling, the absolute Z difference cant exceed MaxHeightDifference
		//	if (!bAllowFalling && FMath::Abs(FoundTile->Location.Z - InTile->Location.Z) >= MaxHeightDifference) continue;
		//	//If we allow falling, the Z of our target cant exceed the pivots Z by more than MaxHeightDifference
		//	if (bAllowFalling && (FoundTile->Location.Z - InTile->Location.Z) >= MaxHeightDifference) continue;

		//	ret.Add(FoundTile);

		//	z++;
		//	Key = PossibleNeighbours[i] + FIntVector(0, 0, z);
		//	FoundTile = Tiles.FindChecked(Key);
		//}


	}

	return ret;
}

void AGridActor::ToggleActive()
{
	SetIsActive(!bIsActive);
}

void AGridActor::SetIsActive(bool bNewActive)
{
	bIsActive = bNewActive;
	GridPainter->UpdateGrid();
}

// Sets default values
AGridActor::AGridActor()
{
	PrimaryActorTick.bCanEverTick = true;
	if(GridPainterClass == nullptr) GridPainterClass = UMyGridPainter::StaticClass();

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));


}

// Called when the game starts or when spawned
void AGridActor::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AGridActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGridActor::CreateGrid()
{
	ClearGrid();
	InitTileOffset();

	for (int x = -GridRadius; x <= GridRadius; x++)
	{
		for (int y = -GridRadius; y <= GridRadius; y++)
		{
			if(UGridUtilityLibrary::GetHexDistance(FIntVector(0,0,0), FIntVector(x, y, 0)) <= GridRadius )
				CreateTilesAtCoordinates(FIntPoint(x, y));
		}
	}
}

void AGridActor::ClearGrid()
{
	Tiles.Reset();
}

void AGridActor::CreateTilesAtCoordinates(FIntPoint Coords)
{
	TArray<FHitResult> Hits;
	LineTraceAt(Coords, Hits);

	for (int i = 0; i < Hits.Num(); i++)
	{
		//Test for grids that'd be too close to one another here

		UHexGridTile* newTile = NewObject<UHexGridTile>(this);
		newTile->Coords = FIntVector(Coords.X, Coords.Y, i);
		newTile->Location = Hits[i].Location;
		newTile->Size = TileSize;
		newTile->Grid = this;

		Tiles.Add(FIntVector(Coords.X, Coords.Y, i), newTile);
	}
}

void AGridActor::LineTraceAt(FIntPoint Coords, TArray<FHitResult>& Results)
{
	FVector Start = GetCoordsCenter(Coords);
	FVector End = Start;

	Start.Z = Start.Z + TraceDistance / 2;
	End.Z = End.Z - TraceDistance / 2;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = { UEngineTypes::ConvertToObjectType(ECC_WorldStatic) };
	ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel1);
	TArray<AActor*> IgnoreActors;

	UKismetSystemLibrary::LineTraceMultiForObjects((UObject*)GetWorld(), Start, End, ObjectTypes, true, IgnoreActors, EDrawDebugTrace::None, Results, true);

	//for (auto Hit : Hits)
	//{
	//	if(Hit.Actor)
	//}
}

FVector AGridActor::GetCoordsCenter(FIntPoint Coords)
{
	FVector pX, pY;
	if (GridOrientation == EHexGridRotation::XAlignedSides)
	{
		pX = Coords.X * TileSize * FVector(1.0f, 0.0f, 0.0f);
		pY = Coords.Y * TileSize * FVector(1.0f, 3.0f/2.0f, 0.0f)/2;
	}
	else
	{
		pY = Coords.Y * TileSize * FVector(0.0f, 1.0f, 0.0f);
		pX = Coords.X * TileSize * FVector(3.0f / 2.0f, 1.0f, 0.0f)/2;
	}	

	return pX + pY + Offset;

	//float X, Y;
	/*if (GridOrientation == EHexGridRotation::XAlignedSides)
	{
		X = (Coords.X + ((Coords.Y % 2) / 2.0f)) * TileSize;
		Y = TileSize * 3.f / 2.f * Coords.Y / 2.0f;
	}
	else
	{
		X = TileSize * 3.f / 2.f * Coords.X / 2;
		Y = (Coords.Y + ((Coords.X % 2) / 2.0f)) * TileSize;
	}*/

	//return FVector(X, Y, 0.f) + Offset;
}


//Initialises the tile offset for alternating coordinate pattern
void AGridActor::InitTileOffset()
{
	Offset = GetActorLocation();

	//if (GridOrientation == EHexGridRotation::XAlignedSides)
	//{
	//	//Offset to recenter the tile around the grid actor location
	//	Offset.X = -(GridDimensions.X - 1 + (((GridDimensions.Y - 1) % 2) / 2.0f)) * TileSize / 2.f + Loc.X;
	//	Offset.Y = -(TileSize * 3.f / 2.f * (GridDimensions.Y - 1) / 4.0f) + Loc.Y;
	//}
	//else
	//{
	//	Offset.X = -(TileSize * 3.f / 2.f * (GridDimensions.X - 1) / 4.0f) + Loc.X;
	//	Offset.Y = -(GridDimensions.Y - 1 + (((GridDimensions.X - 1) % 2) / 2.0f)) * TileSize / 2.f + Loc.Y;
	//}
	//Offset.Z = Loc.Z;
}


void UHexGridTile::SetTileState(const UObject* Agent, TEnumAsByte<ETileState> NewState, int32 InLayer)
{
	ShowState.SetTileState(Agent, NewState, InLayer);
	UpdateTile();
}


void UHexGridTile::ResetTileState(const UObject* Agent)
{
	ShowState.ResetTileState(Agent);
	UpdateTile();
}

TEnumAsByte<ETileState> UHexGridTile::GetTileState()
{
	return ShowState.GetTileState();
}

bool UHexGridTile::LeaveTile(AActor* _OccupyingUnit)
{
	if (OccupyingUnit == _OccupyingUnit)
	{
		OccupyingUnit = nullptr;
		return true;
	}
	return false;
}

AActor* UHexGridTile::GetOccupyingUnit()
{
	return OccupyingUnit;
}

bool UHexGridTile::IsOccupied()
{
	return OccupyingUnit!=nullptr;
}

void UHexGridTile::UpdateTile()
{
	Grid->GridPainter->UpdateTile(this);
}

void UHexGridTile::GetNeighbours(TArray<UHexGridTile*>& Out)
{
	Out = Grid->GetAllReachableNeighbours(this);
}

UMaterialInterface* UHexGridTile::GetTileMaterial()
{
	if (!Grid) return nullptr;

	switch (GetTileState())
	{
	case ETileState::ShowAsDefault: return Grid->DefaultMaterial;
	case ETileState::ShowAsHovered: return Grid->HoveredMaterial;
	case ETileState::ShowAsPath: return Grid->PathIndicationMaterial;
	case ETileState::ShowAsInvalid: return Grid->InvalidMaterial;
	case ETileState::ShowAsMovable: return Grid->MovableMaterial;
	case ETileState::ShowAsAttackable: return Grid->AttackableMaterial;
	}
	return nullptr;
}



void FTileStateLayer::SetTileStateForLayer(const UObject* Agent, TEnumAsByte<ETileState> NewState)
{
	auto* ExistingEntry = LayerContent.FindByPredicate([=](TPair<const UObject*, ETileState> entry)
		{
			return entry.Key == Agent;
		});

	if (ExistingEntry)
	{
		if (NewState != ShowAsDefault)
		{
			//Overwrite the existing state
			ExistingEntry->Value = NewState;
		}else
		{
			//Remove the existing entry, since ShowAsDefault should not be an overwrite state;
			ResetTileStateForLayer(Agent);
		}
	}
	else
	{
		if (NewState != ShowAsDefault)
		{	
			//Create new entry for this layer
			LayerContent.Add(TPair<const UObject*, TEnumAsByte<ETileState>>(Agent, NewState));
		}
	}
}

void FTileStateLayer::ResetTileStateForLayer(const UObject* Agent)
{
	auto* ExistingEntry = LayerContent.FindByPredicate([=](TPair<const UObject*, ETileState> entry)
		{
			return entry.Key == Agent;
		});

	if (ExistingEntry)
	{
		auto temp = *ExistingEntry;
		LayerContent.Remove(temp);
	}
}

TEnumAsByte<ETileState> FTileStateLayer::GetTileStateForLayer()
{
	if (LayerContent.Num() == 0)
	{
		return ShowAsDefault;
	}
	else
	{
		return LayerContent[LayerContent.Num() - 1].Value;
	}
}

void FTileState::SetTileState(const UObject* Agent, TEnumAsByte<ETileState> NewState, int32 LayerIndex)
{
	ResetTileState(Agent);

	auto* LayerRef = Layers.Find(LayerIndex);
	if (LayerRef)
	{
		LayerRef->SetTileStateForLayer(Agent, NewState);
	}
	else
	{
		Layers.Add(LayerIndex, FTileStateLayer());
		LayerRef = Layers.Find(LayerIndex);
		LayerRef->SetTileStateForLayer(Agent, NewState);
	}
}

void FTileState::ResetTileState(const UObject* Agent)
{
	for (auto&& KVPair : Layers)
	{
		KVPair.Value.ResetTileStateForLayer(Agent);
	}

	int i = 0;
	TArray<int32> LayerKeys;
	Layers.GetKeys(LayerKeys);
	while(i < LayerKeys.Num())
	{
		auto* Layer = Layers.Find(LayerKeys[i]);
		if (Layer->LayerContent.Num() == 0)
		{
			Layers.Remove(LayerKeys[i]);
		}

		i++;
	}
}

TEnumAsByte<ETileState> FTileState::GetTileState()
{
	if (Layers.Num() == 0)
		return ETileState::ShowAsDefault;

	TArray<int32> LayerKeys;
	Layers.GetKeys(LayerKeys);
	int32 MaxLayer, index;
	UKismetMathLibrary::MaxOfIntArray(LayerKeys, index, MaxLayer);

	return Layers.Find(MaxLayer)->GetTileStateForLayer();
}
