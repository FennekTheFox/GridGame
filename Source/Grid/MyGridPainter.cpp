#include "MyGridPainter.h"

#include "Components/DecalComponent.h"
#include "GridActor.h"

UMyGridPainter::~UMyGridPainter()
{
	//Use object finder to find decal materials here
}

void UMyGridPainter::UpdateTile(UHexGridTile* Tile)
{
	UWorld* World = GetWorld();
	ensureAlways(World);

	UDecalComponent* DecalComp;

	if (!TileToDecal.Contains(Tile))
	{
		//Tile decal doesn't exist yet, create it first
		DecalComp = NewObject<UDecalComponent>(this);
		DecalComp->RegisterComponentWithWorld(Grid->GetWorld());
		DecalComp->SetRelativeRotation(GetTileDecalRotation());
		TileToDecal.Add(Tile, DecalComp);
	}
	else
	{
		//Tile already exists, update the tile decal here
		DecalComp = *TileToDecal.Find(Tile);
	}
	DecalComp->SetVisibility(Tile->bShouldBeVisible && Grid->bIsActive);
	DecalComp->DecalSize = FVector(1.0f, 1.0f, 1.0f) * Grid->TileSize * (1.f - Grid->TileSpacing) / 2;
	DecalComp->SetDecalMaterial(Tile->GetTileMaterial());
	DecalComp->SetWorldLocation(Tile->Location);
}

void UMyGridPainter::UpdateGrid()
{
	if (Grid)
	{
		for (auto&& KVPair : Grid->Tiles)
		{
			UHexGridTile* Tile = KVPair.Value;
			UpdateTile(Tile);
		}
	}
}

void UMyGridPainter::SetGrid(AGridActor* InGrid)
{
	Grid = InGrid;
	Clear();
	UpdateGrid();
}

void UMyGridPainter::Clear()
{
	for (auto&& KVPair : TileToDecal)
	{
		UHexGridTile* Tile = KVPair.Key;
		UDecalComponent* Decal = KVPair.Value;

		if (Decal) Decal->DestroyComponent();
	}

	TileToDecal.Reset();
}

FRotator UMyGridPainter::GetTileDecalRotation()
{
	ensure(Grid);

	if (Grid)
	{
		switch (Grid->GridOrientation)
		{
		case EHexGridRotation::YAlignedSides: return FRotator(90.0f, 0.0f, 90.0f); break;
		case EHexGridRotation::XAlignedSides: return FRotator(90.0f, 0.0f, 0.0f); break;
		default: return FRotator(0.0f, 0.0f, 0.0f);
		}
		FRotator(90.0f, 0.0f, 0.0f);
	}
	else return FRotator(0.0f, 0.0f, 0.0f);
}
