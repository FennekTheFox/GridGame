#pragma once

#include "MyGridPainter.generated.h"

class UHexGridTile;
class AGridActor;
class UHexGridTile;

UCLASS(DisplayName = "Default Grid Painter")
class UMyGridPainter : public UObject/*, FTickableGameObject*/
{
	GENERATED_BODY()

		~UMyGridPainter();

public:
	/**
	 * Updates a specific tile, use this when the state of a small subset of tiles changes
	 */
	void UpdateTile(UHexGridTile* Tile);
	/**
	 * Updates the entire grid by iterating through every single tile
	 */
	void UpdateGrid();
	/**
	 * Function to register the grid with the grid painter
	 */
	void SetGrid(AGridActor* Grid);
	/**
	 * Function to reset the grid painter, used when SetGrid gets called (because a new grid needs to be painted.
	 */
	void Clear();

private:
	FRotator GetTileDecalRotation();

private:
	UPROPERTY()
		AGridActor* Grid = nullptr;

	UPROPERTY()
		TMap<UHexGridTile*, UDecalComponent*> TileToDecal;
};
