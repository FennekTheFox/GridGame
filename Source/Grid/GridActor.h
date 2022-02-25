// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridPathFinder.h"
#include "GridActor.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogHexGrid, Log, All);

class UGridPathFinderAgent;
class UMyGridPainter;

enum ETileStateLayers
{
	Base = 0,
	HoverAttack = 10,
	HoverMovement = 11,
	ShowPath = 20,
	Selector = 30
};

UENUM()
enum ETileState
{
	ShowAsDefault,
	ShowAsHovered,
	ShowAsSelected,
	ShowAsMovable,
	ShowAsAttackable,
	ShowAsPath,
	ShowAsInvalid,
};

USTRUCT()
struct FTileStateLayer
{
	GENERATED_BODY()

	void SetTileStateForLayer(const UObject* Agent, TEnumAsByte<ETileState> NewState);
	void ResetTileStateForLayer(const UObject* Agent);
	TEnumAsByte<ETileState> GetTileStateForLayer();

	TArray<TPair<const UObject*, TEnumAsByte<ETileState>>> LayerContent;
};


USTRUCT()
struct FTileState
{
	GENERATED_BODY()

	void SetTileState(const UObject* Agent, TEnumAsByte<ETileState> NewState, int32 Layer);
	void ResetTileState(const UObject* Agent);
	TEnumAsByte<ETileState> GetTileState();

public:
	UPROPERTY()
		TMap<int32, FTileStateLayer> Layers;

private:
};

UCLASS(BlueprintType, DisplayName = "Hexagonal Grid Tile")
class UHexGridTile : public UObject
{
	GENERATED_BODY()


public:
	UFUNCTION(BlueprintCallable, Category = "Tile State")
		void SetTileState(UPARAM(ref)const UObject* Agent, TEnumAsByte<ETileState> NewState, int32 Layer = 0);
	UFUNCTION(BlueprintCallable, Category = "Tile State")
		void ResetTileState(UPARAM(ref)const UObject* Agent);
	UFUNCTION(BlueprintPure, Category = "Tile State")
		TEnumAsByte<ETileState> GetTileState();

	UFUNCTION(BlueprintCallable, Category = "Tile Occupation")
		void OccupyTile(AActor* _OccupyingUnit) { OccupyingUnit = _OccupyingUnit; }
	UFUNCTION(BlueprintCallable, Category = "Tile Occupation")
		bool LeaveTile(AActor* _OccupyingUnit);
	UFUNCTION(BlueprintPure, Category = "Tile Occupation")
		AActor* GetOccupyingUnit();
	UFUNCTION(BlueprintPure, Category = "Tile Occupation")
		bool IsOccupied();

public:
	UMaterialInterface* GetTileMaterial();
	void UpdateTile();
	bool Equals(UHexGridTile* other)
	{
		return Coords.X == other->Coords.X && Coords.Y == other->Coords.Y && Coords.Z == other->Coords.Z;
	}
	void GetNeighbours(TArray<UHexGridTile*>& Out);


public:
	UPROPERTY(BlueprintReadOnly)
		AGridActor* Grid;
	UPROPERTY(BlueprintReadOnly)
		FIntVector Coords;
	UPROPERTY(BlueprintReadOnly)
		FVector Location;

	float Size;
	bool bShouldBeVisible = true;


private:
	//Display state stack for the tile. projects from whoever wants to influence the tile to the state 
	//they want it to be. most recent
	FTileState ShowState;
	UPROPERTY()
		AActor* OccupyingUnit = nullptr;
};

UENUM()
enum EHexGridRotation
{
	XAlignedSides,
	YAlignedSides
};

UCLASS(hidecategories = ("Physics", "Rendering", "Replication" ,"Collision", "Input", "Actor", "LOD", "Cooking"))
class GRID_API AGridActor : public AActor
{
	GENERATED_BODY()

	AGridActor();

public:
	UPROPERTY(EditAnywhere, Category="Grid")
		int GridRadius = 2;
	//UPROPERTY(EditAnywhere, Category="Grid")
	//	FIntPoint GridDimensions = FIntPoint(2, 2);
	UPROPERTY(EditAnywhere, Category = "Grid")
		float TileSize = 100.0f;
	UPROPERTY(EditAnywhere, Category = "Grid", meta=(ClampMin=0.0f, ClampMax=1.0f))
		float TileSpacing = 0.02f;
	UPROPERTY(EditAnywhere, Category = "Grid")
		TEnumAsByte<EHexGridRotation> GridOrientation;


	UPROPERTY(EditAnywhere, Category = "GridSettings")
		TSubclassOf<UMyGridPainter> GridPainterClass = nullptr;
	UPROPERTY(EditAnywhere, Category = "GridSettings")
		UMaterialInterface* DefaultMaterial;
	UPROPERTY(EditAnywhere, Category = "GridSettings")
		UMaterialInterface* HoveredMaterial;
	UPROPERTY(EditAnywhere, Category = "GridSettings")
		UMaterialInterface* MovableMaterial;
	UPROPERTY(EditAnywhere, Category = "GridSettings")
		UMaterialInterface* AttackableMaterial;
	UPROPERTY(EditAnywhere, Category = "GridSettings")
		UMaterialInterface* PathIndicationMaterial;
	UPROPERTY(EditAnywhere, Category = "GridSettings")
		UMaterialInterface* InvalidMaterial;

	//Object that paints the grid, public so that tiles can access it and request an update
		UPROPERTY()
			UMyGridPainter* GridPainter = nullptr;
		UPROPERTY()
			UGridPathFinderAgent* PathFinder = nullptr;

public:
	UFUNCTION(BlueprintCallable)
		bool FindPath(FGridPathFinderRequest Request, TArray<UHexGridTile*>& Result);
	UFUNCTION(BlueprintPure)
		UHexGridTile* GetTileClosestToCoordinates(FVector Location, bool bCanBeOccupied = false);
	UFUNCTION(BlueprintPure)
		UHexGridTile* GetTileByCoordinates(FIntVector Coordinates);
	UFUNCTION(BlueprintPure)
		TArray<UHexGridTile*> GetAllReachableNeighbours(UHexGridTile* InTile, float MaxHeightDifference = 100.f, bool bAllowFalling = false);

	UFUNCTION(BlueprintCallable)
		void ToggleActive();
	UFUNCTION(BlueprintCallable)
		void SetIsActive(bool bNewActive);

public:
	virtual void Tick(float DeltaTime) override;
	void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;


private:
	void CreateGrid();
	void ClearGrid();
	void CreateTilesAtCoordinates(FIntPoint Coords);

	void LineTraceAt(FIntPoint Coords, TArray<FHitResult>& Results);
	FVector GetCoordsCenter(FIntPoint Coords);
	void InitTileOffset();

private:
	UPROPERTY()
		USceneComponent* Root;
public:
	UPROPERTY()
		TMap<FIntVector, UHexGridTile*> Tiles;


	float TraceDistance = 10000.0f;
	FVector Offset;
	bool bIsActive = true;;
};
